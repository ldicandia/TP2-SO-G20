// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\memoryManager.c

// #ifdef USE_BUDDY_MEMORY_MANAGER

// #ifndef BUDDY
#ifdef OUR
#include "memoryManager.h"
// #include "buddyMemoryManager.h"

// #else

#include <defs.h>
#include <stdint.h>
#include <stdbool.h>
#include <videoDriver.h>
#include <shared.h>

#define BLOCK_SIZE 32 // Tamaño mínimo de bloque
#define HEADER_SIZE sizeof(BlockHeader)

typedef struct BlockHeader {
	uint64_t size;			  // Tamaño del bloque (sin header)
	bool is_free;			  // Estado del bloque
	struct BlockHeader *next; // Siguiente bloque
	struct BlockHeader *prev; // Bloque anterior
} BlockHeader;

typedef struct MemoryManagerCDT {
	BlockHeader *first_block; // Primer bloque de la lista
	char *heap_start;		  // Inicio del heap
	char *heap_end;			  // Final del heap
	uint64_t total_size;	  // Tamaño total del heap
} MemoryManagerCDT;

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
					void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;

	memoryManager->heap_start = (char *) managedMemory;
	memoryManager->heap_end	  = (char *) managedMemory + memAmount;
	memoryManager->total_size = memAmount;

	// Crear el primer bloque libre que ocupa todo el heap
	memoryManager->first_block			= (BlockHeader *) managedMemory;
	memoryManager->first_block->size	= memAmount - HEADER_SIZE;
	memoryManager->first_block->is_free = true;
	memoryManager->first_block->next	= NULL;
	memoryManager->first_block->prev	= NULL;

	return memoryManager;
}

// Dividir un bloque si es necesario
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

// Fusionar bloques libres adyacentes
static void coalesce_blocks(BlockHeader *block) {
	// Fusionar con el siguiente bloque si está libre
	while (block->next && block->next->is_free) {
		block->size += block->next->size + HEADER_SIZE;
		BlockHeader *next = block->next;
		block->next		  = next->next;
		if (next->next) {
			next->next->prev = block;
		}
	}

	// Fusionar con el bloque anterior si está libre
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

	// Alinear el tamaño a múltiplos del tamaño de bloque
	uint64_t aligned_size =
		(memoryToAllocate + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);

	// Buscar primer bloque libre que sea suficientemente grande (First Fit)
	BlockHeader *current = memoryManager->first_block;
	while (current != NULL) {
		if (current->is_free && current->size >= aligned_size) {
			current->is_free = false;
			split_block(current, aligned_size);
			return (char *) current + HEADER_SIZE;
		}
		current = current->next;
	}

	return NULL; // No hay memoria suficiente
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

	// Fusionar con bloques adyacentes libres
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
