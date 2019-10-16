#include <stdio.h>
#include <stdlib.h> // malloc
#include <file.h>
#include <vga.h>

struct version_info {
    uint8_t nr_starts;
    uint8_t nr_pics;
    const char* name;
};

#define VERSIONS 2
struct version_info versions[] = {
    {159, 132, "WOLF3D FULL"},
    {170, 147, "SPEAR OF DESTINY"}
};

struct huff_node {
    uint16_t bit0;
    uint16_t bit1;
};

#define NUMCHUNKS 149

struct huff_node grhuffman[255];

//static int32_t grstarts[NUMCHUNKS + 1];
#define lengthof(x) (sizeof(x) / sizeof(*(x)))

static struct version_info* find_version(uint8_t nr_starts) {
    for (uint8_t i; i < VERSIONS; i++) {
        if (versions[i].nr_starts == nr_starts) {
            return &versions[i];
        }
    }
    return &versions[0];
}

static void CAL_HuffExpand(uint8_t* source, uint8_t* dest, uint32_t length, struct huff_node* hufftable) {
    if (!length || !dest) {
        fprintf(stderr, "length or dest is null!");
        return;
    }

    struct huff_node* headptr = hufftable + 254;
    struct huff_node* huffptr = headptr;
    uint8_t* end = dest + length;

    uint8_t val = *source++;
    uint8_t mask = 1;
    uint16_t nodeval;
    while(1) {
        if (!(val & mask)) {
            nodeval = huffptr->bit0;
        }
        else {
            nodeval = huffptr->bit1;
        }
        if (mask == 0x80) {
            val = *source++;
            mask = 1;
        }
        else {
            mask <<= 1;
        }

        if (nodeval < 256) {
            *dest++ = (uint8_t)nodeval;
            huffptr = headptr;
            if (dest >= end) {
                break;
            }
        }
        else {
            huffptr = hufftable + (nodeval - 256);
        }
    }
}

int vga_init(struct vga_ctx* ctx, const char* vgadict, const char* vgahead, const char* vgagraph) {
    int fd = open(vgadict, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "could not open %s\n", vgadict);
        return -1;
    }

    read(fd, grhuffman, sizeof(grhuffman));
    close(fd);

    fd = open(vgahead, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "could not open %s\n", vgahead);
        return -1;
    }

    int32_t header_size = lseek(fd, 0, SEEK_END);
    uint8_t nr_starts = header_size / 3;
    lseek(fd, 0, SEEK_SET);

    struct version_info* version = find_version(nr_starts);
    uint8_t nr_pics = version->nr_pics;

    fprintf(stderr, "detected version: %s\n", version->name);

    uint8_t* header_data = malloc(sizeof(uint8_t) * header_size);
    if (header_data == NULL) {
        fprintf(stderr, "unable to malloc\n");
        return -1;
    }
    read(fd, header_data, header_size);
    close(fd);

    int32_t* grstarts = malloc(sizeof(int32_t) * nr_starts);
    if (grstarts == NULL) {
        fprintf(stderr, "unable to malloc\n");
        free(header_data);
        return -1;
    }

    uint8_t* d = header_data;
    uint32_t val;
    for (uint8_t i = 0; i < nr_starts; i++) {
        val = d[0] | d[1] << 8 | d[2] << 16;
        grstarts[i] = val;
        d += 3;
    }

    fd = open(vgagraph, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "could not open %s\n", vgagraph);
        free(header_data);
        free(grstarts);
        return -1;
    }

    struct vga_pic_table* pic_table = malloc(sizeof(struct vga_pic_table) * nr_pics);
    if (pic_table == NULL) {
        fprintf(stderr, "unable to malloc\n");
        return -1;
    }

    int32_t chunkcomplen = 0;
    int32_t chunkexplen = 0;

    lseek(fd, grstarts[0], SEEK_SET);
    read(fd, &chunkexplen, sizeof(chunkexplen));
    chunkcomplen = grstarts[1] - grstarts[0] - 4;

    fprintf(stderr, "chunkcomplen: %d\n", chunkcomplen);
    uint8_t* compseg = malloc(chunkcomplen);
    if (compseg == NULL) {
        fprintf(stderr, "unable to malloc\n");
        free(header_data);
        free(grstarts);
        free(pic_table);
    }

    read(fd, compseg, chunkcomplen);
    CAL_HuffExpand(compseg, (uint8_t*)pic_table, sizeof(struct vga_pic_table) * nr_pics, grhuffman);

    free(header_data);
    free(grstarts);
    free(pic_table);
    free(compseg);

    fprintf(stderr, "nr_pics: %d\n", nr_pics);
    for (uint8_t i = 0; i < nr_pics; i++) {
        struct vga_pic_table entry = pic_table[i];
        fprintf(stderr, "%d x %d\n", entry.width, entry.height);
    }
    return 0;
}
