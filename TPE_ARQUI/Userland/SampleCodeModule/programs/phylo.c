// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <sys_calls.h>
#include <userLibrary.h>
#include <phylo.h>

#define MUTEX 30
#define MAX_PHILOSOPHERS 32
#define MIN_PHILOSOPHERS 3

static int numPhilosophers = 5;
static State states[MAX_PHILOSOPHERS];
static int semaphores[MAX_PHILOSOPHERS];

static char *intToChar(int i) {
	static char buffer[12]; // Enough space for 32-bit integer
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

	// Reverse the string
	for (int k = 0; k < j / 2; k++) {
		char temp		  = buffer[k];
		buffer[k]		  = buffer[j - k - 1];
		buffer[j - k - 1] = temp;
	}

	return buffer;
}

static int mutex;

void initPhilosophers() {
	mutex = user_sem_open(MUTEX, 1);
	for (int i = 0; i < 5; i++) {
		states[i]	  = THINKING;
		semaphores[i] = user_sem_open(i + MUTEX, 0);
		if (semaphores[i] == -1) {
			printStr("Error initializing semaphore\n");
			return;
		}

		user_sem_wait(MUTEX);

		char *idStr = intToChar(i);

		char *args[] = {"philo", idStr, NULL};
		int pid =
			create_process((MainFunction) philosopher, args, "philosopher", 3);
		if (pid == -1) {
			printStr("Error creating philosopher process\n");
			return;
		}

		user_sem_post(MUTEX);
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
		printTable();
	}
}

void takeForks(int philosopher) {
	user_sem_wait(MUTEX); // lock
	states[philosopher] = HUNGRY;
	test(philosopher);
	user_sem_post(MUTEX); // unlock
	user_sem_wait(semaphores[philosopher]);
}

void putForks(int philosopher) {
	user_sem_wait(MUTEX); // lock
	states[philosopher] = THINKING;
	printTable();
	test((philosopher + numPhilosophers - 1) % numPhilosophers);
	test((philosopher + 1) % numPhilosophers);
	user_sem_post(MUTEX); // unlock
}

int philosopher(int argc, char **argv) {
	int id = atoi(argv[1]);
	while (1) {
		sleep_miliseconds(2000); // Pensar por un tiempo aleatorio
		takeForks(id);
		sleep_miliseconds(3000); // Comer por un tiempo aleatorio
		putForks(id);
	}
}

void adjustPhilosophers(char input) {
	user_sem_wait(MUTEX); // Lock critical section
	if (input == 'a' && numPhilosophers < MAX_PHILOSOPHERS) {
		numPhilosophers++;
		states[numPhilosophers - 1] = THINKING;
		semaphores[numPhilosophers - 1] =
			user_sem_open(numPhilosophers - 1 + MUTEX, 0);
		if (semaphores[numPhilosophers - 1] == -1) {
			printStr("Error initializing semaphore for new philosopher\n");
			numPhilosophers--;
		}
		else {
			char *idStr	 = intToChar(numPhilosophers - 1);
			char *args[] = {"philo", idStr, NULL};
			int pid		 = create_process((MainFunction) philosopher, args,
										  "philosopher", 3);
			if (pid == -1) {
				printStr("Error creating philosopher process\n");
			}
		}
	}
	printTable();
	user_sem_post(MUTEX); // Unlock critical section
}

void phylo() {
	initPhilosophers();
	clear();
	printStr("Press 'a' to add a philosopher, 'r' to remove one.\n");
	while (1) {
		char input = getChar();
		if (input == 'a' || input == 'r') {
			adjustPhilosophers(input);
		}
		// printTable();
		// sleep_miliseconds(100);
	}
}