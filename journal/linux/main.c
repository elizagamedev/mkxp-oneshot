#include "display.h"

extern int do_niko(struct Display *dpy, int x, int y);
extern int do_journal(struct Display *dpy);

int main(int argc, char **argv)
{
    // Connect
    struct Display dpy;
    dpy.c = xcb_connect(NULL, NULL);
    dpy.screen = xcb_setup_roots_iterator(xcb_get_setup(dpy.c)).data;

    int result;
    if (argc == 3) {
        // argc == 3, assume niko
        int x = atoi(argv[1]);
        int y = atoi(argv[2]);
        result = do_niko(&dpy, x, y);
    } else {
        // argc == 3, assume journal
        result = do_journal(&dpy);
    }

    xcb_disconnect(dpy.c);
    return result;
}
