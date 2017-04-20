#ifndef _BITVECTOR_H_
#define _BITVECTOR_H_

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
		Bitvector(int items);

		void set(int index);

		/** return 1 for true **/
		int isSet(int index);

		~Bitvector();
};

#endif

