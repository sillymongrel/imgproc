CC=g++
CFLAGS=-Wall -g

image.o:
	$(CC) $(CFLAGS) -c image.c

filter.o: 
	$(CC) $(CFLAGS) -c filter.c

histeq.o:
	$(CC) $(CFLAGS) -c histeq.c

roi.o:
	$(CC) $(CFLAGS) -c roi.c

histeq: image.o filter.o histeq.o 
	$(CC) $(CFLAGS) image.o filter.o histeq.o -o histeq


roi: image.o filter.o roi.o
	$(CC) $(CFLAGS) image.o filter.o roi.c -o roi

all: histeq roi

clean:
	rm -fr *.o histeq roi

    
