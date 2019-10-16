CC=gcc
CFLAGS=-Wall -Werror -ggdb

BINS =
BINS += mapdump
BINS += vgadump
BINS += vswapdump

ALL =
ALL += map.o
ALL += vga.o
ALL += vswap.o

all: $(ALL) $(BINS) wolflib.so

%.o: %.c %.h
	$(CC) $(CFLAGS) -fPIC -I. -c -o $@ $<

mapdump: mapdump.c map.o
	$(CC) $(CFLAGS) -I. -o $@ $< map.o

vgadump: vgadump.c vga.o
	$(CC) $(CFLAGS) -I. -o $@ $< vga.o

vswapdump: vswapdump.c vswap.o
	$(CC) $(CFLAGS) -I. -o $@ $< vswap.o

wolflib.so: $(ALL)
	$(CC) $(CFLAGS) -shared -o $@ -fPIC $(ALL)

clean:
	rm -f $(ALL) $(BINS) wolflib.so
