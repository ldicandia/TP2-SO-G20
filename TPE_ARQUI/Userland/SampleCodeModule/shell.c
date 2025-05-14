// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shell.h"

#include <snake.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <user_time.h>
#include <test_mm.h>
#include <test_processes.h>
#include <test_prio.h>

#include "userLibrary.h"
#define MAX_BUFFER 254
#define COMMANDS_SIZE 12

static void printLine(char c);
int linePos = 0;
char lastc;
int lastEnter			  = 0;
char line[MAX_BUFFER + 1] = {0};

char command[MAX_BUFFER + 1]   = {0};
char parameter[MAX_BUFFER + 1] = {0};

char buffer[MAX_BUFFER] = {0};

char *command_names[] = {"help",	  "time",		"clear",
						 "snake 1",	  "snake 2",	"inforeg",
						 "zerodiv",	  "invopcode",	"increment",
						 "decrement", "testMemory", "testProcesses"};

void (*command_func[COMMANDS_SIZE])() = {help,
										 user_time,
										 clear,
										 shell_snake_1,
										 shell_snake_2,
										 inforeg,
										 exc_zerodiv,
										 exc_invopcode,
										 increment_size_char,
										 decrement_size_char,
										 testMemory,
										 testProcesses};

void shell() {
	char c;
	// testMemory();
	// testProcesses();
	test_prio();
	while (1) {
		c = getChar();
		if (lastc != c) {
			printLine(c);
		}
	}
}

static void printLine(char c) {
	if (isChar(c) || isDigit(c) || c == ' ') {
		buffer[lastEnter] = c;
		lastEnter++;
	}
	if (c == '\n') {
		buffer[lastEnter] = '\0';
		readCommand();
		buffer[0] = '\0';
		printStr(buffer);
		lastEnter = 0;
	}

	if (c == '\b' && lastEnter > 0) {
		lastEnter--;
		buffer[lastEnter] = '\0';
	}
	else if (c == '\b' && lastEnter == 0) {
		lastc = c;
		return;
	}

	printChar(c);
	lastc = c;
}

void help() {
	static Color WHITE = {0xFF, 0xFF, 0xFF};
	printStrColor("\n--------------------| MANUAL |--------------------",
				  WHITE);
	printStrColor("\n time                     shows actual time", WHITE);
	printStrColor("\n clear                    clears the screen", WHITE);
	printStrColor("\n snake [qty players]      starts de Snake game", WHITE);
	printStrColor(
		"\n inforeg                  after SHIFT + TAB prints regsiter's",
		WHITE);
	printStrColor("\n zerodiv                  division by zero exception test",
				  WHITE);
	printStrColor(
		"\n invopcode                tests the invalid operation code", WHITE);
	printStrColor("\n increment                increase letter size", WHITE);
	printStrColor("\n decrement                decrease letter size", WHITE);
	printStrColor("\n testMemory [size]        tests memory allocation", WHITE);
	printStrColor("\n testProcesses [size]     tests process creation", WHITE);
	printStrColor("\n---------------------------------------------------",
				  WHITE);
}

static int strcmp_shell(char *str1, char *str2) {
	int i = 0;
	while (str1[i] != '\0' && str2[i] != '\0') {
		if (str1[i] != str2[i]) {
			return 0;
		}
		i++;
	}
	if (str1[i] != str2[i]) {
		return 0;
	}
	return 1;
}

void readCommand() {
	for (int i = 0; i < COMMANDS_SIZE; i++) {
		if (strcmp_shell(buffer, command_names[i])) {
			command_func[i]();
			return;
		}
	}
	notFound();
}

void notFound() {
	printStr("\n command not found. Try using 'help'");
	return;
}

void shell_snake_1() {
	snake(1);
}

void shell_snake_2() {
	snake(2);
}

void testMemory() {
	uint64_t argc = 1;
	char *argv[]  = {"1000"};
	test_mm(argc, argv);
}

void testProcesses() {
	// MODO DEBUG -> argc = 2, argv[0] = "20", argv[1] = "1"
	// MODO NORMAL -> argc = 1 argv[0] = "20"
	uint64_t argc = 1;
	char *argv[]  = {"20"};
	test_processes(argc, argv);
}
