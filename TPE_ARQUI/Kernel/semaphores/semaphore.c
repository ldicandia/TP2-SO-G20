#include "semaphore.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // <-- para memset
#include <videoDriver.h>
#include <schedule.h>
#include <linkedListADT.h>
#include "process.h"
#include "interrupts.h"
#include "memoryManager.h"
#define MAX_SEMAPHORES 64
#define SEMAPHORE_MANAGER_ADDRESS 0x70000

typedef struct Semaphore {
	int value;
	uint8_t lock;
	LinkedListADT waitingQueue;
	LinkedListADT mutexQueue;
} Semaphore;

typedef struct SemaphoreCDT {
	Semaphore *semaphores[MAX_SEMAPHORES];
} SemaphoreCDT;

extern void acquire(uint8_t *lock);
extern void release(uint8_t *lock);
uint8_t globalSemLock = 0;

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
	sem->mutexQueue	  = createLinkedListADT();
	sem->lock		  = 0;
}

static void resumeFirstAvailableProcess(LinkedListADT queue) {
	Node *n;
	while ((n = getFirst(queue))) {
		removeNode(queue, n);
		ProcessADT p = (ProcessADT) n->data;
		freeMemory(n);
		if (processIsAlive(get_pid(p))) {
			setStatus(get_pid(p), READY);
			break;
		}
	}
}

static int down(Semaphore *sem) {
	acquire(&sem->lock);
	sem->value--;
	if (sem->value < 0) {
		ProcessADT cur = getCurrentProcess();
		appendElement(sem->waitingQueue, cur);
		setStatus(get_pid(cur), BLOCKED);
		release(&sem->lock);
		yield();
	}
	else {
		release(&sem->lock);
	}
	return 0;
}

static int up(Semaphore *sem) {
	acquire(&sem->lock);
	sem->value++;
	if (sem->value <= 0) {
		resumeFirstAvailableProcess(sem->waitingQueue);
	}
	release(&sem->lock);
	return 0;
}

int my_sem_open(uint16_t sem_id, int initialValue) {
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

int my_sem_close(uint16_t sem_id) {
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

void my_sem_wait(uint16_t sem_id) {
	SemaphoreCDT *mgr = getSemaphoreManager();
	if (sem_id >= MAX_SEMAPHORES)
		return;
	Semaphore *s = mgr->semaphores[sem_id];
	if (!s)
		return;

	down(s);
}

void my_sem_post(uint16_t sem_id) {
	SemaphoreCDT *mgr = getSemaphoreManager();
	if (sem_id >= MAX_SEMAPHORES)
		return;
	Semaphore *s = mgr->semaphores[sem_id];
	if (!s)
		return;

	up(s);
}