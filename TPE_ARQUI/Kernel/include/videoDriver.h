#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H
#include <bitMap.h>
#include <stdint.h>
#include <globals.h>

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

uint32_t rgbToHex(uint8_t r, uint8_t g, uint8_t b);

void driver_printChar(char letter, Color color);

void driver_printStr(char *str, Color color);

void driver_printNum(int num, Color color);

void scroll();

void driver_newLine();

void driver_newLine_noChar();

void driver_clear();

void driver_increment_size();

void driver_decrement_size();

void driver_backspace();

void drawChar(char letter, Color color);

void drawSquare(int x, int y, uint32_t fillColor);

void initVideoDriver();

#endif
