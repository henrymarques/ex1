#include "GC.h"

inline void
allocatorTest()
{
	using namespace tcii::ex;

	Allocator::initialize();
	Allocator::printMemoryMap();
	Allocator::exit();
}

//
// Main function
//
int
main()
{
	allocatorTest();
	return 0;
}
