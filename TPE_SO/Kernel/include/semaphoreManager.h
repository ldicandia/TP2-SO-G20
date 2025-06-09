#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <linkedListADT.h>

typedef struct SemaphoreCDT *SemaphoreADT;

void initSemaphores();

int mySemOpen(uint16_t sem_id, int initialValue);

int mySemClose(uint16_t sem_id);

void mySemWait(uint16_t sem_id);

void mySemPost(uint16_t sem_id);

int strcmp(const char *str1, const char *str2);

#endif // SEMAPHORE_H