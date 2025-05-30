// This is a personal academic project. Dear PVS-Studio, please check it.

#ifdef BUDDY

#include "memoryManager.h"
#include <stdint.h>
#include <stddef.h>
#include <shared.h>

#define MIN_EXP 5 // Minimum block size is 2^5 = 32 bytes
#define LEVELS 32 //cant de tamanos distintos de bloques, desde 2^5 hasta 2^36
#define FREE 0
#define USED 1

typedef struct MemoryBlock {
    uint8_t exp;
    uint8_t used;
    struct MemoryBlock *prev;
    struct MemoryBlock *next;
} MemoryBlock;

typedef struct MemoryManagerCDT {
    void *firstAddress;
    uint8_t maxExp;
    MemoryBlock *blocks[LEVELS];
} MemoryManagerCDT;

static MemoryBlock *createMemoryBlock(void *ptr, uint8_t exp, MemoryBlock *next) {
    MemoryBlock *block = (MemoryBlock *)ptr;
    block->exp = exp;
    block->used = FREE;
    block->prev = NULL;
    block->next = next;
    if (next) next->prev = block;
    return block;
}

static MemoryBlock *removeMemoryBlock(MemoryBlock **blocks, MemoryBlock *block) {//remueve de la lista de libres
    if (block->prev)
        block->prev->next = block->next;
    else
        blocks[block->exp - MIN_EXP] = block->next;
    if (block->next)
        block->next->prev = block->prev;
    return block->next;
}

static void split(MemoryManagerADT mm, uint8_t idx) {
    MemoryBlock *block = mm->blocks[idx];
    removeMemoryBlock(mm->blocks, block);

    uint8_t newExp = block->exp - 1;
    uint64_t halfSize = 1UL << newExp;

    MemoryBlock *buddy = (MemoryBlock *)((uint8_t *)block + halfSize);
    createMemoryBlock(buddy, newExp, mm->blocks[newExp - MIN_EXP]);
    mm->blocks[newExp - MIN_EXP] = createMemoryBlock(block, newExp, buddy);
}

static MemoryBlock *merge(MemoryManagerADT mm, MemoryBlock *block, MemoryBlock *buddy) {//une dos buddies libres 
    removeMemoryBlock(mm->blocks, buddy);
    MemoryBlock *left = (block < buddy) ? block : buddy;
    left->exp++;
    return left;
}

MemoryManagerADT createMemoryManager(void *const restrict memoryForManager,
                                     void *const restrict managedMemory,
                                     uint64_t memAmount) {
    (void)memoryForManager; //no lo uso
    MemoryManagerADT mm = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
    mm->firstAddress = managedMemory;
    mm->maxExp = 0;
    uint64_t size = 1UL << MIN_EXP;
    while (size < memAmount && mm->maxExp < LEVELS + MIN_EXP)
        size <<= 1, mm->maxExp++;
    mm->maxExp += MIN_EXP;

    for (int i = 0; i < LEVELS; i++)
        mm->blocks[i] = NULL;

    uint8_t idx = mm->maxExp - MIN_EXP;
    mm->blocks[idx] = createMemoryBlock(managedMemory, mm->maxExp, NULL);
    return mm;
}

MemoryManagerADT getMemoryManager(void) {
    return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

void *allocMemory(const size_t size) {
    MemoryManagerADT mm = getMemoryManager();
    if (!mm) return NULL;

    uint8_t exp = MIN_EXP;
    uint64_t blockSize = 1UL << exp;
    while (blockSize < size + sizeof(MemoryBlock) && exp < mm->maxExp)
        blockSize <<= 1, exp++;

    if (exp > mm->maxExp)
        return NULL;

    uint8_t idx = exp - MIN_EXP;
    uint8_t splitIdx = idx;
    while (splitIdx < LEVELS && mm->blocks[splitIdx] == NULL) 
        splitIdx++;
    if (splitIdx == LEVELS)
        return NULL;

    while (splitIdx > idx) {//divide en buddies hasta alcanzar un tamaño adecuado
        split(mm, splitIdx);//recursiva
        splitIdx--;
    }

    MemoryBlock *block = mm->blocks[idx];
    removeMemoryBlock(mm->blocks, block);
    block->used = USED;
    block->prev = NULL;
    block->next = NULL;

    return (void *)((uint8_t *)block + sizeof(MemoryBlock));
}

void freeMemory(void *ptr) {
    if (!ptr) return;
    MemoryManagerADT mm = getMemoryManager();
    MemoryBlock *block = (MemoryBlock *)((uint8_t *)ptr - sizeof(MemoryBlock));
    if (block->used == FREE)
        return;
    block->used = FREE;

    uint64_t relPos = (uint64_t)((uint8_t *)block - (uint8_t *)mm->firstAddress);
    MemoryBlock *buddy = (MemoryBlock *)((uint8_t *)mm->firstAddress + (relPos ^ (1UL << block->exp)));
    while (buddy->used == FREE && buddy->exp == block->exp && block->exp < mm->maxExp) {
        block = merge(mm, block, buddy);
        relPos = (uint64_t)((uint8_t *)block - (uint8_t *)mm->firstAddress);
        buddy = (MemoryBlock *)((uint8_t *)mm->firstAddress + (relPos ^ (1UL << block->exp)));
    }
    mm->blocks[block->exp - MIN_EXP] = createMemoryBlock((void *)block, block->exp, mm->blocks[block->exp - MIN_EXP]);
}

void getMemoryInfo(MemoryInfo *info) { 
    MemoryManagerADT mm = getMemoryManager();
   // static MemoryInfo *info;
    //if (!mm || !info) return null;

    uint8_t maxExp = mm->maxExp;
    uint64_t total = 1UL << maxExp;
    uint64_t used = 0, free = 0;
    uint64_t usedBlocks = 0, freeBlocks = 0, totalBlocks = 0;

    uint8_t *ptr = (uint8_t *)mm->firstAddress;
    uint8_t *end = ptr + total;

    while (ptr < end) {
        MemoryBlock *block = (MemoryBlock *)ptr;
        uint64_t blockSize = 1UL << block->exp;
        totalBlocks++;
        if (block->used == USED) {
            usedBlocks++;
            used += blockSize;
        } else {
            freeBlocks++;
            free += blockSize;
        }
        ptr += blockSize;
    }

    info->totalMemory = total;
    info->usedMemory = used;
    info->freeMemory = free;
    info->usedBlocks = usedBlocks;
    info->freeBlocks = freeBlocks;
    info->totalBlocks = totalBlocks;
   // return &info;
   return;
}



#endif