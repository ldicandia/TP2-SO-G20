#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <stdint.h>

typedef struct MemoryInfo {
	uint64_t totalMemory;
	uint64_t usedMemory;
	uint64_t freeMemory;
	uint64_t totalBlocks;
	uint64_t usedBlocks;
	uint64_t freeBlocks; //(todo esto es si aplica, si no poner 0)
} MemoryInfo;

typedef struct ProcessInfo {
	uint16_t pid;
	char *name;
	uint8_t priority;
	uint8_t status;		 // 0: READY, 1: BLOCKED, 2: RUNNING, 3: ZOMBIE
	uint64_t memoryUsed; // Memory used by the process
	// Nuevos campos:
	uint64_t stackPointer;
	uint64_t basePointer;
	uint8_t foreground; // 0: background, 1: foreground
} ProcessInfo;

typedef struct ProcessInfoList {
	uint16_t length;
	ProcessInfo *snapshotList;
} ProcessInfoList;

// Devuelve una copia de la info de memoria actual (el caller debe liberar si es
// heap, o devolver estática)
void getMemoryInfo(MemoryInfo *info);

void getProcessInfo(ProcessInfo *info);

#endif