// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "keyboard.h"

#include <stdint.h>

#include "time.h"
#include "videoDriver.h"
#include <process.h>
#include <schedule.h>

unsigned char scanCode = 0;
static char retChar	   = 0;
static int shift	   = 0;
static int capsLock	   = 0;
static int ctrl		   = 0;

static const char hexMapPressed[256] = {
	0,	  0,   '1',	 '2',  '3', '4', '5', '6', '7', '8', '9',
	'0',  '-', '=',	 '\b', // backspace,
	'\t', 'q', 'w',	 'e',  'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[',  ']', '\n', // enter
	0,	  'a', 's',	 'd',  'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 0,	 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',',  '.', '/',	 0,	   0,	0,	 ' ', 0,   0,	0,	 0,
	0,	  0,   0,	 0,	   0,	0,	 0,	  0,   0,	0,	 0,
	0,	  0,   0,	 0,	   0,	0,	 0,	  0,   0,	0};

void keyboard_master(uint8_t keyPressed) {
	scanCode = keyPressed;

	// shift pressed
	if (scanCode == 0x2A || scanCode == 0x36) {
		shift = 1;
	}
	// shift not pressed
	if (scanCode == 0xAA || scanCode == 0xB6) {
		shift = 0;
	}
	// ctrl pressed
	if (scanCode == 0x1D) {
		ctrl = 1;
	}
	// ctrl not pressed
	if (scanCode == 0x9D) {
		ctrl = 0;
	}

	// capsLock
	if (scanCode == 0x3A) {
		capsLock = (capsLock + 1) % 2;
	}
}

char getCharFromKeyboard() {
	if (scanCode > 0x80 || scanCode == 0x0F) {
		retChar = 0;
	}
	else {
		retChar = hexMapPressed[scanCode];
	}

	if ((retChar >= 'a' && retChar <= 'z') && (shift == 1 || capsLock == 1)) {
		return retChar - ('a' - 'A');
	}

	if (ctrl == 1 && retChar == 'c') {
		return 0;
	}

	return retChar;
}

void ctrl_c_handler() { // TODO
	// driver_printChar('a', (Color) {0xFF, 0xFF, 0xFF});  PARA CHEQUEAR Q ANDA
	// CTRL + C
	killForegroundProcess();
	return;
}

void clearScanCode() {
	scanCode = 0;
}

unsigned char getScanCode() {
	return scanCode;
}