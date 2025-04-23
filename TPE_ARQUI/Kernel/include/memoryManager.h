// filepath: c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\include\memoryManager.h
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>

void initMemoryManager(void *memoryStart, uint64_t memorySize);
void *malloc(uint64_t size);
void free(void *ptr);

#endif