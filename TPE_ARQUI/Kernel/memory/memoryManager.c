// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\memoryManager.c
#include "memoryManager.h"

#ifdef USE_BUDDY_MEMORY_MANAGER
#include "buddyMemoryManager.h"
#else
#include <defs.h>
#include <memoryManager.h>
#include <stdint.h>

typedef struct MemoryManagerCDT {
	char *nextAddress;
} MemoryManagerCDT;

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
					void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress	   = managedMemory;

	return memoryManager;
}

MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

void *allocMemory(const uint64_t memoryToAllocate) {
	MemoryManagerADT memoryManager = getMemoryManager();
	char *allocation			   = memoryManager->nextAddress;

	memoryManager->nextAddress += memoryToAllocate;

	return (void *) allocation;
}

#endif