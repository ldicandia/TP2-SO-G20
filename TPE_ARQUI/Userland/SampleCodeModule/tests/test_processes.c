#include <stdio.h>
#include "syscall.h"
#include "test_util.h"
#include <test_processes.h>
#include <userLibrary.h>

enum State { RUNNING, BLOCKED, KILLED };

typedef struct P_rq {
	int32_t pid;
	enum State state;
} p_rq;

// int create_process(void *code, char **args, char *name, uint8_t priority);

int64_t test_processes(uint64_t argc, char *argv[]) {
	printStr("\nEntered Testing Processes...\n");
	uint64_t rq;
	uint64_t alive = 0;
	uint64_t action;
	int64_t max_processes = 5;
	int debug_mode		  = 1; // Modo debugging desactivado por defecto

	if (argc > 1) {
		return -1;
	}

	// if ((max_processes = satoi(argv[1])) <= 0) {
	// 	return -1;
	// }

	p_rq p_rqs[max_processes];

	printStr("\nEntering loop\n");
	while (1) {
		for (rq = 0; rq < max_processes; rq++) {
			char *args[]  = {"endless_loop", NULL};
			p_rqs[rq].pid = create_process((MainFunction) endless_loop, args,
										   "endless_loop", 0);

			if (p_rqs[rq].pid == -1) {
				printStr("test_processes: ERROR creating process\n");
				return -1;
			}
			else {
				if (debug_mode) {
					printStr("\nCreating Processes...\n");
					printStr("PID: ");
					printInteger(p_rqs[rq].pid);
				}
				p_rqs[rq].state = RUNNING;
				alive++;
			}
		}

		// Manejo de procesos
		while (alive > 0) {
			for (rq = 0; rq < max_processes; rq++) {
				action = GetUniform(100) % 2;
				// action = 0;

				switch (action) {
					case 0:
						if (p_rqs[rq].state == RUNNING ||
							p_rqs[rq].state == BLOCKED) {
							if (my_kill(p_rqs[rq].pid) == -1) {
								printStr(
									"test_processes: ERROR killing process\n");
								return -1;
							}
							if (debug_mode) {
								printStr("\nKilling Processes...\n");
								printStr("PID: ");
								printInteger(p_rqs[rq].pid);
							}
							p_rqs[rq].state = KILLED;
							wait_pid(p_rqs[rq].pid);
							alive--;
						}
						break;

					case 1:
						if (p_rqs[rq].state == RUNNING) {
							if (my_block(p_rqs[rq].pid) == -1) {
								printStr(
									"test_processes: ERROR blocking process\n");
								return -1;
							}
							if (debug_mode) {
								printStr("\nBlocking Processes...\n");
								printStr("PID: ");
								printInteger(p_rqs[rq].pid);
							}
							p_rqs[rq].state = BLOCKED;
						}
						break;
				}
			}

			// Desbloquear procesos aleatoriamente
			for (rq = 0; rq < max_processes; rq++) {
				if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
					if (my_unblock(p_rqs[rq].pid) ==
						-1) { // aca se corta aparentemente
						printStr("test_processes: ERROR unblocking process\n");
						return -1;
					}
					if (debug_mode) {
						printStr("\nUnblocking Processes...\n");
						printStr("PID: ");
						printInteger(p_rqs[rq].pid);
					}
					p_rqs[rq].state = RUNNING;
				}
			}
		}
		clear();
	}
}

// int64_t test_processes(uint64_t argc, char *argv[]) {
// 	printStr("\n=== Simple test_processes start ===\n");
// 	int32_t pids[3];
// 	char *args[] = {"endless_loop", NULL};

// 	int i = 0;

// 	while (i < 20) {
// 		i++;
// 		// 1) Create 3 processes
// 		for (int i = 0; i < 3; i++) {
// 			pids[i] = create_process((MainFunction) endless_loop, args,
// 									 "endless_loop", 0);
// 			if (pids[i] == -1) {
// 				printStr("Error creating process\n");
// 				return -1;
// 			}
// 			printStr("Created PID: ");
// 			printInteger(pids[i]);
// 			printStr("\n");
// 		}

// 		// 2) Block them
// 		for (int i = 0; i < 3; i++) {
// 			if (my_block(pids[i]) == -1) {
// 				printStr("Error blocking PID: ");
// 				printInteger(pids[i]);
// 				printStr("\n");
// 				return -1;
// 			}
// 			printStr("Blocked PID: ");
// 			printInteger(pids[i]);
// 			printStr("\n");
// 		}

// 		// 3) Unblock them
// 		for (int i = 0; i < 3; i++) {
// 			if (my_unblock(pids[i]) == -1) {
// 				printStr("Error unblocking PID: ");
// 				printInteger(pids[i]);
// 				printStr("\n");
// 				return -1;
// 			}
// 			printStr("Unblocked PID: ");
// 			printInteger(pids[i]);
// 			printStr("\n");
// 		}

// 		// 4) Kill them
// 		for (int i = 0; i < 3; i++) {
// 			if (my_kill(pids[i]) == -1) {
// 				printStr("Error killing PID: ");
// 				printInteger(pids[i]);
// 				printStr("\n");
// 				return -1;
// 			}
// 			wait_pid(pids[i]);
// 			printStr("Killed PID: ");
// 			printInteger(pids[i]);
// 			printStr("\n");
// 		}

// 		printStr("=== Simple test_processes end ===\n");
// 		clear();
// 	}
// 	return 0;
// }