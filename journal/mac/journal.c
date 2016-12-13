#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "display.h"
#include "image.h"

#define PIPE_DIR "/tmp"
#define PIPE_NAME "oneshot-pipe"
#define PIPE_PATH PIPE_DIR "/" PIPE_NAME
#define MESSAGE_BUF_SIZE 256

#define PAGES \
    PAGE(default) \
    PAGE(c1) \
    PAGE(c2) \
    PAGE(c3) \
    PAGE(c4) \
    PAGE(c5) \
    PAGE(c6) \
    PAGE(c7) \
    PAGE(s1) \
    PAGE(s2) \
    PAGE(s3) \
    PAGE(s4) \
    PAGE(t1) \
    PAGE(t2) \
    PAGE(t3) \
    PAGE(t4) \
    PAGE(t5) \
    PAGE(t6) \
    PAGE(t7) \
    PAGE(t8) \
    PAGE(t9) \
    PAGE(t10) \
    PAGE(t11) \
    PAGE(t12) \
    PAGE(t13) \
    PAGE(t14) \
    PAGE(t15) \
    PAGE(t16) \
    PAGE(final) \
    PAGE(save)

struct ThreadData {
    struct Display *dpy;
    xcb_window_t window;
    xcb_gcontext_t gc;
    xcb_pixmap_t *masks;
    xcb_pixmap_t *images;
    volatile int page;
};

void set_page(struct ThreadData *threaddata, const char *name)
{
    int page = 0;
#define PAGE(x) if (strncmp(name, #x, MESSAGE_BUF_SIZE) == 0) { goto end; } else { ++page; }
    PAGES
#undef PAGE

end:
    threaddata->page = page;
    xcb_shape_mask(threaddata->dpy->c, ShapeSet, ShapeBounding,
                   threaddata->window, 0, 0, threaddata->masks[page]);
    xcb_copy_area(threaddata->dpy->c,
                  threaddata->images[page],
                  threaddata->window,
                  threaddata->gc,
                  0, 0, 0, 0, JOURNAL_WIDTH, JOURNAL_HEIGHT);
    xcb_flush(threaddata->dpy->c);
}

static void *ipc_thread(void *data)
{
    struct ThreadData *threaddata = (struct ThreadData*)data;

    for (;;) {
        // Try to open pipe
        int pipe = open(PIPE_PATH, O_RDONLY);
        if (pipe == -1) {
            // Use the default image for now
            set_page(threaddata, "default");
            // Wait until the pipe appears to try again
            int inotify = inotify_init();
            inotify_add_watch(inotify, PIPE_DIR, IN_CREATE);
            char inotify_buf[sizeof(struct inotify_event) + NAME_MAX + 1];
            for (;;) {
                ssize_t result = read(inotify, inotify_buf, sizeof(inotify_buf));
                if (result > 0) {
                    struct inotify_event *e = (struct inotify_event*)inotify_buf;
                    if (strcmp(e->name, PIPE_NAME) == 0)
                        break;
                } else {
                    break;
                }
            }
            close(inotify);
        } else {
            // Message loop
            char message[MESSAGE_BUF_SIZE];
            for (;;) {
                ssize_t len = read(pipe, message, sizeof(message));
                if (len > 0) {
                    set_page(threaddata, message);
                } else if (len == 0) {
                    exit(0); // empty string
                } else {
                    // End of the line
                    break;
                }
            }
            close(pipe);
        }
    }

    return NULL;
}

