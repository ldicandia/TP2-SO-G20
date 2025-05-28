#include "semaphore.h"
#include <stdlib.h>
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


Semaphore *my_sem_open(const char *name, int initialValue) {
    acquire(&globalSemLock);


    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i].inUse && strcmp(semaphores[i].name, name) == 0) {
            release(&globalSemLock);
            return &semaphores[i].sem;
        }
    }


    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (!semaphores[i].inUse) {
            strncpy(semaphores[i].name, name, SEM_NAME_MAX_LEN - 1);
            semaphores[i].name[SEM_NAME_MAX_LEN - 1] = '\0';
            semaphores[i].inUse = 1;
            my_sem_init(&semaphores[i].sem, initialValue);
            release(&globalSemLock);
            return &semaphores[i].sem;
        }
    }

    release(&globalSemLock);
    return NULL;
}




void my_sem_init(Semaphore *s, int initialValue) {
    s->value = initialValue;
    s->lock = 0;
    s->waitingQueue = createLinkedListADT();
    if (s->waitingQueue == NULL) {
        driver_printStr("\nError al alocar memoria para el semaforo", (Color) {0xFF, 0x00, 0x00});
    }
}

int my_sem_close(const char *name) {
    acquire(&globalSemLock);

    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i].inUse && strcmp(semaphores[i].name, name) == 0) {
            // Libera recursos asociados al semáforo
            if (semaphores[i].sem.waitingQueue != NULL) {
                freeLinkedListADTDeep(semaphores[i].sem.waitingQueue);
                semaphores[i].sem.waitingQueue = NULL;
            }
            semaphores[i].inUse = 0;
            release(&globalSemLock);
            return 0;
        }
    }

    release(&globalSemLock);
    return -1;
}


void my_sem_wait(const char *name) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i].inUse && strcmp(semaphores[i].name, name) == 0) {
            Semaphore *s = &semaphores[i].sem;
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
            return;
        }
    }
}

void my_sem_post(const char *name) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphores[i].inUse && strcmp(semaphores[i].name, name) == 0) {
            Semaphore *s = &semaphores[i].sem;
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
            return;
        }
    }

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