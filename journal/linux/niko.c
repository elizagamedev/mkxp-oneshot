#include <sys/time.h>

#include "display.h"
#include "image.h"

static uint64_t getticks()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}

static void draw_niko(xcb_connection_t *c,
                      xcb_window_t win,
                      xcb_gcontext_t gc,
                      xcb_pixmap_t masks[3],
                      xcb_pixmap_t images[3],
                      int x, int y, int w, int h,
                      int offset)
{
    int frame;
    if (offset % 32 >= 16) {
        frame = 1;
    } else {
        if ((offset / 32) % 2)
            frame = 0;
        else
            frame = 2;
    }
    xcb_shape_mask(c, ShapeSet, ShapeBounding, win, 0, 0, masks[frame]);
    xcb_copy_area(c, images[frame], win, gc, x, y, x, y, w, h);

}

int do_niko(struct Display *dpy, int x, int y)
{
    // Create window
    uint32_t values[] = {
        true, // override_redirect
        XCB_EVENT_MASK_EXPOSURE, // event mask
    };
    xcb_window_t window = xcb_generate_id(dpy->c);
    xcb_create_window(dpy->c, XCB_COPY_FROM_PARENT, window, dpy->screen->root,
                      x, y, NIKO_WIDTH, NIKO_HEIGHT,
                      0, XCB_WINDOW_CLASS_INPUT_OUTPUT, dpy->screen->root_visual,
                      XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK, &values);

    // Stick window on top
    xcb_stack_mode_t stack = XCB_STACK_MODE_ABOVE;
    xcb_configure_window(dpy->c, window, XCB_CONFIG_WINDOW_STACK_MODE, &stack);

    // Create pixmaps
    xcb_pixmap_t niko_masks[] = {
        create_mask(dpy->c, window, niko1_mask, NIKO_WIDTH, NIKO_HEIGHT),
        create_mask(dpy->c, window, niko2_mask, NIKO_WIDTH, NIKO_HEIGHT),
        create_mask(dpy->c, window, niko3_mask, NIKO_WIDTH, NIKO_HEIGHT),
    };
    xcb_pixmap_t niko_images[] = {
        create_image(dpy->c, window, niko1_rgb, NIKO_WIDTH, NIKO_HEIGHT),
        create_image(dpy->c, window, niko2_rgb, NIKO_WIDTH, NIKO_HEIGHT),
        create_image(dpy->c, window, niko3_rgb, NIKO_WIDTH, NIKO_HEIGHT),
    };

    // Create graphics context
    xcb_gcontext_t gc = xcb_generate_id(dpy->c);
    xcb_create_gc(dpy->c, gc, window, 0, NULL);

    // Map!
    xcb_map_window(dpy->c, window);
    xcb_flush(dpy->c);

    // Niko movement info
    int niko_offset = 0;

    uint64_t ticks = getticks();
    for (;;) {
        // Handle events
        for (;;) {
            xcb_generic_event_t *e = xcb_poll_for_event(dpy->c);
            if (e) {
                switch (e->response_type) {
                case XCB_EXPOSE: {
                    xcb_expose_event_t *ee = (xcb_expose_event_t*)e;
                    draw_niko(dpy->c, window, gc, niko_masks, niko_images,
                              ee->x, ee->y, ee->width, ee->height, niko_offset);
                    xcb_flush(dpy->c);
                } break;
                }
            } else if (getticks() - ticks >= 1000 / 60) {
                break;
            }
        }

        // Move niko
        niko_offset += 2;
        int new_y = y + niko_offset;
        if (new_y > dpy->screen->height_in_pixels)
            break;
        xcb_configure_window(dpy->c, window, XCB_CONFIG_WINDOW_Y, &new_y);
        draw_niko(dpy->c, window, gc, niko_masks, niko_images, 0, 0, NIKO_WIDTH, NIKO_HEIGHT, niko_offset);
        xcb_flush(dpy->c);
        ticks = getticks();
    }

    // Free up stuff
    xcb_free_gc(dpy->c, gc);
    for (int i = 0; i < 3; ++i) {
        xcb_free_pixmap(dpy->c, niko_masks[i]);
        xcb_free_pixmap(dpy->c, niko_images[i]);
    }

    return 0;
}
