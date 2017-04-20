#include "Bitvector.h"

Bitvector::Bitvector(int items)
{
	m_size = items;
	m_bitset = new unsigned char[(m_size/BPC)+1];
}

void Bitvector::set(int index)
{
	int arrPosition = index/BPC;
	int maskPosition =  index%BPC;
	m_bitset[arrPosition] |= (1<<maskPosition);
}

int Bitvector::isSet(int index)
{
	int arrPosition = index/BPC;
	int maskPosition = index%BPC;
	return (m_bitset[arrPosition] & (1<<maskPosition)) > 0 ? 1 : 0;
}

Bitvector::~Bitvector()
{
	delete m_bitset;
}

