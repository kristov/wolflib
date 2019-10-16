#ifndef VGA_H
#define VGA_H
#include <stdint.h>

struct vga_pic_table { // pictabletype
    uint16_t width;
    uint16_t height;
};

struct vga_font { // fontstruct
    uint16_t height;
    int16_t location[256];
    int8_t width[256];
};

struct vga_ctx {
    uint16_t nothing;
};

int vga_init(struct vga_ctx* ctx, const char* vgadict, const char* vgahead, const char* vgagraph);

#endif
