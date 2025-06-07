// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <sys_calls.h>
#include <userLibrary.h>
#include <phylo.h>

#define MUTEX 10
#define MAX_PHILOSOPHERS 32
#define MIN_PHILOSOPHERS 3

static int numPhilosophers = 5;
static State states[MAX_PHILOSOPHERS];
static int semaphores[MAX_PHILOSOPHERS];

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

void initPhilosophers() {
	user_sem_open(MUTEX, 1);

	for (int i = 0; i < numPhilosophers; i++) {
		states[i]	  = THINKING;
		int semID	  = MUTEX + 1 + i;
		semaphores[i] = semID;
		if (user_sem_open(semID, 0) == -1) {
			printStr("Error initializing semaphore ");
			printStr(intToChar(semID));
			printStr("\n");
			return;
		}
	}

	user_sem_wait(MUTEX);
	for (int i = 0; i < numPhilosophers; i++) {
		char *idStr	 = intToChar(i);
		char *args[] = {"philo", idStr, NULL};
		int pid =
			create_process((MainFunction) philosopher, args, "philosopher", 0);
		if (pid == -1) {
			printStr("Error creating philosopher process ");
			printStr(idStr);
			printStr("\n");
			user_sem_post(MUTEX);
			return;
		}
	}
	user_sem_post(MUTEX);
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
		// printTable();
	}
}

void takeForks(int philosopher) {
	user_sem_wait(MUTEX);
	states[philosopher] = HUNGRY;
	test(philosopher);
	user_sem_post(MUTEX);
	user_sem_wait(semaphores[philosopher]);
}

void putForks(int philosopher) {
	user_sem_wait(MUTEX);
	states[philosopher] = THINKING;
	// printTable();
	test((philosopher + numPhilosophers - 1) % numPhilosophers);
	test((philosopher + 1) % numPhilosophers);
	user_sem_post(MUTEX);
}

int philosopher(int argc, char **argv) {
	int id = atoi(argv[1]);
	while (1) {
		sleep_miliseconds(20);
		takeForks(id);
		user_sem_wait(MUTEX);
		printTable();
		user_sem_post(MUTEX);
		sleep_miliseconds(30);
		putForks(id);
	}
}

void adjustPhilosophers(char input) {
	user_sem_wait(MUTEX);
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
										  "philosopher", 0);
			if (pid == -1) {
				printStr("Error creating philosopher process\n");
			}
		}
	}
	printTable();
	user_sem_post(MUTEX);
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
	}
}