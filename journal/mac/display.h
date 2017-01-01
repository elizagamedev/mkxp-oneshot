#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

struct Display {
    xcb_connection_t *c;
    xcb_screen_t *screen;
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define STRING(x) sizeof(x) - 1, (x)
#define ARRAY(x) sizeof(x), (x)

#endif // DISPLAY_H
