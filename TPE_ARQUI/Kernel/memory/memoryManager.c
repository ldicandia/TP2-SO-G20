// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\memoryManager.c

#include "memoryManager.h"

#ifdef USE_BUDDY_MEMORY_MANAGER

#include "buddyMemoryManager.h"

#else

#include <defs.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BLOCKS 1024 // Máxima cantidad de bloques a trackear

// Estructura de tracking de memoria asignada
typedef struct MemoryBlock {
	void *address;
	uint64_t size;
} MemoryBlock;

// Estructura del memory manager
typedef struct MemoryManagerCDT {
	char *nextAddress;	 // Siguiente dirección disponible para asignación
	char *endAddress;	 // Fin del área de memoria gestionada
	MemoryBlock *blocks; // Array de bloques asignados
	uint16_t blockCount; // Cantidad actual de bloques usados
} MemoryManagerCDT;

MemoryManagerADT
createMemoryManager(void *const restrict memoryForMemoryManager,
					void *const restrict managedMemory, uint64_t memAmount) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;

	// Reservamos al inicio un espacio para los bloques
	memoryManager->blocks = (MemoryBlock *) managedMemory;

	memoryManager->blockCount = 0;

	// Espacio restante para asignación real al usuario
	char *startOfUserMemory =
		(char *) managedMemory + sizeof(MemoryBlock) * MAX_BLOCKS;

	memoryManager->nextAddress = startOfUserMemory;
	memoryManager->endAddress  = (char *) managedMemory + memAmount;

	return memoryManager;
}

// Asumimos que esta dirección está seteada correctamente en la parte del kernel
MemoryManagerADT getMemoryManager() {
	return (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
}

// Agrega un bloque al array de tracking
static void addBlock(MemoryManagerADT memoryManager, void *address,
					 uint64_t size) {
	if (memoryManager->blockCount >= MAX_BLOCKS)
		return;

	memoryManager->blocks[memoryManager->blockCount].address = address;
	memoryManager->blocks[memoryManager->blockCount].size	 = size;
	memoryManager->blockCount++;
}

// Elimina un bloque del tracking si coincide con la dirección dada
static bool removeBlock(MemoryManagerADT memoryManager, void *address) {
	if (memoryManager == NULL || address == NULL)
		return false;

	for (int i = 0; i < memoryManager->blockCount; i++) {
		if (memoryManager->blocks[i].address == address) {
			// Reemplazamos el bloque eliminado con el último bloque
			memoryManager->blocks[i] =
				memoryManager->blocks[memoryManager->blockCount - 1];
			memoryManager->blockCount--;
			return true;
		}
	}

	return false; // No encontrado
}

// Asigna memoria y guarda el bloque
void *allocMemory(const uint64_t memoryToAllocate) {
	MemoryManagerADT memoryManager = getMemoryManager();

	if (memoryToAllocate == 0 || memoryManager == NULL)
		return NULL;

	if (memoryManager->nextAddress + memoryToAllocate >
		memoryManager->endAddress) {
		return NULL; // No hay suficiente espacio
	}

	void *allocation = memoryManager->nextAddress;
	memoryManager->nextAddress += memoryToAllocate;

	addBlock(memoryManager, allocation, memoryToAllocate);

	return allocation;
}

// Libera un bloque asignado
void freeMemory(void *ptr) {
	if (ptr == NULL)
		return;

	MemoryManagerADT memoryManager = getMemoryManager();

	removeBlock(memoryManager, ptr);
}

#endif
