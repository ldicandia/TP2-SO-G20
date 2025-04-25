// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\include\memoryManager.h
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stdlib.h>

#ifdef USE_BUDDY_MEMORY_MANAGER
#include "buddyMemoryManager.h"
#define initMemoryManager initBuddyMemoryManager
#define malloc buddyMalloc
#define free buddyFree
#else

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict managedMemory,
									 uint64_t memAmount);
void *allocMemory(MemoryManagerADT const restrict memoryManager,
				  const size_t memoryToAllocate);
void freeMemory(void *ptr);
#endif

#endif
