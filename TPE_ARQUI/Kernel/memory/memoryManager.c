// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// filepath:
// c:\Users\poloa\OneDrive\Documentos\GitHub\g14-64607-63212-62837\TPE_ARQUI\Kernel\memoryManager.c

// #ifdef USE_BUDDY_MEMORY_MANAGER

// #ifndef BUDDY
#ifdef OUR
#include "memoryManager.h"
// #include "buddyMemoryManager.h"

// #else

#include <defs.h>
#include <stdint.h>
#include <stdbool.h>
#include <videoDriver.h>
#include <shared.h>

#define MAX_BLOCKS 1024 // Máxima cantidad de bloques a trackear

#define RED (Color){0x00, 0x00, 0xFF}

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
	if (memoryManager->blockCount >= MAX_BLOCKS) {
		driver_printStr("addBlock error: max blocks reached\n", RED);
		return;
	}
	memoryManager->blocks[memoryManager->blockCount].address = address;
	memoryManager->blocks[memoryManager->blockCount].size	 = size;
	memoryManager->blockCount++;
}

// Elimina un bloque del tracking si coincide con la dirección dada
static bool removeBlock(MemoryManagerADT memoryManager, void *address) {
	if (memoryManager == NULL || address == NULL) {
		driver_printStr("removeBlock error: null argument\n", RED);
		return false;
	}
	for (int i = 0; i < memoryManager->blockCount; i++) {
		if (memoryManager->blocks[i].address == address) {
			// Reemplazamos el bloque eliminado con el último bloque
			memoryManager->blocks[i] =
				memoryManager->blocks[memoryManager->blockCount - 1];
			memoryManager->blockCount--;
			return true;
		}
	}
	// driver_printStr("removeBlock warning: block not found\n", RED);
	return false; // No encontrado
}

// Asigna memoria y guarda el bloque
void *allocMemory(const uint64_t memoryToAllocate) {
	MemoryManagerADT memoryManager = getMemoryManager();

	if (memoryToAllocate == 0 || memoryManager == NULL) {
		driver_printStr(
			"allocMemory error: invalid size or uninitialized manager\n", RED);
		return NULL;
	}

	if (memoryManager->nextAddress + memoryToAllocate >
		memoryManager->endAddress) {
		driver_printStr("allocMemory error: insufficient memory\n", RED);
		return NULL;
	}

	void *allocation = memoryManager->nextAddress;
	memoryManager->nextAddress += memoryToAllocate;

	addBlock(memoryManager, allocation, memoryToAllocate);

	return allocation;
}

// Libera un bloque asignado
void freeMemory(void *ptr) {
	if (ptr == NULL) {
		driver_printStr("freeMemory error: null pointer\n", RED);
		return;
	}

	MemoryManagerADT memoryManager = getMemoryManager();
	removeBlock(memoryManager, ptr);
}

MemoryInfo *getMemoryInfo() {
	static MemoryInfo *info;
	MemoryManagerADT mm = getMemoryManager(); // recibe la direccion de memoria.
	// if (!mm || !info) return;

	uint64_t total = mm->endAddress - (char *) mm->blocks;
	uint64_t used  = 0;
	for (int i = 0; i < mm->blockCount; i++) {
		used += mm->blocks[i].size;
	}
	info->totalMemory = total;
	info->usedMemory  = used;
	info->freeMemory  = total - used;
	info->usedBlocks  = mm->blockCount;
	info->freeBlocks =
		MAX_BLOCKS -
		mm->blockCount; // No hay bloques libres en el simple manager
	info->totalBlocks = MAX_BLOCKS; // Total de bloques gestionados
	return &info;
}

void printMemoryInfo(const MemoryInfo *info) {
	Color c = (Color) {0xFF, 0xFF, 0xFF};
	driver_printStr("\n[Memory Info]\n", c);
	driver_printStr("Total: ", c);
	driver_printNum(info->totalMemory, c);
	driver_printStr(" bytes\n", c);
	driver_printStr("Usada: ", c);
	driver_printNum(info->usedMemory, c);
	driver_printStr(" bytes\n", c);
	driver_printStr("Libre: ", c);
	driver_printNum(info->freeMemory, c);
	driver_printStr(" bytes\n", c);
	driver_printStr("Bloques usados: ", c);
	driver_printNum(info->usedBlocks, c);
	driver_printStr("\n", c);
	driver_printStr("Bloques libres: ", c);
	driver_printNum(info->freeBlocks, c);
	driver_printStr("\n", c);
	driver_printStr("Bloques totales: ", c);
	driver_printNum(info->totalBlocks, c);
	driver_printStr("\n", c);
}

#endif
