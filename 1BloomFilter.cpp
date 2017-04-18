#include <iostream>
#include <string>
#include <set>
#include <time.h>  
#include <math.h>
#include <stdlib.h>
#include "murmur3.h"

using namespace std;

/** Unsigned char-> BitVector abstraction **/
class Bitvector
{
	private:
		unsigned char *m_bitset;
		int m_size;
		/** Bytes per char **/
		static const int BPC = 8;

	public:
		Bitvector(int items)
		{
			m_size = items;
			m_bitset = new unsigned char[(m_size/BPC)+1];
		}

		void set(int index)
		{
			int arrPosition = index/BPC;
			int maskPosition =  index%BPC;
			m_bitset[arrPosition] |= (1<<maskPosition);
		}

		/** return 1 for true **/
		int isSet(int index)
		{
			int arrPosition = index/BPC;
			int maskPosition = index%BPC;
			return (m_bitset[arrPosition] & (1<<maskPosition)) > 0 ? 1 : 0;
		}

		~Bitvector()
		{
			delete m_bitset;
		}
};

/*** Currently supporting upto 5 hash functions ***/
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
		static const int MAX_ELEMENTS_SUPPORTED = 1<<13; // 8K
		static const int DEFAULT_SIZE = 1<<16; // 8KB

		/** 
		* Generate hash for data, 
		* Thanks to https://github.com/aappleby/smhasher for awesome implementation
		**/
		int generateHash(string key, int hashFnIndex)
		{
			uint32_t hash[1];
			MurmurHash3_x86_32(key.c_str(), 32, m_seeds[hashFnIndex], hash);
			cout<<"key: "<<key<<" hash: "<<*hash<<endl;
			if(*hash < 0)
			{
				return *hash*(-1);
			} 
			return *hash;
		}

	public:
		Bloomfilter()
		{
			Bloomfilter(DEFAULT_SIZE, MAX_ELEMENTS_SUPPORTED);
		}

		Bloomfilter(int nBits)
		{
			Bloomfilter(nBits, MAX_ELEMENTS_SUPPORTED);
		}

		Bloomfilter(int nBits, int nElements)
		{
			m_bits = nBits;
			m_bv = new Bitvector(m_bits);
			m_hashFunctionCount = (m_bits*LOG2)/nElements;
			m_maxElementsSupported = nElements;

			cout<<"k = "<<m_hashFunctionCount<<endl;

			m_seeds = new int[m_hashFunctionCount];
			srand(time(NULL));
			for(int i=0; i<m_hashFunctionCount; i++)
			{
				m_seeds[i] = rand()%1000;
			}
		}

		void add(string key)
		{
			for(int i=0; i<m_hashFunctionCount; i++)
			{
				int index = generateHash(key, m_seeds[i]);
				cout<<index<<"\t"<<i<<"\t"<<m_seeds[i]<<endl;
				m_bv->set(index%m_bits);
			}
		}

		/** 0 means definitely not there, 1 means probably there **/
		int probablyContains(string key)
		{
			for(int i=0; i<m_hashFunctionCount; i++)
			{
				int index = generateHash(key, m_seeds[i]);
				cout<<index<<"\t"<<i<<"\t"<<m_seeds[i]<<endl;
				if(!m_bv->isSet(index%m_bits))
				{
					return 0;
				}
			}
			return 1;
		}

		float getFalsePositiveRate()
		{
			// kn/m
			float x = ((float)m_maxElementsSupported/m_bits)*m_hashFunctionCount;
			float base = pow(E,-x);
			return pow((1-base), m_hashFunctionCount);
		}

		~Bloomfilter()
		{
			delete m_seeds;
			delete m_bv;
		}
};

/* run a dry run of bloom filter performance with m bits and n elements */
/*int test(int m, int n)
{
	set<long> myset;
	Bloomfilter *bf = new Bloomfilter(m, n);
	for(int i=0; i<1000; i++)
	{
		//long random = rand()%n;
		myset.insert(i);
		bf->add(i);
	}
	int counter = 0;
	for(int i=0; i<n; i++)
	{
		if(myset.find(i) != myset.end() && bf->probablyContains(i) == 1) 
		{
			counter++;
		}
	}
	cout<<"found FP : "<<counter<<endl;
	cout<<"expected FP : "<<bf->getFalsePositiveRate()<<endl;
	int errors = 0;
	delete bf;
	if (errors == 0)
	{
		std::cout << "all tests ok" << std::endl;
	}
	else
	{
		std::cerr << errors << " tests failed" << std::endl;
	}
	return errors;
}*/

int main(int argc, char* argv[])
{
	if (argc <= 2)
	{
		long n = 1<<13;
		Bloomfilter *bf = new Bloomfilter(1<<16, n);
		bf->add("356");
		bf->probablyContains("356");
		// test(1<<16, n);
		return 0;
	}
	cout<<"USAGE: filter <bits to use> \nOR SIMPLY: filter"<<endl;
	return 0;
}
