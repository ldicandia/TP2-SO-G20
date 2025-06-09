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
#include <sys_calls.h>
#include <test_sync.h>
#include <phylo.h>
#include <builtin.h>
#include <process_commands.h>
#include <string_lib.h>

#include "userLibrary.h"
#define MAX_BUFFER 254
#define COMMANDS_SIZE 25
#define DEV_NULL -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2

static char last_command[MAX_BUFFER + 1] = {0};

static void copy_command(char *dest, const char *src) {
	int i = 0;
	while (i < MAX_BUFFER && src[i]) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

typedef int (*MainFunction)(int argc, char **args);

static void printLine(char c);
int linePos = 0;
char lastc;
int lastEnter				   = 0;
char line[MAX_BUFFER + 1]	   = {0};
char command[MAX_BUFFER + 1]   = {0};
char parameter[MAX_BUFFER + 1] = {0};
char buffer[MAX_BUFFER]		   = {0};
char *command_names[]		   = {
	 "help",	   "time",			"clear",	 "snake 1",	  "snake 2",
	 "inforeg",	   "zerodiv",		"invopcode", "increment", "decrement",
	 "testMemory", "testProcesses", "testPrio",	 "testSync",  "ps",
	 "mem",		   "loop",			"kill",		 "nice",	  "block",
	 "wc",		   "filter",		"cat",		 "phylo",	  "red"};

MainFunction command_func[COMMANDS_SIZE] = {(MainFunction) help,
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
											(MainFunction) test_prio,
											(MainFunction) test_sync,
											_ps_wrap,
											_mem_wrap,
											(MainFunction) loop,
											_kill_wrap,
											_nice_wrap,
											_block_wrap,
											(MainFunction) wc,
											(MainFunction) filter,
											(MainFunction) cat,
											(MainFunction) phylo_wrap,
											(MainFunction) red};

void shell() {
	printStrColor("\nITBA Shell Group 20\n", (Color) {0xFF, 0xFF, 0x00});
	printStrColor("ALFIERI - DI CANDIA - DIAZ VARELA\n",
				  (Color) {0xFF, 0xFF, 0x00});
	char c;

	printStr("\n--------------------| SHELL |--------------------\n");

	while (1) {
		c = getChar();
		if (lastc != c) {
			printLine(c);
		}
	}
}

static void printLine(char c) {
	if (c == '=' && lastEnter == 0 && last_command[0] != '\0') {
		int i = 0;
		while (last_command[i]) {
			buffer[lastEnter++] = last_command[i];
			printChar(last_command[i]);
			i++;
		}
		lastc = c;
		return;
	}

	if (isChar(c) || isDigit(c) || c == ' ' || c == '|' || c == '&' ||
		c == '-' || c == '_' || c == '/' || c == '.' || c == ',' || c == ':' ||
		c == '\\' || c == '=') {
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

static int strcmp_shell(const char *str, const char *cmd) {
	int i = 0;
	while (cmd[i] != '\0') {
		if (str[i] != cmd[i]) {
			return 0;
		}
		i++;
	}
	return (str[i] == ' ' || str[i] == '\0');
}

void readCommand() {
	if (buffer[0] != '\0') {
		copy_command(last_command, buffer);
	}

	int background = 0;
	if (buffer[0] == '&') {
		background = 1;
		for (int i = 0; buffer[i] != '\0'; i++) {
			buffer[i] = buffer[i + 1];
		}
	}

	if (strchr(buffer, '|')) {
		char *bar		= strchrAndReturn(buffer, '|');
		*bar			= '\0';
		char *left_cmd	= buffer;
		char *right_cmd = bar + 1;
		while (*right_cmd == ' ')
			right_cmd++;

		char *argv1[5] = {NULL}, *argv2[5] = {NULL};
		int argc1 = 0, argc2 = 0;
		char *tok;
		tok = strtok(left_cmd, " ");
		while (tok && argc1 < 4) {
			argv1[argc1++] = tok;
			tok			   = strtok(NULL, " ");
		}
		argv1[argc1] = NULL;
		tok			 = strtok(right_cmd, " ");
		while (tok && argc2 < 4) {
			argv2[argc2++] = tok;
			tok			   = strtok(NULL, " ");
		}
		argv2[argc2] = NULL;

		int i1 = -1, i2 = -1;
		for (int i = 0; i < COMMANDS_SIZE; i++) {
			if (strcmp_shell(argv1[0], command_names[i]))
				i1 = i;
			if (strcmp_shell(argv2[0], command_names[i]))
				i2 = i;
		}
		if (i1 < 0 || i2 < 0) {
			notFound();
			return;
		}
		int pipeId = getPipe();

		int16_t fileDescriptors1[] = {STDIN, pipeId, STDERR};
		// stdout → fds[1]
		int pid1 = create_process_with_fds(
			command_func[i1], argv1, command_names[i1], 1, fileDescriptors1);

		int16_t fileDescriptors2[] = {pipeId, STDOUT, STDERR};
		// stdin ← fds[0]
		int pid2 = create_process_with_fds(
			command_func[i2], argv2, command_names[i2], 1, fileDescriptors2);

		releasePipeAccess(pid1);
		releasePipeAccess(pid2);

		wait_pid(pid1);
		wait_pid(pid2);
		return;
	}

	for (int i = 0; i < COMMANDS_SIZE; i++) {
		if (strcmp_shell(buffer, command_names[i])) {
			if (i <= 2) {
				command_func[i](0, NULL);
			}
			else {
				char *argv[4] = {NULL};
				int argc	  = 0;
				char *tok	  = strtok(buffer, " ");
				while (tok && argc < 3) {
					argv[argc++] = tok;
					tok			 = strtok(NULL, " ");
				}
				argv[argc] = NULL;

				int pid = 0;
				if (background) {
					pid = create_process_with_fds(
						command_func[i], argv, command_names[i], 1,
						(int16_t[]) {DEV_NULL, STDOUT, STDERR});
				}
				else {
					pid = create_process_with_fds(
						command_func[i], argv, command_names[i], 1,
						(int16_t[]) {STDIN, STDOUT, STDERR});
					wait_pid(pid);
				}
			}
			return;
		}
	}
	notFound();
}
