// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

	if (argc != 4)
		return -1;

	if ((n = satoi(argv[1])) <= 0)
		return -1;
	if ((inc = satoi(argv[2])) == 0)
		return -1;
	if ((use_sem = satoi(argv[3])) < 0)
		return -1;

	if (use_sem)
		if (user_sem_open(SEM_ID, 1) == -1) {
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

	return 0;
}

//{"name", "argv[1]", "argv[2]", "argv[3]"}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
	uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

	// check params
	if (argc < 2 || argc > 3) {
		printStr("\nUsage: test_sync <n> [use_sem]\n");
		return -1;
	}

	// agregado para que funcione con nuestra implementación de semáforos
	int8_t useSem = satoi(argv[2]);
	if (useSem) {
		if (user_sem_open(SEM_ID, 1) == -1) {
			printStr("test_sync: ERROR creating semaphore\n");
			return -1;
		}
	}

	if (argc != 3)
		return -1;

	char *argvDec[] = {"my_processDec", argv[1], "-1", argv[2], NULL};
	char *argvInc[] = {"my_processInc", argv[1], "1", argv[2], NULL};

	global = 0;

	uint64_t i;
	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		pids[i] = create_process((MainFunction) my_process_inc, argvDec,
								 "my_process_dec", 3);
		pids[i + TOTAL_PAIR_PROCESSES] = create_process(
			(MainFunction) my_process_inc, argvInc, "my_process_inc", 3);
	}

	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		wait_pid(pids[i]);
		wait_pid(pids[i + TOTAL_PAIR_PROCESSES]);
	}

	printStr("Final value: \n");
	printInteger(global);

	if (useSem)
		user_sem_close(SEM_ID);

	return 0;
}
