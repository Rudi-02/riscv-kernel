//
// Memory Allocator header
//

#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"


class MemoryAllocator
{
public:
    static void* mem_alloc(size_t size);
    static uint32 mem_free(void* ptr);

private:
    struct dataBlock
    {
        size_t size;
        dataBlock *next;
        dataBlock *prev;
    };

    static dataBlock* free;
    static dataBlock* used;
    static uint32 init;
    static void connectBlocks(dataBlock* current);
};


#endif //PROJECT_BASE_MEMORYALLOCATOR_HPP