int do_journal(struct Display *dpy)
{
    // Connect EWMH
    xcb_ewmh_connection_t ewmh;
    xcb_ewmh_init_atoms_replies(&ewmh, xcb_ewmh_init_atoms(dpy->c, &ewmh), NULL);

    // Request some atoms
    xcb_intern_atom_cookie_t WM_DELETE_WINDOW_cookie = xcb_intern_atom(dpy->c, false, STRING("WM_DELETE_WINDOW"));
    xcb_intern_atom_cookie_t _NET_WM_PING_cookie = xcb_intern_atom(dpy->c, false, STRING("_NET_WM_PING"));
    xcb_intern_atom_cookie_t WM_PROTOCOLS_cookie = xcb_intern_atom(dpy->c, false, STRING("WM_PROTOCOLS"));
    xcb_atom_t WM_DELETE_WINDOW = xcb_intern_atom_reply(dpy->c, WM_DELETE_WINDOW_cookie, NULL)->atom;
    xcb_atom_t _NET_WM_PING = xcb_intern_atom_reply(dpy->c, _NET_WM_PING_cookie, NULL)->atom;
    xcb_atom_t WM_PROTOCOLS = xcb_intern_atom_reply(dpy->c, WM_PROTOCOLS_cookie, NULL)->atom;

    // Create window
    uint32_t values[] = {
        XCB_EVENT_MASK_EXPOSURE, // event mask
    };
    xcb_window_t window = xcb_generate_id(dpy->c);
    xcb_create_window(dpy->c, XCB_COPY_FROM_PARENT, window, dpy->screen->root,
                      0, 0, JOURNAL_WIDTH, JOURNAL_HEIGHT,
                      0, XCB_WINDOW_CLASS_INPUT_OUTPUT, dpy->screen->root_visual,
                      XCB_CW_EVENT_MASK, &values);

    xcb_atom_t protocols[] = {
        WM_DELETE_WINDOW,
        _NET_WM_PING,
    };
    xcb_icccm_set_wm_protocols(dpy->c, window, WM_PROTOCOLS, ARRAY(protocols));

    xcb_icccm_set_wm_name(dpy->c, window, XCB_ATOM_STRING, 8, STRING(""));
    xcb_icccm_set_wm_class(dpy->c, window, ARRAY("_______\0OneshotJournal"));
    xcb_ewmh_set_wm_window_type(&ewmh, window, 1, &ewmh._NET_WM_WINDOW_TYPE_NORMAL);

    xcb_size_hints_t size_hints;
    xcb_icccm_size_hints_set_min_size(&size_hints, JOURNAL_WIDTH, JOURNAL_HEIGHT);
    xcb_icccm_size_hints_set_max_size(&size_hints, JOURNAL_WIDTH, JOURNAL_HEIGHT);
    xcb_icccm_set_wm_size_hints(dpy->c, window, XCB_ATOM_WM_NORMAL_HINTS, &size_hints);

    // Create pixmaps
    xcb_pixmap_t masks[] = {
#define PAGE(x) create_mask(dpy->c, window, x##_mask, JOURNAL_WIDTH, JOURNAL_HEIGHT),
        PAGES
#undef PAGE
    };
    xcb_pixmap_t images[] = {
#define PAGE(x) create_image(dpy->c, window, x##_rgb, JOURNAL_WIDTH, JOURNAL_HEIGHT),
        PAGES
#undef PAGE
    };

    // Create graphics context
    xcb_gcontext_t gc = xcb_generate_id(dpy->c);
    xcb_create_gc(dpy->c, gc, window, 0, NULL);

    // Start IPC thread
    struct ThreadData threaddata = {
        dpy,
        window,
        gc,
        masks,
        images,
        0,
    };
    pthread_t thread;
    if (pthread_create(&thread, NULL, ipc_thread, &threaddata) != 0)
        return 1;

    // Map!
    xcb_map_window(dpy->c, window);
    xcb_flush(dpy->c);

    // Handle events
    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(dpy->c))) {
        switch (e->response_type & ~0x80) {
        case XCB_EXPOSE: {
            xcb_expose_event_t *ee = (xcb_expose_event_t*)e;
            xcb_copy_area(dpy->c, images[threaddata.page],
                          window, gc,
                          ee->x, ee->y, ee->x, ee->y,
                          ee->width, ee->height);
            xcb_flush(dpy->c);
        } break;
        case XCB_CLIENT_MESSAGE: {
            xcb_client_message_event_t *ce = (xcb_client_message_event_t*)e;
            if (ce->type == WM_PROTOCOLS && ce->format == 32) {
                xcb_atom_t atom = ce->data.data32[0];
                if (atom == WM_DELETE_WINDOW) {
                    goto end;
                } else if (atom == _NET_WM_PING) {
                    ce->window = dpy->screen->root;
                    xcb_send_event(dpy->c, false, dpy->screen->root,
                                   XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                                   (const char*)e);
                }
            }
        } break;
        }
        free(e);
    }

end:
    xcb_free_gc(dpy->c, gc);
    for (size_t i = 0; i < ARRAY_SIZE(masks); ++i) {
        xcb_free_pixmap(dpy->c, masks[i]);
        xcb_free_pixmap(dpy->c, images[i]);
    }
    return 0;
}
