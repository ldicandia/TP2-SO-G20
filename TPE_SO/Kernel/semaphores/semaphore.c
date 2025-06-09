// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <semaphoreManager.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <videoDriver.h>
#include <schedule.h>
#include <linkedListADT.h>
#include "process.h"
#include "interrupts.h"
#include "memoryManager.h"
#include <globals.h>

#define MAX_SEMAPHORES 64

typedef struct Semaphore {
	int value;
	uint8_t lock; // Lock por semáforo (cada semáforo tiene su propio spinlock)
	LinkedListADT waitingQueue;
} Semaphore;

typedef struct SemaphoreCDT {
	Semaphore *semaphores[MAX_SEMAPHORES];
} SemaphoreCDT;

extern void acquire(uint8_t *lock);
extern void release(uint8_t *lock);
uint8_t globalSemLock =
	0; // Solo para proteger el array de semáforos al crear/cerrar

void initSemaphores() {
	SemaphoreCDT *mgr = (SemaphoreCDT *) SEMAPHORE_MANAGER_ADDRESS;
	for (int i = 0; i < MAX_SEMAPHORES; i++) {
		mgr->semaphores[i] = NULL;
	}
}

static SemaphoreCDT *getSemaphoreManager() {
	return (SemaphoreCDT *) SEMAPHORE_MANAGER_ADDRESS;
}

static void my_sem_init(Semaphore *sem, int initialValue) {
	sem->value		  = initialValue;
	sem->waitingQueue = createLinkedListADT();
	sem->lock		  = 0; // el lock propio del semáforo
}

// Mejora: barrer todos los procesos muertos al reanudar
static void resumeFirstAvailableProcess(LinkedListADT queue) {
	Node *n;
	while ((n = getFirst(queue))) {
		removeNode(queue, n);
		ProcessADT p = (ProcessADT) n->data;
		if (processIsAlive(getProcessId(p))) {
			setStatus(getProcessId(p), READY);
			// freeMemory(n);
			break;
		}
		// Si no está vivo, seguimos al siguiente
		// freeMemory(n);
	}
}

static int down(Semaphore *sem) {
	acquire(&sem->lock);
	while (sem->value == 0) {
		ProcessADT cur = getCurrentProcess();
		appendElement(sem->waitingQueue, cur);
		setStatus(getProcessId(cur), BLOCKED);
		release(&sem->lock);
		yield();
		acquire(&sem->lock); // re-adquirimos al despertar
	}
	sem->value--;
	release(&sem->lock);
	return 0;
}

static int up(Semaphore *sem) {
	acquire(&sem->lock);
	sem->value++;
	resumeFirstAvailableProcess(sem->waitingQueue);
	release(&sem->lock);
	return 0;
}

int mySemOpen(uint16_t sem_id, int initialValue) {
	SemaphoreCDT *mgr = getSemaphoreManager();
	if (sem_id >= MAX_SEMAPHORES) {
		return 0;
	}

	acquire(&globalSemLock);
	if (!mgr->semaphores[sem_id]) {
		Semaphore *sem			= allocMemory(sizeof(Semaphore));
		mgr->semaphores[sem_id] = sem;
		my_sem_init(sem, initialValue);
		release(&globalSemLock);
		return 1;
	}
	release(&globalSemLock);
	return 0;
}

int mySemClose(uint16_t sem_id) {
	SemaphoreCDT *mgr = getSemaphoreManager();
	if (sem_id >= MAX_SEMAPHORES)
		return -1;

	acquire(&globalSemLock);
	Semaphore *sem = mgr->semaphores[sem_id];
	if (sem) {
		freeLinkedListADTDeep(sem->waitingQueue);
		freeMemory(sem);
		mgr->semaphores[sem_id] = NULL;
		release(&globalSemLock);
		return 0;
	}
	release(&globalSemLock);
	return -1;
}

void mySemWait(uint16_t sem_id) {
	SemaphoreCDT *mgr = getSemaphoreManager();
	if (sem_id >= MAX_SEMAPHORES)
		return;
	Semaphore *s = mgr->semaphores[sem_id];
	if (!s)
		return;

	down(s);
}

void mySemPost(uint16_t sem_id) {
	SemaphoreCDT *mgr = getSemaphoreManager();
	if (sem_id >= MAX_SEMAPHORES)
		return;
	Semaphore *s = mgr->semaphores[sem_id];
	if (!s)
		return;

	up(s);
}
