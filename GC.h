#ifndef __GC_h
#define __GC_h

#include <iostream>
#include <new>
#include <algorithm>

// #include "Block.h"

namespace tcii::ex
{ // begin namespace tcii::ex


//////////////////////////////////////////////////////////
//
// BlockInfo class
// =========
struct BlockInfo
{
    unsigned flag;
    unsigned size;
    BlockInfo* prev;
    BlockInfo* next;

}; // BlockInfo


//////////////////////////////////////////////////////////
//
// Allocator class
// =========
class Allocator
{
public:
    static constexpr auto minBlockSize = 16u;
    static constexpr auto heapSize = 1048576u; // bytes

    static void initialize(unsigned = heapSize);
    template <typename T> static T* allocate(unsigned = 1);
    static void free(void*);
    static void printMemoryMap();
    static void exit();

private:
    static Allocator* _instance;

    BlockInfo* _header{nullptr};
    unsigned _memory{0};

    Allocator() {};

}; // Allocator

inline void
Allocator::initialize(unsigned heapSize)
{
    if (_instance != nullptr)
    {
        return;
    }
    _instance = new Allocator();

    // allocate heap space starting at _header
    _instance->_header = (BlockInfo*) new char[2 * sizeof(BlockInfo) + heapSize];

    // set block header at _header with offset 0
    *(_instance->_header) = {0, heapSize, _instance->_header, nullptr};

    // set block footer at _header with offset 1 BlockInfo + heapSize char
    BlockInfo* footer = (BlockInfo*)((char*)(_instance->_header + 1) + heapSize);
    *footer = {0, 0, _instance->_header, nullptr};

    // points block header next to yourself
    _instance->_header->next = _instance->_header;
    // points block header prev to block header since we have only one block
    _instance->_header->prev = _instance->_header;

    _instance->_memory = heapSize;
}

template <typename T>
inline T*
Allocator::allocate(unsigned size)
{
    auto allocSize = std::max<unsigned>(sizeof(T) * size, minBlockSize);

    // search block with size s >= allocSize + 2 BlockInfo
    BlockInfo* block = _instance->_header;
    for (block = block->next; block != _instance->_header && !(allocSize <= block->size); block = block->next);

    if (allocSize + 2 * sizeof(BlockInfo) > block->size)
        throw std::bad_alloc();

    // offset to footer of existing block
    BlockInfo* blockFooter = (BlockInfo*)((char*)(block + 1) + block->size);
    // set footer to be footer of the new block
    blockFooter->flag = 1;
    blockFooter->prev = nullptr;

    // update existing block size
    block->size = block->size - allocSize - 2 * sizeof(BlockInfo);

    // negatively offset footer to fit alloc size and 2 new block infos
    BlockInfo* allocStart = (BlockInfo*)((char*)(blockFooter - 2) - allocSize);
    // set new footer to existing block
    *allocStart = {0, 0, block, nullptr};
    // set header to new block
    *(allocStart + 1) = {1, allocSize, nullptr, nullptr};

    return (T*)((BlockInfo*)allocStart + 1);
}

inline void
Allocator::printMemoryMap()
{
    using namespace std;

    unsigned freeMem{0};

    for (BlockInfo* b = _instance->_header; b < (BlockInfo*)((char*)_instance->_header + _instance->_memory); b = (BlockInfo*)((char*)(b + 2) + b->size))
    {
        if (!b->flag)
            freeMem += b->size;

        cout << "Block @ " << b << " | size: " << b->size << " | flag: " << (b->flag) << endl;
    }

    cout << "\nTotal memory: " << _instance->_memory << " bytes\n";
    cout << "Available memory: " << freeMem << " bytes\n";
}

inline void
Allocator::free(void* ptr)
{
    auto currentBlock = (BlockInfo*)ptr;
    auto prevBlockFooter = currentBlock - 1;
    auto currentBlockFooter = (BlockInfo*)((char*)(currentBlock + 1) + currentBlock->size);
    auto nextBlockHeader = currentBlockFooter + 1;

    // if (!currentBlock->flag) return;

    if (prevBlockFooter < _instance->_header || nextBlockHeader > _instance->_header + _instance->_memory)
    {
        return;
    }

    // left flag 1 right flag 1
    if (prevBlockFooter->flag && nextBlockHeader->flag)
    {
        // reset
        currentBlockFooter->flag = 0;
        currentBlockFooter->prev = currentBlock;

        // add to free blocks list
        currentBlock->flag = 0;
        currentBlock->next = _instance->_header->next;
        currentBlock->prev = _instance->_header;
        _instance->_header->next = currentBlock;
    }
    // left flag 0 right flag 0
    else if (!prevBlockFooter->flag && !nextBlockHeader->flag)
    {
        auto header = prevBlockFooter->prev;
        auto footer = (BlockInfo*)((char*)(nextBlockHeader + 1) + nextBlockHeader->size);
        footer->prev = header;
        footer->flag = 0;
        header->size = header->size + currentBlock->size + nextBlockHeader->size + 4 * sizeof(BlockInfo);

        header->flag = 0;
        header->next = _instance->_header->next;
        header->prev = _instance->_header;
        _instance->_header->next = header;
    }
    // left flag 0
    else if (!prevBlockFooter->flag)
    {
        currentBlockFooter->flag = 0;
        auto header = prevBlockFooter->prev;
        currentBlockFooter->prev = header;
        header->size = header->size + currentBlock->size + 2 * sizeof(BlockInfo);
        
        header->next = _instance->_header->next;
        header->prev = _instance->_header;
        _instance->_header->next = header;
    }
    // right flag 0
    else if (!nextBlockHeader->flag)
    {
        currentBlock->flag = 0;
        auto footer = (BlockInfo*)((char*)(nextBlockHeader + 1) + nextBlockHeader->size);
        footer->prev = currentBlock;
        currentBlock->size = currentBlock->size + nextBlockHeader->size + 2 * sizeof(BlockInfo);

        currentBlock->next = _instance->_header->next;
        currentBlock->prev = _instance->_header;
        _instance->_header->next = currentBlock;
    }
}

inline void
Allocator::exit()
{
    delete[] _instance->_header;
    delete   _instance;
}

} // begin namespace tcii::ex

#endif // __GC_h
