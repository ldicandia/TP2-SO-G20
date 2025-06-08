// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "keyboard.h"

#include <stdint.h>
#include "time.h"
#include "videoDriver.h"
#include <process.h>
#include <schedule.h>
#include <semaphoreManager.h>
#include <pipeManager.h>

#define EOF (-1)
#define KEYBOARD_SEM 1 // ID del semáforo para el teclado

// Buffer circular de teclado
#define KEYBOARD_BUFFER_SIZE 128

static char keyboardBuffer[KEYBOARD_BUFFER_SIZE];
static int bufferHead = 0;
static int bufferTail = 0;

// Estados de las teclas modificadoras
static int shift	= 0;
static int capsLock = 0;
static int ctrl		= 0;

// Agrega esta variable global para indicar actividad del teclado
static int keyboardActivity = 0;

// Mapa de scancodes a caracteres
static const char hexMapPressed[256] = {
	0,	  0,   '1',	 '2',  '3', '4', '5', '6', '7', '8', '9',
	'0',  '-', '=',	 '\b', // backspace
	'\t', 'q', 'w',	 'e',  'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[',  ']', '\n', // enter
	0,	  'a', 's',	 'd',  'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', 0,	 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',',  '.', '/',	 0,	   0,	0,	 ' ', 0,   0,	0,	 0,
	0,	  0,   0,	 0,	   0,	0,	 0,	  0,   0,	0,	 0,
	0,	  0,   0,	 0,	   0,	0,	 0,	  0,   0,	0};

// Inicializa el teclado
void initializeKeyboard() {
	my_sem_open(KEYBOARD_SEM, 0);
}

// Agrega un caracter al buffer
void keyboard_buffer_push(char c) {
	int nextHead = (bufferHead + 1) % KEYBOARD_BUFFER_SIZE;
	if (nextHead == bufferTail) {
		driver_printStr("Keyboard buffer overflow\n",
						(Color) {0xFF, 0x00, 0x00});
		return;
	}
	keyboardBuffer[bufferHead] = c;
	bufferHead				   = nextHead;
}

// Obtiene un caracter del buffer
char keyboard_buffer_pop() {
	if (bufferHead == bufferTail) {
		return 0;
	}
	char c	   = keyboardBuffer[bufferTail];
	bufferTail = (bufferTail + 1) % KEYBOARD_BUFFER_SIZE;
	return c;
}

// Esta es la función que llama el ASM (pasa el scan code leído)
void keyboard_master(uint8_t keyPressed) {
	keyboardActivity = 1;

	// Shift
	if (keyPressed == 0x2A || keyPressed == 0x36) {
		shift = 1;
		return;
	}
	if (keyPressed == 0xAA || keyPressed == 0xB6) {
		shift = 0;
		return;
	}

	// Ctrl
	if (keyPressed == 0x1D) {
		ctrl = 1;
		return;
	}
	if (keyPressed == 0x9D) {
		ctrl = 0;
		return;
	}

	// CapsLock
	if (keyPressed == 0x3A) {
		capsLock = (capsLock + 1) % 2;
		return;
	}

	// Si es una tecla de liberación, ignoramos
	if (keyPressed >= 0x80)
		return;

	char retChar = hexMapPressed[keyPressed];

	if ((retChar >= 'a' && retChar <= 'z') && (shift || capsLock)) {
		retChar -= ('a' - 'A');
	}

	if (retChar == '\\' && shift)
		retChar = '|';
	if (retChar == '7' && shift)
		retChar = '&';

	if (ctrl && retChar == 'c') {
		ctrl_c_handler();
		return;
	}

	if (ctrl && retChar == 'd') {
		keyboard_buffer_push(EOF);
		return;
	}

	keyboard_buffer_push(retChar);
}

char getCharFromKeyboard() {
	return keyboard_buffer_pop();
}

void ctrl_c_handler() {
	killForegroundProcess();
	return;
}

void ctrl_d_handler() {
	keyboard_buffer_push(EOF);
	return;
}

int checkKeyboardActivity() {
	int activity	 = keyboardActivity;
	keyboardActivity = 0;
	return activity;
}

void clearKeyboardBuffer() {
	bufferHead = bufferTail = 0;
}
