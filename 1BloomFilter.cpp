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

		/** 
		* Generate hash for data, 
		* Thanks to https://github.com/aappleby/smhasher for awesome implementation
		**/
		uint32_t generateHash(string key, int seed)
		{
			uint32_t hash[1];
			MurmurHash3_x86_32(key.c_str(), 64, seed, hash);
			if(*hash < 0)
			{
				return -*hash;
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

			m_seeds = new int[2];
			srand(time(NULL));
			for(int i=0; i<2; i++)
			{
				m_seeds[i] = rand()%1000;
			}
		}

		void add(string key)
		{
			uint32_t hashA = generateHash(key, m_seeds[0]);
			uint32_t hashB = generateHash(key, m_seeds[1]);
			for(int i=0; i<m_hashFunctionCount; i++)
			{
				int index = (hashA + i*(hashB%m_bits))%m_bits;
				m_bv->set(index);
			}
		}

		/** 0 means definitely not there, 1 means probably there **/
		int probablyContains(string key)
		{
			uint32_t hashA = generateHash(key, m_seeds[0]);
			uint32_t hashB = generateHash(key, m_seeds[1]);
			for(int i=0; i<m_hashFunctionCount; i++)
			{
				int index = (hashA + i*(hashB%m_bits))%m_bits;
				if(!m_bv->isSet(index))
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

/* a 32 letter string */
string getRandomString()
{
	char randomString[32];
	for(int i=0; i<32; i++)
	{
		randomString[i] = rand()%128 + 'a';
	}
	return std::string(randomString);
}

/* run a dry run of bloom filter performance with m bits and n elements */
void test(int m, int n)
{
	set<string> myset;
	Bloomfilter *bf = new Bloomfilter(m, n);
	for(int i=0; i<n; i++)
	{
		string randomString = getRandomString();
		//cout<<randomString;
		myset.insert(randomString);
		bf->add(randomString);
	}
	int counter = 0;
	int testUniverseSize = 4*n;
	for(int i=0; i<testUniverseSize; i++)
	{
		string randomString = getRandomString();
		if(myset.find(randomString) == myset.end() && bf->probablyContains(randomString) == 1) 
		{
			counter++;
		}
	}
	float fp = (float)counter/testUniverseSize;
	float epsilon = fp - bf->getFalsePositiveRate();
	delete bf;

	cout << "found FP : "<< fp << endl;
	cout << "expected FP : "<< bf->getFalsePositiveRate() << endl;
	cout << "epsilon : " << epsilon << endl; 
	if ( epsilon <= 0.0001f)
	{
		cout << "dry-run test passed" << endl;
	}
	else
	{
		cerr << "test failed" << endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc <= 2)
	{
		test(1<<25, 1<<22);
	}
	return 0;
}
