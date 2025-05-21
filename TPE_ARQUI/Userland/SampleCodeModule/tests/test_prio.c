#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include "test_util.h"

#define MINOR_WAIT                                                             \
	"500000000" // TODO: Change this value to prevent a process from flooding
				// the
				// screen
#define WAIT                                                                   \
	10000000 // TODO: Change this value to make the wait long enough to see
			 // theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 1  // TODO: Change as required
#define HIGHEST 2 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

int test_prio() {
	int64_t pids[TOTAL_PROCESSES];
	char *args[] = {"endless_loop_print", MINOR_WAIT, 0};
	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++) {
		pids[i] = create_process((MainFunction) endless_loop_print, args,
								 "endless_loop_print", 4);
	}

	bussy_wait(WAIT);
	printStr("\nCHANGING PRIORITIES...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], prio[i]);

	bussy_wait(WAIT);
	printStr("\nBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_block(pids[i]);

	printStr("CHANGING PRIORITIES WHILE BLOCKED...\n");

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
