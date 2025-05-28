#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <linkedListADT.h>

typedef struct Semaphore {
    int value;
    uint8_t lock;
    LinkedListADT waitingQueue;
} Semaphore;

void initSemaphores();
Semaphore *my_sem_open(const char *name, int initialValue);
void my_sem_init(Semaphore *s, int initialValue);
int my_sem_close(const char *name);
void my_sem_wait(const char *name);
void my_sem_post(const char *name);
int strcmp(const char *str1, const char *str2);
void strncpy(char *dest, const char *src, int n);

#endif // SEMAPHORE_H