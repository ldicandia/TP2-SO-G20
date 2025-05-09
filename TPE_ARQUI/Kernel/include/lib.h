#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

char *cpuVendor(char *result);

unsigned int log(uint64_t n, int base);

int stringArrayLen(char **array);

int strcpychar(char *dest, const char *origin, char limit);

int strcpy(char *dest, const char *origin);

int strlen(const char *str);

void forceTimerTick();

#endif