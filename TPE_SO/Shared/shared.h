#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <stdint.h>

typedef struct MemoryInfo {
	uint64_t totalMemory;
	uint64_t usedMemory;
	uint64_t freeMemory;
	uint64_t totalBlocks;
	uint64_t usedBlocks;
	uint64_t freeBlocks;
} MemoryInfo;

typedef struct ProcessInfo {
	uint16_t pid;
	char *name;
	uint8_t priority;
	uint8_t status;
	uint64_t memoryUsed;
	uint64_t stackPointer;
	uint64_t basePointer;
	uint8_t foreground;
} ProcessInfo;

typedef struct ProcessInfoList {
	uint16_t length;
	ProcessInfo *snapshotList;
} ProcessInfoList;

void getMemoryInfo(MemoryInfo *info);

void getProcessInfo(ProcessInfo *info);

#endif