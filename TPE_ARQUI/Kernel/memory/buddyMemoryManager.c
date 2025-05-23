// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifdef BUDDY
#include "memoryManager.h"

#include <lib.h>
#include <stdint.h>
#include <stdlib.h>

#define LEVELS 10
#define FREE 0
#define USED 1
#define MIN_BUDDY_EXP 5 // Tamaño del MemoryBlock

/*División: Los bloques grandes se dividen en pares de bloques iguales
(buddies). Asignación: Se asigna el bloque del tamaño adecuado (o uno de los
buddies si es q se dividió). Fusión: Cuando dos bloques buddy contiguos están
libres, se fusionan para formar un bloque más grande. */

typedef struct BuddyBlock {
	uint8_t exp;  // Exponente de 2 que representa el tamaño del bloque
	uint8_t used; // 0 si libre, 1 si usado
	struct BuddyBlock *prev; // Puntero al bloque anterior en la lista libre
	struct BuddyBlock *next; // Puntero al bloque siguiente en la lista libre
} BuddyBlock;

typedef struct MemoryManagerCDT {
	uint8_t maxExp;
	void *Address;				// dir inicial de la memoria
	BuddyBlock *blocks[LEVELS]; // Array de listas doblemente enlazadas de
								// bloques libres (tantos niveles)
	uint64_t totalMemory;
	uint64_t freeMemory;
	uint32_t totalBlocks;
	uint32_t freeBlocks;

} MemoryManagerCDT;

static uint8_t next_exp(uint64_t n) { // encuentra el menor exponenta tal que
									  // 2^exp sea mayor o igual a n
	uint8_t exp = 0;
	while ((1UL << exp) < n) { // 1UL
		exp++;
	}
	return exp;
}

static BuddyBlock *createBuddyBlock(void *address, uint8_t exp,
									BuddyBlock *next);
static BuddyBlock *removeBuddyBlock(MemoryManagerADT memoryManager,
									BuddyBlock *block);
static BuddyBlock *merge(MemoryManagerADT memoryManager, BuddyBlock *block,
						 BuddyBlock *buddy);
static void split(MemoryManagerADT memoryManager, uint8_t index);
static void insertFreeBlock(uint8_t index, BuddyBlock *block,
							MemoryManagerADT memoryManager);

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
					void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;

	memoryManager->Address	   = managedMemory;
	memoryManager->totalMemory = 1UL << next_exp(memAmount);
	memoryManager->freeMemory  = memoryManager->totalMemory;
	memoryManager->totalBlocks = 1;
	memoryManager->freeBlocks  = 1;
	memoryManager->maxExp	   = next_exp(memoryManager->totalMemory);

	for (int i = 0; i < LEVELS; ++i) {
		memoryManager->blocks[i] = NULL;
	}

	if (memoryManager->maxExp < MIN_BUDDY_EXP) {
		return NULL; // La memoria es demasiado chica
	}
	BuddyBlock *initialBlock = createBuddyBlock(
		managedMemory, memoryManager->maxExp, NULL); // no tiene previo
	insertFreeBlock(memoryManager->maxExp - MIN_BUDDY_EXP, initialBlock,
					memoryManager); // Insertar en la lista correcta

	return memoryManager;
}

MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

void *allocMemory(const size_t size) {
	MemoryManagerADT memoryManager = getMemoryManager();
	if (size == 0 || memoryManager == NULL) {
		return NULL;
	}

	// Sumar el tamaño del header para reservar espacio para el BuddyBlock
	uint8_t exp = next_exp(size + sizeof(BuddyBlock));
	if (exp < MIN_BUDDY_EXP)
		exp = MIN_BUDDY_EXP;

	if (exp > memoryManager->maxExp)
		return NULL; // No hay suficiente memoria

	int idxToAlloc = exp - MIN_BUDDY_EXP;

	// Si no hay bloques libres en el nivel, buscar el más cercano superior y
	// dividir
	int closestIdx = -1;
	for (int i = idxToAlloc; i < LEVELS; ++i) {
		if (memoryManager->blocks[i] != NULL) {
			closestIdx = i;
			break;
		}
	}
	if (closestIdx == -1)
		return NULL; // No hay bloques libres disponibles

	// Dividir bloques grandes hasta llegar al tamaño deseado
	for (int i = closestIdx; i > idxToAlloc; --i) {
		split(memoryManager, i + MIN_BUDDY_EXP);
	}

	// Tomar el primer bloque libre del nivel adecuado
	BuddyBlock *block =
		removeBuddyBlock(memoryManager, memoryManager->blocks[idxToAlloc]);
	block->used = USED;
	block->prev = NULL;
	block->next = NULL;

	uint64_t blockSize = 1UL << block->exp;
	memoryManager->freeMemory -= blockSize;
	memoryManager->freeBlocks--;
	memoryManager->totalBlocks--;

	// Retornar la dirección usable (después del header)
	return (void *) ((uint8_t *) block + sizeof(BuddyBlock));
}

