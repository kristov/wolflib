#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <map.h>

#define NEARTAG 0xa7
#define FARTAG 0xa8

static inline uint16_t READWORD(uint8_t* ptr) {
    uint16_t val = ptr[0] | ptr[1] << 8;
    return val;
}

void CAL_CarmackExpand(uint8_t* source, uint16_t* dest, uint16_t length) {
    uint16_t ch, chhigh, count, offset;
    uint8_t* inptr;
    uint16_t* copyptr;
    uint16_t* outptr;

    length /= 2;

    inptr = source;
    outptr = dest;

    while (length > 0) {
        ch = READWORD(inptr); inptr += 2;
        chhigh = ch >> 8;
        if (chhigh == NEARTAG) {
            count = ch & 0xff;
            if (!count) {
                ch |= *inptr++;
                *outptr++ = ch;
                length--;
            }
            else {
                offset = *inptr++;
                copyptr = outptr - offset;
                length -= count;
                while (count--)
                    *outptr++ = *copyptr++;
            }
        }
        else if (chhigh == FARTAG) {
            count = ch & 0xff;
            if (!count) {
                ch |= *inptr++;
                *outptr++ = ch;
                length--;
            }
            else {
                offset = READWORD(inptr); inptr += 2;
                copyptr = dest + offset;
                length -= count;
                while (count--)
                    *outptr++ = *copyptr++;
            }
        }
        else {
            *outptr++ = ch;
            length--;
        }
    }
}

void CA_RLEWexpand(uint16_t *source, uint16_t *dest, int32_t length, uint16_t rlewtag) {
    uint16_t value, count, i;
    uint16_t *end = dest + length / 2;

    do {
        value = *source++;
        if (value != rlewtag) {
            *dest++ = value;
        }
        else {
            count = *source++;
            value = *source++;
            for (i=1;i<=count;i++) {
                *dest++ = value;
            }
        }
    } while (dest < end);
}

static void fill_plane(struct map_seg* seg, uint8_t plane, uint16_t rlew_tag, int fd, uint16_t* dest) {
    uint32_t pos = seg->planestart[plane];
    uint32_t csize = seg->planelength[plane];

    uint16_t* bigbufferseg = (uint16_t*)malloc(sizeof(uint8_t) * csize);
    if (bigbufferseg == NULL) {
        fprintf(stderr, "can not allocate memory!\n");
        return;
    }
    uint16_t* source = bigbufferseg;

    lseek(fd, pos, SEEK_SET);
    read(fd, (uint8_t*)source, csize);

    uint32_t expanded = *source;
    source++;

    uint16_t* buffer2seg = (uint16_t*)malloc(sizeof(uint8_t) * expanded);
    if (buffer2seg == NULL) {
        fprintf(stderr, "can not allocate memory!\n");
        return;
    }

    CAL_CarmackExpand((uint8_t*)source, buffer2seg, expanded);
    CA_RLEWexpand(buffer2seg + 1, dest, PLANESIZE, rlew_tag);
    free(buffer2seg);
    free(bigbufferseg);
}

int fill_map_planes(struct map_ctx* ctx, const char* gamemaps, uint8_t map_nr, struct map_planes* planes) {
    int fd = open(gamemaps, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "can not open %s\n", gamemaps);
        return -1;
    }

    struct map_seg* seg = &ctx->seg[map_nr];

    fill_plane(seg, 0, ctx->head.RLEWtag, fd, &planes->plane_0[0]);
    fill_plane(seg, 1, ctx->head.RLEWtag, fd, &planes->plane_1[0]);

    return 0;
}

static int read_mapsegs(struct map_ctx* ctx, const char* gamemaps) {
    int handle = open(gamemaps, O_RDONLY);
    if (handle == -1) {
        fprintf(stderr, "can not open %s\n", gamemaps);
        return -1;
    }

    int32_t pos;
    for (uint8_t i = 0; i < NUMMAPS; i++) {
        pos = ctx->head.headeroffsets[i];
        if (pos <= 0) {
            break;
        }
        ctx->lastmap = i;
        lseek(handle, pos, SEEK_SET);
        read(handle, &ctx->seg[i], sizeof(struct map_seg));
    }
    return 0;
}

static int filelength(int handle) {
    struct stat statbuf;
    if (fstat(handle, &statbuf) < 0) {
        fprintf(stderr, "can not read file length\n");
        return 0;
    }
    return statbuf.st_size;
}

static int read_maphead(struct map_ctx* ctx, const char* maphead) {
    int handle = open(maphead, O_RDONLY);
    if (handle == -1) {
        fprintf(stderr, "can not open %s\n", maphead);
        return -1;
    }
    uint32_t length = filelength(handle);
    int n = read(handle, &ctx->head, length);
    if (n != length) {
        fprintf(stderr, "can not read from %s\n", maphead);
        return -1;
    }
    close(handle);
    return 0;
}

int map_init(struct map_ctx* ctx, const char* maphead, const char* gamemaps) {
    memset(ctx, 0, sizeof(struct map_ctx));
    if (read_maphead(ctx, maphead) < 0) {
        return -1;
    }
    if (read_mapsegs(ctx, gamemaps) < 0) {
        return -1;
    }
    return 0;
}
