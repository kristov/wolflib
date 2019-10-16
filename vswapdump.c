#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <vswap.h>

void usage() {
    printf("Usage: vswapdump [OPTION]...\n");
    printf("Show information about Wolfenstein 3D sprites\n\n");
    printf("  --help        Show this help\n");
    printf("  --vswap=S     VSWAP.xxx file name\n");
    printf("\n");
    return;
}

int main(int argc, char *argv[]) {
    char vswap[256];

    memset(&vswap[0], 0, 256);

    struct option longopts[] = {
        {"help", 0, NULL, 0},
        {"vswap", 1, NULL, 1},
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
                        printf("vswap filename too long\n");
                        exit(-1);
                    }
                    strcpy(vswap, optarg);
                }
                break;
        }
    }

    struct vswap_ctx ctx;
    vswap_init(&ctx, vswap);

    return 0;
}
