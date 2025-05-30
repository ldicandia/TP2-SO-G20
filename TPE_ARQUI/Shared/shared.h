#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <stdint.h>

typedef struct MemoryInfo {
    uint64_t totalMemory;
	uint64_t usedMemory;
    uint64_t freeMemory;
    uint64_t totalBlocks;
    uint64_t usedBlocks; 
    uint64_t freeBlocks;      //(todo esto es si aplica, si no poner 0)
} MemoryInfo;

// Devuelve una copia de la info de memoria actual (el caller debe liberar si es heap, o devolver estática)
void getMemoryInfo(MemoryInfo *info); 

#endif