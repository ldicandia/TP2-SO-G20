// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include "test_util.h"

#define MINOR_WAIT "500000000"
#define WAIT 1000000000

#define TOTAL_PROCESSES 3
#define LOWEST 0
#define MEDIUM 1
#define HIGHEST 2

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

static char *intToChar(int i) {
	static char buffer[12];
	int j = 0;

	if (i < 0) {
		buffer[j++] = '-';
		i			= -i;
	}

	if (i == 0) {
		buffer[j++] = '0';
	}

	while (i > 0) {
		buffer[j++] = (i % 10) + '0';
		i /= 10;
	}

	buffer[j] = '\0';

	for (int k = 0; k < j / 2; k++) {
		char temp		  = buffer[k];
		buffer[k]		  = buffer[j - k - 1];
		buffer[j - k - 1] = temp;
	}

	return buffer;
}

void endless_pid(int argc, char **argv) {
	while (1) {
		printStr(argv[1]);
		printStr("  ");
		for (int i = 0; i < 100000000; i++)
			;
		yield();
	}
}

#define DEV_NULL -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2

int test_prio() {
	int64_t pids[TOTAL_PROCESSES];
	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++) {
		char *args_pid[] = {"endless_loopA", intToChar(i), NULL};

		pids[i] = create_process_with_fds(
			(MainFunction) endless_pid, args_pid, "print_A", 2,
			(int16_t[]) {DEV_NULL, STDOUT, STDERR});
	}

	bussy_wait(WAIT);
	printStr("\nCHANGING PRIORITIES...\n");
	printStr("NEW PROCESS PRIORITY:\n");
	for (i = 0; i < TOTAL_PROCESSES; i++) {
		printStr("PROCESS: ");
		printStr(intToChar(i));
		printStr("  ");
		printStr(intToChar(prio[i]));
		printStr("\n");
	}

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], prio[i]);

	bussy_wait(WAIT);
	printStr("\nBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_block(pids[i]);

	printStr("CHANGING PRIORITIES WHILE BLOCKED...\n");
	printStr("NEW PROCESS PRIORITY:\n");
	for (i = 0; i < TOTAL_PROCESSES; i++) {
		printStr("PROCESS: ");
		printStr(intToChar(i));
		printStr("  ");
		printStr(intToChar(MEDIUM));
		printStr("\n");
	}

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], MEDIUM);

	printStr("UNBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_unblock(pids[i]);

	bussy_wait(WAIT);
	printStr("\nKILLING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_kill(pids[i]);

	return 0;
}
