// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\memoryManager.c
#include "memoryManager.h"

#ifdef USE_BUDDY_MEMORY_MANAGER
#include "buddyMemoryManager.h"
#else
#include <lib.h>
#include <stddef.h>
#include <stdint.h>
#define MEMORY_MANAGER_ADDRESS 0x50000

typedef struct MemoryManagerCDT *MemoryManagerADT;

typedef struct Block {
	size_t size;
	struct Block *next;
	int is_free;
} Block;

struct MemoryManagerCDT {
	void *managed_memory;
	Block *block_list;
	size_t total_size;
};

#define MIN_BLOCK_SIZE sizeof(Block)

#define ALIGN(size) (((size) + 7) & ~7)

MemoryManagerADT createMemoryManager(void *const restrict managedMemory,
									 uint64_t memAmount) {
	if (!managedMemory || !memAmount)
		return NULL;

	MemoryManagerADT mm = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
	mm->managed_memory	= managedMemory;
	mm->total_size		= 0;

	Block *initial_block   = (Block *) managedMemory;
	initial_block->size	   = 0;
	initial_block->next	   = NULL;
	initial_block->is_free = 1;

	mm->block_list = initial_block;

	return mm;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager,
				  const size_t memoryToAllocate) {
	if (!memoryManager || memoryToAllocate == 0)
		return NULL;

	size_t alloc_size = ALIGN(memoryToAllocate);
	if (alloc_size < MIN_BLOCK_SIZE)
		alloc_size = MIN_BLOCK_SIZE;

	Block *current = memoryManager->block_list;
	Block *prev	   = NULL;

	while (current) {
		if (current->is_free && current->size >= alloc_size) {
			if (current->size >= alloc_size + sizeof(Block) + MIN_BLOCK_SIZE) {
				Block *new_block =
					(Block *) ((char *) current + sizeof(Block) + alloc_size);
				new_block->size	   = current->size - alloc_size - sizeof(Block);
				new_block->is_free = 1;
				new_block->next	   = current->next;
				current->size	   = alloc_size;
				current->next	   = new_block;
			}
			current->is_free = 0;
			return (char *) current + sizeof(Block);
		}
		prev	= current;
		current = current->next;
	}

	if (!prev)
		return NULL;
	void *end_of_last_block = (char *) prev + sizeof(Block) + prev->size;
	void *new_block_addr	= end_of_last_block;
	Block *new_block		= (Block *) new_block_addr;

	new_block->size	   = alloc_size;
	new_block->is_free = 0;
	new_block->next	   = NULL;
	prev->next		   = new_block;

	return (char *) new_block + sizeof(Block);
}

// Free allocated memory
void freeMemory(void *ptr) {
	if (!ptr)
		return;
	Block *block   = (Block *) ((char *) ptr - sizeof(Block));
	block->is_free = 1;
}
#endif