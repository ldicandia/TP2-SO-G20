// filepath: c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\memoryManager.c
#include "memoryManager.h"
#include <lib.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct Block {
    uint64_t size;
    struct Block *next;
    uint8_t free;
} Block;

static Block *freeList = NULL;

void initMemoryManager(void *memoryStart, uint64_t memorySize) {
    freeList = (Block *)memoryStart;
    freeList->size = memorySize - sizeof(Block);
    freeList->next = NULL;
    freeList->free = 1;
}

void *malloc(uint64_t size) {
    Block *current = freeList;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            current->free = 0;
            return (void *)(current + 1);
        }
        current = current->next;
    }
    return NULL; // No hay suficiente memoria
}

void free(void *ptr) {
    if (ptr == NULL) return;
    Block *block = (Block *)ptr - 1;
    block->free = 1;
}