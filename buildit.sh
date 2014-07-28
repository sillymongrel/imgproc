CC=g++
CFLAGS=-Wall -g

image.o:
    $(CC) $(CFLAGS) -c image.c

histeq: image.o
    $(CC) $(CFLAGS) image.o histeq.c -o histeq

roi: image.o
    $(CC) $(CFLAGS) image.o roi.c -o roi

all: histeq roi

clean:
    rm -fr *.o histeq roi

    
