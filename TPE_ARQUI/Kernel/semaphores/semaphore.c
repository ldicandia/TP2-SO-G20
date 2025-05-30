#include "semaphore.h"
#include <stdlib.h>
#include <stdint.h>
#include <videoDriver.h>
#include <schedule.h>
#include <linkedListADT.h>
#include "schedule.h"
#include "process.h"
#include "interrupts.h"
#include "memoryManager.h"
#define MAX_SEMAPHORES 64
#define SEM_NAME_MAX_LEN 32


extern void acquire(uint8_t *lock);
extern void release(uint8_t *lock);



uint8_t globalSemLock = 0;


typedef struct NamedSemaphore {
    char name[SEM_NAME_MAX_LEN];
    Semaphore sem;
    int inUse;
} NamedSemaphore;

NamedSemaphore semaphores[MAX_SEMAPHORES];


void initSemaphores() {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphores[i].inUse = 0;
    }
}


// Cambia la función para devolver el índice (id) del semáforo
int my_sem_open(uint16_t sem_id, int initialValue) {
    if (sem_id < 0 || sem_id >= MAX_SEMAPHORES)
        return 0;

    acquire(&globalSemLock);

    if (!semaphores[sem_id].inUse) {
        semaphores[sem_id].name[0] = '\0'; // Opcional: limpiar nombre
        semaphores[sem_id].inUse = 1;
        my_sem_init(&semaphores[sem_id].sem, initialValue);
        release(&globalSemLock);
        return 1;
    }

    release(&globalSemLock);
    return 0; // Ya estaba en uso
}

void my_sem_init(Semaphore *sem, int initialValue) {
    sem->value = initialValue;
    sem->waitingQueue = createLinkedListADT();
    sem->lock = 0;
}

int my_sem_close(uint16_t sem_id) {
    if (sem_id < 0 || sem_id >= MAX_SEMAPHORES) return -1;
    acquire(&globalSemLock);

    if (semaphores[sem_id].inUse) {
        if (semaphores[sem_id].sem.waitingQueue != NULL) {
            freeLinkedListADTDeep(semaphores[sem_id].sem.waitingQueue);
            semaphores[sem_id].sem.waitingQueue = NULL;
        }
        semaphores[sem_id].inUse = 0;
        release(&globalSemLock);
        return 0;
    }

    release(&globalSemLock);
    return -1;
}

void my_sem_wait(uint16_t sem_id) {
    if (sem_id < 0 || sem_id >= MAX_SEMAPHORES) return;
    if (!semaphores[sem_id].inUse) return;

    Semaphore *s = &semaphores[sem_id].sem;
    acquire(&s->lock);
    s->value--;
    if (s->value < 0) {
        ProcessADT currentProcess = getCurrentProcess();
        if (currentProcess != NULL) {
            setStatus(get_pid(currentProcess), BLOCKED);
            appendElement(s->waitingQueue, currentProcess);
        }
        release(&s->lock);
        yield();
    } else {
        release(&s->lock);
    }
}

void my_sem_post(uint16_t sem_id) {
    if (sem_id < 0 || sem_id >= MAX_SEMAPHORES) return;
    if (!semaphores[sem_id].inUse) return;

    Semaphore *s = &semaphores[sem_id].sem;
    acquire(&s->lock);
    s->value++;

    if (s->value <= 0) {
        Node *waitingProcessNode = getFirst(s->waitingQueue);
        if (waitingProcessNode != NULL) {
            ProcessADT waitingProcess = (ProcessADT) waitingProcessNode->data;
            setStatus(get_pid(waitingProcess), READY);
            freeMemory(waitingProcessNode);
        }
    }

    release(&s->lock);
}



int strcmp(const char *str1, const char *str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

void strncpy(char *dest, const char *src, int n) {
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
}