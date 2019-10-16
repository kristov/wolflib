#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <vga.h>

void usage() {
    printf("Usage: vgadump [OPTION]...\n");
    printf("Show information about Wolfenstein 3D images\n\n");
    printf("  --help        Show this help\n");
    printf("  --map=M       dump map number M\n");
    printf("  --vgadict=S   VGADICT.xxx file name\n");
    printf("  --vgahead=S   VGAHEAD.xxx file name\n");
    printf("  --vgagraph=S  VGAGRAPH.xxx file name\n");
    printf("\n");
    return;
}

int main(int argc, char *argv[]) {
    char vgadict[256];
    char vgahead[256];
    char vgagraph[256];

    memset(&vgadict[0], 0, 256);
    memset(&vgahead[0], 0, 256);
    memset(&vgagraph[0], 0, 256);

    struct option longopts[] = {
        {"help", 0, NULL, 0},
        {"vgadict", 1, NULL, 1},
        {"vgahead", 1, NULL, 2},
        {"vgagraph", 1, NULL, 3},
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
                    if (strlen(optarg) > 255) {
                        printf("vgadict filename too long\n");
                        exit(-1);
                    }
                    strcpy(vgadict, optarg);
                }
                break;
            case 2:
                if (optarg) {
                    if (strlen(optarg) > 255) {
                        printf("vgahead filename too long\n");
                        exit(-1);
                    }
                    strcpy(vgahead, optarg);
                }
                break;
            case 3:
                if (optarg) {
                    if (strlen(optarg) > 255) {
                        printf("vgagraph filename too long\n");
                        exit(-1);
                    }
                    strcpy(vgagraph, optarg);
                }
                break;
        }
    }

    struct vga_ctx ctx;
    vga_init(&ctx, vgadict, vgahead, vgagraph);

    return 0;
}
