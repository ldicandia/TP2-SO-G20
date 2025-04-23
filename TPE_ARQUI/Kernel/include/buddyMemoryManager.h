#ifndef BUDDY_MEMORY_MANAGER_H
#define BUDDY_MEMORY_MANAGER_H

#include <stdint.h>

void initBuddyMemoryManager(void *memoryStart, uint64_t memorySize);
void *buddyMalloc(uint64_t size);
void buddyFree(void *ptr);

#endif