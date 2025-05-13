// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\include\memoryManager.h
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stdlib.h>

#define MEMORY_MANAGER_ADDRESS 0x50000
#define SYSTEM_VARIABLES 0x5A00

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT

createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory,
					uint64_t memAmount);

void *allocMemory(const size_t memoryToAllocate);

MemoryManagerADT getMemoryManager(void);

void freeMemory(void *ptr);

#endif
