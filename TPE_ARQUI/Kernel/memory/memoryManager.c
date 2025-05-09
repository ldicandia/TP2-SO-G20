// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\memoryManager.c

#ifndef BUDDY //si no se pide el buddy

#include "memoryManager.h"


#include <defs.h>
#include <memoryManager.h>
#include <stdint.h>
#include <stdbool.h>

// Estructura para rastrear bloques de memoria asignados
typedef struct MemoryBlock {
	void *address;
	uint64_t size;
	struct MemoryBlock *next;
} MemoryBlock;

typedef struct MemoryManagerCDT {
	char *nextAddress;
	char *endAddress;
	MemoryBlock *allocatedBlocks; // Lista de bloques asignados
} MemoryManagerCDT;

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
					void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress	   = managedMemory;
	memoryManager->endAddress	   = (char *) managedMemory + memAmount;
	memoryManager->allocatedBlocks = NULL;

	return memoryManager;
}

MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

static void addBlock(MemoryManagerADT memoryManager, void *address,
					 uint64_t size) {
	MemoryBlock *newBlock = (MemoryBlock *) memoryManager->nextAddress;
	memoryManager->nextAddress += sizeof(MemoryBlock);

	newBlock->address			   = address;
	newBlock->size				   = size;
	newBlock->next				   = memoryManager->allocatedBlocks;
	memoryManager->allocatedBlocks = newBlock;
}

static bool removeBlock(MemoryManagerADT memoryManager, void *address) {
	MemoryBlock **current = &memoryManager->allocatedBlocks;

	while (*current != NULL) {
		if ((*current)->address == address) {
			MemoryBlock *toRemove = *current;
			*current			  = toRemove->next;
			return true;
		}
		current = &(*current)->next;
	}
	return false;
}

void *allocMemory(const uint64_t memoryToAllocate) {
	MemoryManagerADT memoryManager = getMemoryManager();

	if (memoryToAllocate == 0) {
		return NULL;
	}

	if (memoryManager->nextAddress + memoryToAllocate >
		memoryManager->endAddress) {
		return NULL;
	}

	char *allocation = memoryManager->nextAddress;
	memoryManager->nextAddress += memoryToAllocate;

	addBlock(memoryManager, allocation, memoryToAllocate);

	return (void *) allocation;
}

void freeMemory(void *ptr) {
	if (ptr == NULL) {
		return;
	}

	MemoryManagerADT memoryManager = getMemoryManager();

	if (!removeBlock(memoryManager, ptr)) {
		return;
	}
}

#endif