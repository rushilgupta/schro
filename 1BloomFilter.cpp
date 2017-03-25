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

class Bloomfilter
{
	private:
		Bitvector *bv;
		int size;
		int hashFunctionCount;
		int seed;
		static const double LOG2 = 0.69314718056;
		static const int MAX_ELEMENTS_SUPPORTED = 1<<10; // ~1m
		static const int DEFAULT_SIZE = 1<<13; // 8KB

		/** 
		* Generate hash for data, 
		* Thanks to https://github.com/aappleby/smhasher for awesome implementation
		**/
		int generateHash(long data)
		{
			int hash = MurmurHash2(&data, 64, seed);
			if(hash < 0)
			{
				return hash*(-1);
			}
			return hash;
		}
	public:
		Bloomfilter()
		{
			size = DEFAULT_SIZE;
			bv = new Bitvector(size);
			hashFunctionCount = (size*LOG2)/MAX_ELEMENTS_SUPPORTED;
			srand(time(NULL));
			seed = rand()%1000;
		}
		Bloomfilter(int nBits)
		{
			size = nBits;
			bv = new Bitvector(size);
			hashFunctionCount = (size*LOG2)/MAX_ELEMENTS_SUPPORTED;
		}
		void add(long key)
		{
			int index = generateHash(key);
			bv->set(index);
		}
		/** return 1 for true **/
		int probablyContains(long key)
		{
			int index = generateHash(key);
			return bv->isSet(index);
		}
		int getFalsePositiveRate()
		{
			return 0;
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
	return 0;
}

