CC=g++
CFLAGS= -c -Wall

all: filter
filter: 1BloomFilter.o murmur3.o
	$(CC) 1BloomFilter.o murmur3.o -o filter
1BloomFilter.o: 1BloomFilter.cpp
	$(CC) $(CFLAGS) 1BloomFilter.cpp
murmur3.o: murmur3.cpp
	$(CC) $(CFLAGS) murmur3.cpp
clean:
	rm -rf *o filter

