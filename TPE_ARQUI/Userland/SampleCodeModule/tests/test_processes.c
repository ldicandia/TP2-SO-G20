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

// aca funciona, en test_util no, probablemente sea por problema de args
int endless_loop1(int argc, char **argv) {
	while (1) {
		yield();
	}
}

// int create_process(void *code, char **args, char *name, uint8_t priority);

int test_processes(uint64_t argc, char *argv[]) {
	printStr("\nEntered Testing Processes...\n");
	uint64_t rq;
	uint64_t alive = 0;
	uint64_t action;
	int64_t max_processes = 5;
	int debug_mode		  = 0; // Modo debugging desactivado por defecto

	if (argc < 2) {
		printStr("ERROR: Not enough arguments, testProcesses <#processes>\n");
		return -1;
	}

	if ((max_processes = satoi(argv[1])) <= 0 || max_processes > 10) {
		printStr("ERROR: <#processes> must be between 1 and 10\n");
		return -1;
	}

	p_rq p_rqs[max_processes];

	printStr("\nEntering loop\n");
	while (1) {
		for (rq = 0; rq < max_processes; rq++) {
			char *args[]  = {"endless_loop", NULL};
			p_rqs[rq].pid = create_process((MainFunction) endless_loop1, args,
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
	}
}
