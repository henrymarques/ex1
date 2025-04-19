#ifndef __BLOCK_h
#define __BLOCK_h

#include "GC.h"

namespace tcii::ex
{ // begin namespace tcii::ex

class Block
{
public:
	Block(unsigned size) {
		header = (BlockInfo*) new char[2 * sizeof(BlockInfo) + size];
		blockStart = (char*)(header + 1);
		footer = (BlockInfo*)(blockStart + size);
	}

private:
	// Pointer to start of allocated memory (after header)
	char* blockStart;
	BlockInfo* header;
	BlockInfo* footer;
};
}

#endif
