#ifndef VSWAP_H
#define VSWAP_H
#include <stdint.h>

struct vswap_header {
    int16_t chunks_in_file;
    int16_t sprite_start;
    int16_t sound_start;
};

struct vswap_chunk {
    int32_t offset;
    int16_t length;
};

struct vswap_ctx {
    uint8_t nothing;
};

int vswap_init(struct vswap_ctx* ctx, const char* vswap);

#endif
