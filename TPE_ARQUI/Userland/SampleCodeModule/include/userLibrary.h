#ifndef USER_LIBRARY_H
#define USER_LIBRARY_H

#include <stdint.h>

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} Color;


void printChar(char c);

char getChar();

void printCharColor(char c, Color fnt);

void printStr(char * str);

int isChar(char c);

int isDigit(char c);

#endif // USER_LIBRARY_H