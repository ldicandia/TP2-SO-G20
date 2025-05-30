// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\include\memoryManager.h
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stdlib.h>
#include <videoDriver.h>

#define MEMORY_MANAGER_ADDRESS 0x50000
#define SYSTEM_VARIABLES 0x5A00

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT

createMemoryManager(void *const restrict memoryForMemoryManager,
					void *const restrict managedMemory, uint64_t memAmount);

void *allocMemory(const size_t memoryToAllocate);

MemoryManagerADT getMemoryManager(void);

void freeMemory(void *ptr);

//para mem (info)

typedef struct MemoryInfo {
    uint64_t totalMemory;
	uint64_t usedMemory;
    uint64_t freeMemory;
    uint64_t totalBlocks;
    uint64_t usedBlocks; 
    uint64_t freeBlocks;      //(todo esto es si aplica, si no poner 0)
} MemoryInfo;

// Devuelve una copia de la info de memoria actual (el caller debe liberar si es heap, o devolver estática)
MemoryInfo * getMemoryInfo(); 
void printMemoryInfo(const MemoryInfo *info);
#endif
