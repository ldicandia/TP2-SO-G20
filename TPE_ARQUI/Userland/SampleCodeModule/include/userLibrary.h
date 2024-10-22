#ifndef USER_LIBRARY_H
#define USER_LIBRARY_H

#include <stdint.h>
#include <color.h>

void printChar(char c);

char getChar();

void printCharColor(char c, Color fnt);

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

#endif // USER_LIBRARY_H