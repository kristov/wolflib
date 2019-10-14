#ifndef H_MAP
#define H_MAP
#include <stdint.h>

#define NUMMAPS 60
#define PLANESIZE 8192

struct map_head {
    uint16_t RLEWtag;
    int32_t headeroffsets[100];
    uint8_t tileinfo[1];
} __attribute__ ((packed));

struct map_seg {
    int32_t planestart[3];
    uint16_t planelength[3];
    uint16_t width;
    uint16_t height;
    char name[16];
};

struct map_planes {
    uint16_t plane_0[PLANESIZE];
    uint16_t plane_1[PLANESIZE];
};

struct map_ctx {
    struct map_head head;
    struct map_seg seg[NUMMAPS];
    uint16_t lastmap;
};

int map_init(struct map_ctx* ctx, const char* maphead, const char* gamemaps);

int fill_map_planes(struct map_ctx* ctx, const char* gamemaps, uint8_t map_nr, struct map_planes* planes);

#endif
