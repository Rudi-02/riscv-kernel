//
// Memory Allocator implementation
//

#include "../h/MemoryAllocator.hpp"

MemoryAllocator::dataBlock* MemoryAllocator::free = nullptr;
MemoryAllocator::dataBlock* MemoryAllocator::used = nullptr;
uint32 MemoryAllocator::init = 0;


void* MemoryAllocator::mem_alloc(size_t size)
{
	size = size * MEM_BLOCK_SIZE + sizeof(dataBlock);

	if (size <= 0) return nullptr;

	if (init++ == 0) {
		MemoryAllocator::free = (dataBlock*)((char*)HEAP_START_ADDR);

		used = nullptr;
		free->next = nullptr;
        free->prev  = nullptr;
		free->size = ((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - sizeof(dataBlock));
	}

	size_t newSize = size + sizeof(dataBlock);
	if (newSize % MEM_BLOCK_SIZE == 0) size = newSize;
	else {
		uint64 numberOfBlocks = (newSize/MEM_BLOCK_SIZE);
		size = (numberOfBlocks +1) * MEM_BLOCK_SIZE;
	}

	for(dataBlock* current = MemoryAllocator::free; current; current = current->next) {
		if (current->size < size) continue;
		else if(current->size > size) {
			dataBlock* nextFreeBlock = (dataBlock*)((char*)current + sizeof(dataBlock)+ newSize);

			if(current->prev) current->prev->next = nextFreeBlock;
			else MemoryAllocator::free = nextFreeBlock;
			if(current->next) current->next->prev = nextFreeBlock;
			nextFreeBlock->prev = current->prev;
			nextFreeBlock->next = current->next;

			nextFreeBlock->size = current->size - newSize - sizeof(dataBlock);
			current->size = newSize;

            dataBlock* iter = MemoryAllocator::used;
			dataBlock* previous = nullptr;
			for (; iter && iter < current; previous = iter, iter = iter->next);

			if (previous) previous->next = current;
			else used = current;
			current->prev = previous;
			if (iter) iter->prev = current;
			current->next = iter;
		}
		else {
			if (current->prev) current->prev->next = current->next;
			else MemoryAllocator::free = current->next;
			if(current->next) current->next->prev = current->prev;

			dataBlock* iter = MemoryAllocator::used;
			dataBlock* previous = nullptr;
			for (; iter && iter < current; previous = iter, iter = iter->next);

			if (previous) previous->next = current;
			else used = current;
			current->prev = previous;
			if (iter) iter->prev = current;
			current->next = iter;

		}
		return (char*)current + sizeof(dataBlock);
	}

	return nullptr;
}

uint32 MemoryAllocator::mem_free(void* ptr){
	if (!init || used==nullptr) return -1;
	if(ptr == nullptr || ptr < HEAP_START_ADDR || ptr > HEAP_END_ADDR) return -2;

	dataBlock* current = (dataBlock*)((char*)ptr - sizeof(dataBlock));
	if(current < used) return -3;

	if (used == current) {
		used = current->next;
		if(used) used->prev = nullptr;
		current->next = nullptr;
		current->prev = nullptr;
	}
	else {
		current->prev->next = current->next;
		if(current->next) current->next->prev = current->prev;
		current->next = nullptr;
		current->prev = nullptr;
	}

	dataBlock* iter = MemoryAllocator::free;
	dataBlock* previous = nullptr;
	for (; iter && iter < current; previous = iter, iter = iter->next);

	if (previous) previous->next = current;
	else free = current;
	current->prev = previous;
	if (iter) iter->prev = current;
	current->next = iter;
	connectBlocks(current);
	if(current->prev) connectBlocks(current->prev);

	return 0;
}

void MemoryAllocator::connectBlocks(dataBlock* current){
	if(current->next && (char*)current + sizeof(dataBlock) + current->size == (char*)current->next) {
		current->size += current->next->size + sizeof(dataBlock);
		current->next = current->next->next;
		if(current->next) current->next->prev = current;
	}
}

