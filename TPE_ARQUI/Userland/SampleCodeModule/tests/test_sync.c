#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include "test_util.h"

#include "userLibrary.h"

#define SEM_ID 20
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
	uint64_t aux = *p;
	yield(); // This makes the race condition highly probable
	aux += inc;
	*p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
	uint64_t n;
	int8_t inc;
	int8_t use_sem;

	if (argc != 3)
		return -1;

	if ((n = satoi(argv[0])) <= 0)
		return -1;
	if ((inc = satoi(argv[1])) == 0)
		return -1;
	if ((use_sem = satoi(argv[2])) < 0)
		return -1;

	if (use_sem)
		if (!user_sem_open(SEM_ID, 1)) {
			printStr("test_sync: ERROR opening semaphore\n");
			return -1;
		}

	uint64_t i;
	for (i = 0; i < n; i++) {
		if (use_sem)
			user_sem_wait(SEM_ID);
		slowInc(&global, inc);
		if (use_sem)
			user_sem_post(SEM_ID);
	}

	if (use_sem)
		user_sem_close(SEM_ID);

	return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
	uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

	if (argc != 2)
		return -1;

	char *argvDec[] = {argv[0], "-1", argv[1], NULL};
	char *argvInc[] = {argv[0], "1", argv[1], NULL};

	global = 0;

	uint64_t i;
	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		pids[i] = create_process((MainFunction) my_process_inc, argvDec, "my_process_inc", 3);
		pids[i + TOTAL_PAIR_PROCESSES] =
			create_process((MainFunction) my_process_inc, argvInc, "my_process_inc", 3);
	}

	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		user_sem_wait(pids[i]);
		user_sem_wait(pids[i + TOTAL_PAIR_PROCESSES]);
	}

	printStr("Final value: \n");
	printInteger(global);

	return 0;
}