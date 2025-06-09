#ifndef STRING_LIB_H
#define STRING_LIB_H

#include <stdint.h>

int strchr(const char *str, char c);

char *strchrAndReturn(const char *str, char c);

char *strtok(char *str, const char *delim);

int strlen(const char *str);

int atoi(const char *str);

#endif // STRING_LIB_H