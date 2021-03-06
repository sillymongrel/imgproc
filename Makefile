CC=clang++
CFLAGS=-Wall -g -Werror -fno-exceptions

image.o:
	$(CC) $(CFLAGS) -c image.cc

filter.o: 
	$(CC) $(CFLAGS) -c filter.cc

histeq.o:
	$(CC) $(CFLAGS) -c histeq.cc

roi.o:
	$(CC) $(CFLAGS) -c roi.cc

histeq: image.o filter.o histeq.o 
	$(CC) $(CFLAGS) image.o filter.o histeq.o -o histeq


roi: image.o filter.o roi.o
	$(CC) $(CFLAGS) image.o filter.o roi.o -o roi

all: histeq roi

clean:
	rm -fr *.o histeq roi

    
