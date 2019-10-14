# wolflib: Utilities for wolf3d

Problem: couldn't find code for decoding wolf3d map files that compiled on Linux. Solution: cobble together different bits of code into something that seems to work:

    git clone 
    cd wolflib
    make

## Sources

I got my sources from [AlumiuN/Wolf4SDL](https://github.com/AlumiuN/Wolf4SDL.git) and [XWolf](https://devinsmith.net/backups/xwolf/mapdump.html).

## Synopsis

    #include <map.h>

    // Create a context structure
    struct map_ctx ctx;

    // Init it (reads map information)
    map_init(&ctx, "MAPHEAD.WL6", "GAMEMAPS.WL6");

    // Create structure for storing the two planes
    struct map_planes planes;

    // Pick a map number
    uint8_t map = 5;

    // Fill the planes with data
    fill_map_planes(&ctx, "GAMEMAPS.WL6", map, &planes);

## Example

See `mapdump.c` for example usage.
