#include "GC.h"

#include <crtdbg.h>

inline void
allocatorTest()
{
	using namespace tcii::ex;

	Allocator::initialize();

	auto t1 = Allocator::allocate<int>(2);
	auto t2 = Allocator::allocate<int>(5);
	auto t3 = Allocator::allocate<int>(8);
	auto t4 = Allocator::allocate<int>(52);
	auto t5 = Allocator::allocate<int>(2);

	Allocator::printMemoryMap();
	
	Allocator::free(t4);
	auto t6 = Allocator::allocate<int>(30);
	Allocator::printMemoryMap();

	Allocator::free(t1);
	Allocator::free(t2);
	Allocator::free(t3);
	Allocator::free(t5);
	Allocator::free(t6);
	Allocator::printMemoryMap();
	Allocator::exit();
}

//
// Main function
//
int
main()
{
	_CrtMemState sOld;
	_CrtMemState sNew;
	_CrtMemState sDiff;
	_CrtMemCheckpoint(&sOld); //take a snapshot

	allocatorTest();
	_CrtMemCheckpoint(&sNew); //take a snapshot 
	if (_CrtMemDifference(&sDiff, &sOld, &sNew)) // if there is a difference
	{
		printf("-----------_CrtMemDumpStatistics ---------\n");
		_CrtMemDumpStatistics(&sDiff);
		printf("-----------_CrtMemDumpAllObjectsSince ---------\n");
		_CrtMemDumpAllObjectsSince(&sOld);
		printf("-----------_CrtDumpMemoryLeaks ---------\n");
		_CrtDumpMemoryLeaks();
	}
	return 0;
}
