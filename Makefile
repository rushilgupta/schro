CC=g++
CFLAGS= -c -Wall

all: filter
filter: 1BloomFilter.o Bitvector.o murmur3.o
	$(CC) 1BloomFilter.o Bitvector.o murmur3.o -o filter
1BloomFilter.o: 1BloomFilter.cpp
	$(CC) $(CFLAGS) 1BloomFilter.cpp
Bitvector.o: Bitvector.cpp
	$(CC) $(CFLAGS) Bitvector.cpp
murmur3.o: murmur3.cpp
	$(CC) $(CFLAGS) murmur3.cpp
clean:
	rm -rf *o filter

