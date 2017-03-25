CC=g++
CFLAGS= -c -Wall

all: filter
filter: 1BloomFilter.o MurmurHash2.o
	$(CC) 1BloomFilter.o MurmurHash2.o -o filter
1BloomFilter.o: 1BloomFilter.cpp
	$(CC) $(CFLAGS) 1BloomFilter.cpp
MurmurHash2.o: MurmurHash2.cpp
	$(CC) $(CFLAGS) MurmurHash2.cpp
clean:
	rm -rf *o filter

