#ifndef _1BLOOMFILTER_H_
#define _1BLOOMFILTER_H_

#include <iostream>
#include <set>
#include <math.h>
#include <stdlib.h>
#include "murmur3.h"
#include "Bitvector.h"

using namespace std;

class Bloomfilter
{
	private:
		Bitvector *m_bv;
		int m_bits;
		int m_hashFunctionCount;
		int m_maxElementsSupported;
		int *m_seeds;

		static const double LOG2 = 0.69314718056;
		static const double E = 2.71828182845;
		static const int MAX_ELEMENTS_SUPPORTED = 1<<20; // 1m
		static const int DEFAULT_SIZE = 1<<23; // 1MB

		uint32_t generateHash(string key, int seed);

	public:		
		Bloomfilter();
		Bloomfilter(int nBits);
		Bloomfilter(int nBits, int nElements);

		void add(string key);
		/** 0 means definitely not there, 1 means probably there **/
		int probablyContains(string key);
		float getFalsePositiveRate();

		~Bloomfilter();
};

#endif

