#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H
#include <stdint.h>
#include <bitMap.h>

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} Color;

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
uint32_t rgbToHex(uint8_t r, uint8_t g, uint8_t b);
void driver_printChar(char letter, Color color);
void driver_printStr(char * str, Color color);
void scroll();
void driver_newLine();
void driver_newLine_noChar();



#endif

