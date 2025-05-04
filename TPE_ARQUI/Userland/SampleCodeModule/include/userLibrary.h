#ifndef USER_LIBRARY_H
#define USER_LIBRARY_H

#include <color.h>
#include <stdint.h>
#include <userLibrary.h>

void printChar(char c);

char getChar();

void printCharColor(char c, Color fnt);

void printStrColor(char *str, Color fnt);

void printStr(char *str);

int isChar(char c);

int isDigit(char c);

void increment_size_char();

void decrement_size_char();

void exc_invopcode();

void exc_zerodiv();

void clear();

void inforeg();

void printHex(uint64_t n);

void uint64ToHex(uint64_t n, char buf[16]);

void drawSquare(int x, int y, uint32_t fillColor);

void sleep_miliseconds(int miliseconds);

void playSound(uint32_t frequence, uint64_t duration);

void stopSound();

void *allocMemory(uint64_t size);

void freeMemory(void *address);

#endif // USER_LIBRARY_H