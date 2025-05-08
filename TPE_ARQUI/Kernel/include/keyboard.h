#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "lib.h"

void keyboard_master(uint8_t keyPressed);

char getCharFromKeyboard();

unsigned char getScanCode();

void ctrl_c_handler();

void clearScanCode();

#endif