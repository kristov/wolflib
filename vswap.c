#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // memset
#include <file.h>
#include <vswap.h>

int vswap_init(struct vswap_ctx* ctx, const char* vswap) {
    int fd = open(vswap, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "could not open %s\n", vswap);
        return -1;
    }

    struct vswap_header header;
    read(fd, &header, sizeof(int16_t) * 3);

    fprintf(stderr, "chunks_in_file: %d\n", header.chunks_in_file);
    fprintf(stderr, "sprite_start: %d\n", header.sprite_start);
    fprintf(stderr, "sound_start: %d\n", header.sound_start);

    struct vswap_chunk* chunks = malloc(sizeof(struct vswap_chunk) * header.chunks_in_file);
    memset(chunks, 0, sizeof(struct vswap_chunk) * header.chunks_in_file);
    for (uint16_t i = 0; i < header.chunks_in_file; i++) {
        read(fd, &chunks[i].offset, sizeof(int32_t));
    }
    for (uint16_t i = 0; i < header.chunks_in_file; i++) {
        read(fd, &chunks[i].length, sizeof(int16_t));
    }

    for (uint16_t i = 0; i < header.chunks_in_file; i++) {
        fprintf(stderr, "  offset: %d, length: %d\n", chunks[i].offset, chunks[i].length);
    }

    close(fd);
    return 0;
}
