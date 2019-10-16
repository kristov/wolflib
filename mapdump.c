#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <map.h>

#define AREATILE 107
#define NUMAREAS 37
#define ELEVATORTILE 21
#define ALTELEVATORTILE 107

static void print_mapsegs(struct map_ctx* ctx) {
    int32_t pos;
    for (uint8_t i = 0; i < NUMMAPS; i++) {
        pos = ctx->head.headeroffsets[i];
        struct map_seg* seg = &ctx->seg[i];
        printf("map %d\n", i);
        printf("  header at %d\n", pos);
        printf("    planestart[0]  %5d\n", seg->planestart[0]);
        printf("    planestart[1]  %5d\n", seg->planestart[1]);
        printf("    planelength[0] %5d\n", seg->planelength[0]);
        printf("    planelength[1] %5d\n", seg->planelength[1]);
        printf("    width          %5d\n", seg->width);
        printf("    height         %5d\n", seg->height);
        printf("    name '%s'\n", seg->name);
    }
}

void print_map(struct map_planes* planes) {
    uint16_t tile;
    //uint16_t ting;
    for (uint8_t y = 0; y < 64; y++) {
        printf ("%2d: ", y);
        for (uint8_t x = 0; x < 64; x++) {
            tile = planes->plane_0[y * 64 + x];
            //ting = planes->plane_1[y * 64 + x];
            if (tile < AREATILE) {
                printf("%02x", tile);
            }
            else if (tile < AREATILE + NUMAREAS) {
                printf("  "); // floor area
            }
            else {
                printf("??");
            }
        }
        printf("\n");
    }
}

void usage() {
    printf("Usage: mapdump [OPTION]...\n");
    printf("Show information about Wolfenstein 3D maps\n\n");
    printf("  --help        Show this help\n");
    printf("  --map=M       dump map number M\n");
    printf("  --maphead=S   MAPHEAD.xxx file name\n");
    printf("  --gamemaps=S  GAMEMAPS.xxx file name\n");
    printf("\n");
    return;
}

int main(int argc, char *argv[]) {
    uint8_t map = 0;
    char maphead[256];
    char gamemaps[256];

    memset(&maphead[0], 0, 256);
    memset(&gamemaps[0], 0, 256);

    struct option longopts[] = {
        {"help", 0, NULL, 0},
        {"map",  1, NULL, 1},
        {"maphead", 1, NULL, 2},
        {"gamemaps", 1, NULL, 3},
        {0, 0, 0, 0}
    };

    if (argc < 1) {
        usage();
        return 0;
    }

    while (1) {
        int c;
        c = getopt_long(argc, argv, "", longopts, NULL);
        if (c == EOF) {
            break;
        }
        switch (c) {
            case '?':
                break;
            case 0:
                usage();
                break;
            case 1:
                if (optarg) {
                    map = atoi(optarg);
                }
                break;
            case 2:
                if (optarg) {
                    if (strlen(optarg) > 255) {
                        printf("maphead filename too long\n");
                        exit(-1);
                    }
                    strcpy(maphead, optarg);
                }
                break;
            case 3:
                if (optarg) {
                    if (strlen(optarg) > 255) {
                        printf("gamemaps filename too long\n");
                        exit(-1);
                    }
                    strcpy(gamemaps, optarg);
                }
                break;
        }
    }

    struct map_ctx ctx;
    map_init(&ctx, maphead, gamemaps);
    print_mapsegs(&ctx);

    struct map_planes planes;
    fill_map_planes(&ctx, gamemaps, map, &planes);
    print_map(&planes);

    return 0;
}
