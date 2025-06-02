#include <stdio.h>
#include <stdlib.h>
#include <sys_calls.h>
#include <userLibrary.h>
#include <phylo.h>

static int numPhilosophers = 5;
static State states[MAX_PHILOSOPHERS];
static int semaphores[MAX_PHILOSOPHERS];

static char *intToChar(int n) {
	static char buffer[2];
	if (n < 0 || n > 9) {
		buffer[0] = '?'; // Error case
		buffer[1] = '\0';
		return buffer;
	}
	buffer[0] = '0' + n; // Convert int to char
	buffer[1] = '\0';
	return buffer;
}

void initPhilosophers() {
	for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
		states[i]	  = THINKING;
		semaphores[i] = user_sem_open(i, 1);
		if (semaphores[i] == -1) {
			printStr("Error initializing semaphore\n");
			return;
		}

		char *idStr = intToChar(i);

		char *args[] = {"philo", idStr, NULL};
		int pid =
			create_process((MainFunction) philosopher, args, "philosopher", 0);
		if (pid == -1) {
			printStr("Error creating philosopher process\n");
			return;
		}
	}
}

void printTable() {
	for (int i = 0; i < numPhilosophers; i++) {
		if (states[i] == EATING) {
			printChar('E');
		}
		else {
			printChar('.');
		}
		printChar(' ');
	}
	printChar('\n');
}

void test(int philosopher) {
	if (states[philosopher] == HUNGRY &&
		states[(philosopher + numPhilosophers - 1) % numPhilosophers] !=
			EATING &&
		states[(philosopher + 1) % numPhilosophers] != EATING) {
		states[philosopher] = EATING;
		user_sem_post(semaphores[philosopher]);
	}
}

void takeForks(int philosopher) {
	states[philosopher] = HUNGRY;
	test(philosopher);
	user_sem_wait(semaphores[philosopher]);
}

void putForks(int philosopher) {
	states[philosopher] = THINKING;
	test((philosopher + numPhilosophers - 1) % numPhilosophers);
	test((philosopher + 1) % numPhilosophers);
}

int philosopher(int argc, char **argv) {
	int id = atoi(argv[1]); // Obtener el ID del filósofo desde los argumentos
	while (1) {
		sleep_miliseconds(1000); // Thinking
		takeForks(id);
		sleep_miliseconds(1000); // Eating
		putForks(id);
	}
	return 0;
}

void adjustPhilosophers(char input) {
	if (input == 'a' && numPhilosophers < MAX_PHILOSOPHERS) {
		numPhilosophers++;
		states[numPhilosophers - 1]		= THINKING;
		semaphores[numPhilosophers - 1] = user_sem_open(numPhilosophers - 1, 1);
		if (semaphores[numPhilosophers - 1] == -1) {
			printStr("Error initializing semaphore for new philosopher\n");
			numPhilosophers--; // Revertir el cambio si falla
		}
	}
	else if (input == 'r' && numPhilosophers > MIN_PHILOSOPHERS) {
		numPhilosophers--;
		user_sem_close(semaphores[numPhilosophers]); // Cerrar el semáforo del
													 // filósofo eliminado
	}
}

void phylo() {
	initPhilosophers();
	printStr("Press 'a' to add a philosopher, 'r' to remove one.\n");
	clear();

	while (1) {
		char input = getChar();
		if (input == 'a' || input == 'r') {
			adjustPhilosophers(input);
		}
		printTable();
		sleep_miliseconds(500);
	}
}