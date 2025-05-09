#include <stdio.h>
#include "syscall.h"
#include "test_util.h"

enum State { RUNNING, BLOCKED, KILLED };

typedef struct P_rq {
	int32_t pid;
	enum State state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
	printStr("\nTesting Processes...\n");
	uint64_t rq;
	uint64_t alive = 0;
	uint64_t action;
	int64_t max_processes;
	int debug_mode = 0; // Modo debugging desactivado por defecto

	if (argc < 1 || argc > 2) {
		return -1;
	}

	if ((max_processes = satoi(argv[0])) <= 0) {
		return -1;
	}

	if (argc == 2 && satoi(argv[1]) == 1) {
		debug_mode = 1; // Activar modo debugging si el segundo argumento es 1
	}

	p_rq p_rqs[max_processes];

	while (1) {
		// Crear procesos
		for (rq = 0; rq < max_processes; rq++) {
			char *args[] = {"endless_loop", NULL};
			p_rqs[rq].pid =
				create_process(&endless_loop, args, "endless_loop", 0);

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
					if (my_unblock(p_rqs[rq].pid) == -1) {
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