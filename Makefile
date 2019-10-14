CC=gcc
CFLAGS=-Wall -Werror -ggdb

ALL =
ALL += map.o

all: $(ALL) mapdump wolflib.so

map.o: map.c map.h
	$(CC) $(CFLAGS) -fPIC -I. -c -o $@ $<

mapdump: mapdump.c map.o
	$(CC) $(CFLAGS) -I. -o $@ $< map.o

wolflib.so: $(ALL)
	$(CC) $(CFLAGS) -shared -o $@ -fPIC $(ALL)

clean:
	rm -f $(ALL) mapdump wolflib.so
