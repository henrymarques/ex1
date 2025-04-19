#include <cstdio>

#include "GC.h"

namespace tcii::ex
{ // begin namespace tcii::ex


//////////////////////////////////////////////////////////
//
// Allocator implementation
// =========
Allocator* Allocator::_instance;

void
Allocator::initialize(unsigned heapSize)
{
	if (_instance != nullptr) {
		return;
	}

	_instance = new Allocator();
	_instance->_first = (BlockInfo*) new char[2 * sizeof(BlockInfo) + heapSize];
	*(_instance->_first) = { 0, heapSize, nullptr, nullptr };
	*(BlockInfo*)(((char*)(_instance->_first + 1)) + heapSize) = { 0, 0, _instance->_first, nullptr };
}

template <typename T> T*
Allocator::allocate(unsigned size)
{
	
}

void
Allocator::printMemoryMap()
{
	printf("%d\n", _instance->_first->size);
}

void
Allocator::exit()
{
	delete _instance->_first;
	delete _instance;
}

} // begin namespace tcii::ex
