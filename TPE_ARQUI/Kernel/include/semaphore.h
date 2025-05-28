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
int my_sem_open(uint16_t sem_id, int initialValue);
int my_sem_close(uint16_t sem_id);
void my_sem_wait(uint16_t sem_id);
void my_sem_post(uint16_t sem_id);
int strcmp(const char *str1, const char *str2);
void strncpy(char *dest, const char *src, int n);
void my_sem_init(Semaphore *sem, int initialValue);

#endif // SEMAPHORE_H