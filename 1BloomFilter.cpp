#include <iostream>
#include <string>
#include <time.h>  
#include <math.h>
#include <stdlib.h>
#include "MurmurHash2.h"

using namespace std;

/** Unsigned char-> BitVector abstraction **/
class Bitvector
{
	private:
		unsigned char *bitset;
		int size;
		/** Bytes per char **/
		static const int BPC = 8;

	public:
		Bitvector(int items)
		{
			size = items;
			bitset = new unsigned char[(size/BPC)+1];
		}

		void set(int index)
		{
			int arrPosition = index/BPC;
			int maskPosition =  index%BPC;
			bitset[arrPosition] |= (1<<maskPosition);
		}

		/** return 1 for true **/
		int isSet(int index)
		{
			int arrPosition = index/BPC;
			int maskPosition =  index%BPC;
			return (bitset[arrPosition] & (1<<maskPosition)) > 0 ? 1 : 0;
		}

		~Bitvector()
		{
			delete bitset;
		}
};

/*** Currently supporting upto 5 hash functions ***/
class Bloomfilter
{
	private:
		Bitvector *bv;
		int bits;
		int hashFunctionCount;
		int *seeds;

		static const double LOG2 = 0.69314718056;
		static const double E = 2.71828182845;
		static const int MAX_ELEMENTS_SUPPORTED = 1<<16; // 65k
		static const int DEFAULT_SIZE = 1<<13; // 1KB

		/** 
		* Generate hash for data, 
		* Thanks to https://github.com/aappleby/smhasher for awesome implementation
		**/
		int generateHash(long key, int hashFnIndex)
		{
			int hash = MurmurHash2(&key, 64, seeds[hashFnIndex]);
			if(hash < 0)
			{
				return hash*(-1);
			}
			return hash;
		}

	public:
		Bloomfilter()
		{
			bits = DEFAULT_SIZE;
			bv = new Bitvector(bits);
			hashFunctionCount = (bits*LOG2)/MAX_ELEMENTS_SUPPORTED;

			seeds = new int[hashFunctionCount];
			srand(time(NULL));
			for(int i=0; i<hashFunctionCount; i++)
			{
				seeds[i] = rand()%1000; 
			}
		}

		Bloomfilter(int nBits)
		{
			bits = nBits;
			bv = new Bitvector(bits);
			hashFunctionCount = (bits*LOG2)/MAX_ELEMENTS_SUPPORTED;

			seeds = new int[hashFunctionCount];
			srand(time(NULL));
			for(int i=0; i<hashFunctionCount; i++)
			{
				seeds[i] = rand()%1000; 
			}
		}

		void add(long key)
		{
			for(int i=0; i<hashFunctionCount; i++)
			{
				int index = generateHash(key, seeds[i]);
				bv->set(index%bits);
			}
		}

		/** 0 means definitely not there, 1 means probably there **/
		int probablyContains(long key)
		{
			for(int i=0; i<hashFunctionCount; i++)
			{
				int index = generateHash(key, seeds[i]);
				if(!bv->isSet(index%bits))
				{
					return 0;
				}
			}
			return 1;
		}

		float getFalsePositiveRate()
		{
			float x = ((float)MAX_ELEMENTS_SUPPORTED/bits)*hashFunctionCount;
			float base = pow(E,x);
			return pow((1-base), hashFunctionCount);
		}

		~Bloomfilter()
		{
			delete bv;
		}
};

int main()
{
	Bitvector *bv = new Bitvector(8);
	for(int i=0; i<8; i++)
	{
		if(i%2 == 0) bv->set(i);
	}
	for(int i=0; i<8; i++)
	{
		cout<<bv->isSet(i)<<endl;
	}
	Bloomfilter *bf = new Bloomfilter();
	bf->add(19900);
	bf->add(2000);
	cout<<bf->getFalsePositiveRate()<<endl;
	return 0;
}
