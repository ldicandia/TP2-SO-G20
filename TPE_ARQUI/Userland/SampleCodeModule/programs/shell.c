// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shell.h"

#include <snake.h>
#include <stdio.h>
#include <stdlib.h>
#include <user_time.h>
#include <test_mm.h>
#include <test_processes.h>
#include <test_prio.h>

#include "userLibrary.h"
#define MAX_BUFFER 254
#define COMMANDS_SIZE 13

typedef int (*MainFunction)(int argc, char **args);

static void printLine(char c);
int linePos = 0;
char lastc;
int lastEnter			  = 0;
char line[MAX_BUFFER + 1] = {0};

char command[MAX_BUFFER + 1]   = {0};
char parameter[MAX_BUFFER + 1] = {0};

char buffer[MAX_BUFFER] = {0};

char *command_names[] = {
	"help",		  "time",		   "clear",		"snake 1",	 "snake 2",
	"inforeg",	  "zerodiv",	   "invopcode", "increment", "decrement",
	"testMemory", "testProcesses", "testPrio"};

// 1) Creamos los wrappers para los comandos “externos”:
static int _shell_snake_1(int argc, char **argv) {
	snake(1);
	return 0;
}
static int _shell_snake_2(int argc, char **argv) {
	snake(2);
	return 0;
}
static int _inforeg_wrap(int argc, char **argv) {
	inforeg();
	return 0;
}
static int _zerodiv_wrap(int argc, char **argv) {
	exc_zerodiv();
	return 0;
}
static int _invop_wrap(int argc, char **argv) {
	exc_invopcode();
	return 0;
}
static int _inc_wrap(int argc, char **argv) {
	increment_size_char();
	return 0;
}
static int _dec_wrap(int argc, char **argv) {
	decrement_size_char();
	return 0;
}

// 2) Cambiamos el array de handlers para que apunte a MainFunction:
MainFunction command_func[COMMANDS_SIZE] = {(MainFunction) help, // built-ins
											(MainFunction) user_time,
											(MainFunction) clear,
											_shell_snake_1,
											_shell_snake_2,
											_inforeg_wrap,
											_zerodiv_wrap,
											_invop_wrap,
											_inc_wrap,
											_dec_wrap,
											(MainFunction) test_mm,
											(MainFunction) test_processes,
											(MainFunction) test_prio};

void infiniteLoop(uint64_t argc, char *argv[]) {
	while (1) {
		printChar('a');
		yield();
	}
}

void test_prio_wrapper(uint64_t argc, char *argv[]) {
	argc		= 1;
	int64_t pid = test_prio();
	if (pid == -1) {
		printStr("\nError creating processes test\n");
	}
	else {
		printStr("\nProcesses test created with PID: ");
		printInteger(pid);
		printStr("\n");
	}
}

void endless_A(int argc, char **argv) {
	while (1) {
		printStr("A");
		for (int i = 0; i < 100000000; i++)
			;
		yield(); // Cede el control manualmente
	}
}

void endless_B(int argc, char **argv) {
	while (1) {
		printStr("B");
		for (int i = 0; i < 100000000; i++)
			;
		yield(); // No hace yield, depende del quantum para ser expulsado
	}
}

void shell() {
	printStrColor("\nITBA Shell Group 20\n", (Color) {0xFF, 0xFF, 0x00});
	printStrColor("ALFIERI - DI CANDIA - DIAZ VARELA\n",
				  (Color) {0xFF, 0xFF, 0x00});
	char c;

	printStr("\n--------------------| SHELL |--------------------\n");

	// testProcesses();

	// BBBBBBBBBAABBBBBBBBAABBBBB

	char *argsA[] = {"endless_loopA", NULL};
	char *argsB[] = {"endless_loopB", NULL};

	create_process(endless_A, argsA, "print_A", 2); // prioridad alta
	create_process(endless_B, argsB, "print_B", 4); // prioridad media

	while (1) {
		// c = getChar();
		// if (lastc != c) {
		// 	printLine(c);
		// }
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

static int strcmp_shell(const char *str, const char *cmd) {
	int i = 0;
	// comparamos mientras cmd no termine
	while (cmd[i] != '\0') {
		if (str[i] != cmd[i]) {
			return 0;
		}
		i++;
	}
	// tras coincidir todo cmd, str[i] debe ser espacio o '\0'
	return (str[i] == ' ' || str[i] == '\0');
}

static int strchr(const char *str, char c) {
	while (*str) {
		if (*str == c) {
			return 1;
		}
		str++;
	}
	return 0;
}

static char *strtok(char *str, const char *delim) {
	static char *last = NULL;
	if (str == NULL) {
		str = last;
	}
	if (str == NULL) {
		return NULL;
	}
	while (*str && strchr(delim, *str)) {
		str++;
	}
	if (*str == '\0') {
		last = NULL;
		return NULL;
	}
	char *start = str;
	while (*str && !strchr(delim, *str)) {
		str++;
	}
	if (*str) {
		*str++ = '\0';
	}
	last = str;
	return start;
}

// 3) Adaptamos readCommand() para diferenciar built-ins de procesos:
void readCommand() {
	for (int i = 0; i < COMMANDS_SIZE; i++) {
		if (strcmp_shell(buffer, command_names[i])) {
			// built-ins (índices 0,1,2)
			if (i <= 2) {
				command_func[i](0, NULL);
			}
			else {
				// parseo muy sencillo de argumentos
				char *argv[4] = {NULL};
				int argc	  = 0;
				char *tok	  = strtok(buffer, " ");
				while (tok && argc < 3) {
					argv[argc++] = tok;
					tok			 = strtok(NULL, " ");
				}
				argv[argc] = NULL;
				// lanzamos proceso en background con prioridad 0

				create_process(command_func[i], argv, command_names[i], 1);
			}
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
