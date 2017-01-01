#ifndef EMBED_H
#define EMBED_H

#include <xcb/xcb_image.h>
#include <xcb/shape.h>
#include <X11/extensions/shape.h>

static inline xcb_pixmap_t create_mask(xcb_connection_t *c,
                                       xcb_window_t win,
                                       unsigned char *bits,
                                       uint32_t width,
                                       uint32_t height)
{
    return xcb_create_pixmap_from_bitmap_data(c, win, bits, width, height, 1, 0, 0, NULL);
}

static inline xcb_pixmap_t create_image(xcb_connection_t *c,
                                        xcb_window_t win,
                                        unsigned char *bytes,
                                        uint32_t width,
                                        uint32_t height)
{
    xcb_image_t *image = xcb_image_create_native(
                c, width, height, XCB_IMAGE_FORMAT_Z_PIXMAP, 24, NULL, width * height * 4, bytes);
    xcb_pixmap_t pixmap = xcb_generate_id(c);
    xcb_create_pixmap(c, 24, pixmap, win, width, height);
    xcb_gcontext_t gc = xcb_generate_id(c);
    xcb_create_gc(c, gc, pixmap, 0, NULL);
    xcb_image_put(c, pixmap, gc, image, 0, 0, 0);
    xcb_image_destroy(image);
    xcb_free_gc(c, gc);
    return pixmap;

}

static inline void set_shape(xcb_connection_t *c, xcb_window_t win, xcb_pixmap_t mask)
{
    xcb_shape_mask(c, ShapeSet, ShapeBounding, win, 0, 0, mask);
}

#define NIKO_WIDTH 48
#define NIKO_HEIGHT 64
extern unsigned char niko1_mask[];
extern unsigned char niko1_rgb[];
extern unsigned char niko2_mask[];
extern unsigned char niko2_rgb[];
extern unsigned char niko3_mask[];
extern unsigned char niko3_rgb[];

#define JOURNAL_WIDTH 800
#define JOURNAL_HEIGHT 600
extern unsigned char default_mask[];
extern unsigned char default_rgb[];
extern unsigned char c1_mask[];
extern unsigned char c1_rgb[];
extern unsigned char c2_mask[];
extern unsigned char c2_rgb[];
extern unsigned char c3_mask[];
extern unsigned char c3_rgb[];
extern unsigned char c4_mask[];
extern unsigned char c4_rgb[];
extern unsigned char c5_mask[];
extern unsigned char c5_rgb[];
extern unsigned char c6_mask[];
extern unsigned char c6_rgb[];
extern unsigned char c7_mask[];
extern unsigned char c7_rgb[];
extern unsigned char s1_mask[];
extern unsigned char s1_rgb[];
extern unsigned char s2_mask[];
extern unsigned char s2_rgb[];
extern unsigned char s3_mask[];
extern unsigned char s3_rgb[];
extern unsigned char s4_mask[];
extern unsigned char s4_rgb[];
extern unsigned char t1_mask[];
extern unsigned char t1_rgb[];
extern unsigned char t2_mask[];
extern unsigned char t2_rgb[];
extern unsigned char t3_mask[];
extern unsigned char t3_rgb[];
extern unsigned char t4_mask[];
extern unsigned char t4_rgb[];
extern unsigned char t5_mask[];
extern unsigned char t5_rgb[];
extern unsigned char t6_mask[];
extern unsigned char t6_rgb[];
extern unsigned char t7_mask[];
extern unsigned char t7_rgb[];
extern unsigned char t8_mask[];
extern unsigned char t8_rgb[];
extern unsigned char t9_mask[];
extern unsigned char t9_rgb[];
extern unsigned char t10_mask[];
extern unsigned char t10_rgb[];
extern unsigned char t11_mask[];
extern unsigned char t11_rgb[];
extern unsigned char t12_mask[];
extern unsigned char t12_rgb[];
extern unsigned char t13_mask[];
extern unsigned char t13_rgb[];
extern unsigned char t14_mask[];
extern unsigned char t14_rgb[];
extern unsigned char t15_mask[];
extern unsigned char t15_rgb[];
extern unsigned char t16_mask[];
extern unsigned char t16_rgb[];
extern unsigned char final_mask[];
extern unsigned char final_rgb[];
extern unsigned char save_mask[];
extern unsigned char save_rgb[];

#endif // EMBED_H
