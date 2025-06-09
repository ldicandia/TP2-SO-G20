// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifdef OUR
#include "memoryManager.h"

#include <defs.h>
#include <stdint.h>
#include <stdbool.h>
#include <videoDriver.h>
#include <shared.h>

#define BLOCK_SIZE 32
#define HEADER_SIZE sizeof(BlockHeader)

typedef struct BlockHeader {
	uint64_t size;
	bool is_free;
	struct BlockHeader *next;
	struct BlockHeader *prev;
} BlockHeader;

typedef struct MemoryManagerCDT {
	BlockHeader *first_block;
	char *heap_start;
	char *heap_end;
	uint64_t total_size;
} MemoryManagerCDT;

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
					void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;

	memoryManager->heap_start = (char *) managedMemory;
	memoryManager->heap_end	  = (char *) managedMemory + memAmount;
	memoryManager->total_size = memAmount;

	memoryManager->first_block			= (BlockHeader *) managedMemory;
	memoryManager->first_block->size	= memAmount - HEADER_SIZE;
	memoryManager->first_block->is_free = true;
	memoryManager->first_block->next	= NULL;
	memoryManager->first_block->prev	= NULL;

	return memoryManager;
}

static void split_block(BlockHeader *block, uint64_t size) {
	if (block->size > size + HEADER_SIZE + BLOCK_SIZE) {
		BlockHeader *new_block =
			(BlockHeader *) ((char *) block + HEADER_SIZE + size);
		new_block->size	   = block->size - size - HEADER_SIZE;
		new_block->is_free = true;
		new_block->next	   = block->next;
		new_block->prev	   = block;

		if (block->next) {
			block->next->prev = new_block;
		}
		block->next = new_block;
		block->size = size;
	}
}

static void coalesce_blocks(BlockHeader *block) {
	while (block->next && block->next->is_free) {
		block->size += block->next->size + HEADER_SIZE;
		BlockHeader *next = block->next;
		block->next		  = next->next;
		if (next->next) {
			next->next->prev = block;
		}
	}

	if (block->prev && block->prev->is_free) {
		block->prev->size += block->size + HEADER_SIZE;
		block->prev->next = block->next;
		if (block->next) {
			block->next->prev = block->prev;
		}
	}
}

void *allocMemory(const uint64_t memoryToAllocate) {
	MemoryManagerADT memoryManager = getMemoryManager();

	if (memoryToAllocate == 0 || memoryManager == NULL) {
		return NULL;
	}

	uint64_t aligned_size =
		(memoryToAllocate + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);

	BlockHeader *current = memoryManager->first_block;
	while (current != NULL) {
		if (current->is_free && current->size >= aligned_size) {
			current->is_free = false;
			split_block(current, aligned_size);
			return (char *) current + HEADER_SIZE;
		}
		current = current->next;
	}

	return NULL;
}

MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

void freeMemory(void *ptr) {
	if (ptr == NULL) {
		return;
	}

	BlockHeader *block = (BlockHeader *) ((char *) ptr - HEADER_SIZE);
	block->is_free	   = true;

	coalesce_blocks(block);
}

void getMemoryInfo(MemoryInfo *info) {
	MemoryManagerADT mm = getMemoryManager();
	if (!mm || !info)
		return;

	uint64_t used		 = 0;
	uint64_t free_blocks = 0;
	uint64_t used_blocks = 0;

	BlockHeader *current = mm->first_block;
	while (current != NULL) {
		if (current->is_free) {
			free_blocks++;
		}
		else {
			used += current->size;
			used_blocks++;
		}
		current = current->next;
	}

	info->totalMemory = mm->total_size;
	info->usedMemory  = used;
	info->freeMemory  = mm->total_size - used;
	info->usedBlocks  = used_blocks;
	info->freeBlocks  = free_blocks;
	info->totalBlocks = used_blocks + free_blocks;
}
#endif