void freeMemory(void *ptr) {
	if (ptr == NULL)
		return;

	MemoryManagerADT memoryManager = getMemoryManager();
	// El bloque está justo antes del puntero devuelto al usuario
	BuddyBlock *block = (BuddyBlock *) ((uint8_t *) ptr - sizeof(BuddyBlock));
	if (block->used == FREE)
		return;
	block->used = FREE;

	uint64_t blockSize = 1UL << block->exp;
	memoryManager->freeMemory += blockSize;
	memoryManager->totalBlocks++;
	memoryManager->freeBlocks++;

	// Calcular la posición relativa del bloque respecto al inicio de la memoria
	// administrada
	uint64_t relativePosition =
		(uint64_t) ((uint8_t *) block - (uint8_t *) memoryManager->Address);

	// Buscar y fusionar con buddies libres del mismo tamaño
	BuddyBlock *buddy = NULL;
	while (block->exp < memoryManager->maxExp) {
		uint64_t buddyOffset = relativePosition ^ (1UL << block->exp);
		buddy =
			(BuddyBlock *) ((uint8_t *) memoryManager->Address + buddyOffset);

		if (buddy->used == FREE && buddy->exp == block->exp) {
			// Remover buddy de la lista de libres
			removeBuddyBlock(memoryManager, buddy);
			// Fusionar ambos bloques
			if (block > buddy)
				block = buddy;
			block->exp++;
			memoryManager->freeBlocks--;
			memoryManager->totalBlocks--;
			// Actualizar posición relativa para el siguiente ciclo
			relativePosition = (uint64_t) ((uint8_t *) block -
										   (uint8_t *) memoryManager->Address);
		}
		else {
			break;
		}
	}

	// Insertar el bloque (fusionado o no) en la lista de libres correspondiente
	insertFreeBlock(block->exp - MIN_BUDDY_EXP, block, memoryManager);
}

// add, remove, merge y split  de los buddy blocks...

static BuddyBlock *createBuddyBlock(void *address, uint8_t exp,
									BuddyBlock *next) {
	BuddyBlock *block = (BuddyBlock *) address;
	block->exp		  = exp;
	block->used		  = FREE;
	block->prev		  = NULL;
	if (next != NULL) {
		next->prev = block;
	}
	return block;
}

static BuddyBlock *removeBuddyBlock(MemoryManagerADT memoryManager,
									BuddyBlock *block) {
	if (block->prev != NULL) {
		block->prev->next = block->next;
	}
	else {
		memoryManager->blocks[block->exp - MIN_BUDDY_EXP] = block->next;
	}

	if (block->next != NULL) {
		block->next->prev = block->prev;
	}

	return block->next;
}

static BuddyBlock *merge(MemoryManagerADT memoryManager, BuddyBlock *block,
						 BuddyBlock *buddy) {
	removeBuddyBlock(memoryManager,
					 buddy); // el buddy se elimina, xq se fusionan en uno

	BuddyBlock *leftBlock =
		(BuddyBlock *) ((uintptr_t) block < (uintptr_t) buddy ? block : buddy);
	leftBlock->exp++;

	memoryManager->freeBlocks--;
	memoryManager->totalBlocks--;
	// memoryManager->freeMemory += (1UL << (leftBlock->exp - MIN_BUDDY_EXP));
	uint64_t mergedBlockSize = 1UL << block->exp;
	memoryManager->freeMemory += mergedBlockSize;
	return leftBlock;
}

static void split(MemoryManagerADT memoryManager, uint8_t index) {
	BuddyBlock *block = memoryManager->blocks[index - MIN_BUDDY_EXP];
	removeBuddyBlock(memoryManager, block);

	uint64_t blockSize =
		1UL << (index + MIN_BUDDY_EXP); // Tamaño del bloque a dividir
	uint64_t halfBlockSize = blockSize >> 1;
	uint8_t newExp		   = index + MIN_BUDDY_EXP - 1;

	BuddyBlock *buddyBlock = (BuddyBlock *) ((char *) block + halfBlockSize);
	buddyBlock->exp		   = newExp;
	buddyBlock->used	   = FREE;
	buddyBlock->prev	   = NULL;
	buddyBlock->next	   = memoryManager->blocks[newExp - MIN_BUDDY_EXP];
	if (memoryManager->blocks[newExp - MIN_BUDDY_EXP] != NULL) {
		memoryManager->blocks[newExp - MIN_BUDDY_EXP]->prev = buddyBlock;
	}
	memoryManager->blocks[newExp - MIN_BUDDY_EXP] = buddyBlock;

	block->exp									  = newExp;
	block->used									  = FREE;
	block->prev									  = NULL;
	block->next									  = buddyBlock;
	buddyBlock->prev							  = block;
	memoryManager->blocks[newExp - MIN_BUDDY_EXP] = block;

	memoryManager->freeBlocks++;
	memoryManager->totalBlocks++;
}

static void insertFreeBlock(uint8_t index, BuddyBlock *block,
							MemoryManagerADT memoryManager) {
	block->next = memoryManager->blocks[index];
	block->prev = NULL;

	if (memoryManager->blocks[index] != NULL) {
		memoryManager->blocks[index]->prev = block;
	}

	memoryManager->blocks[index] = block;
	memoryManager->freeBlocks++;
}

#endif