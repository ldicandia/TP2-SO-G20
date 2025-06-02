#include <stdio.h>
#include <stdlib.h>
#include <sys_calls.h>
#include <userLibrary.h>
#include <phylo.h>

static int numPhilosophers = 5;
static State states[MAX_PHILOSOPHERS];
static int semaphores[MAX_PHILOSOPHERS];

void initPhilosophers() {
	for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
		states[i]	  = THINKING;
		semaphores[i] = user_sem_open(i, 1);
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

void philosopher(int philosopher) {
	while (1) {
		sleep_miliseconds(1000); // Thinking
		takeForks(philosopher);
		sleep_miliseconds(1000); // Eating
		putForks(philosopher);
	}
}

void adjustPhilosophers(char input) {
	if (input == 'a' && numPhilosophers < MAX_PHILOSOPHERS) {
		numPhilosophers++;
	}
	else if (input == 'r' && numPhilosophers > MIN_PHILOSOPHERS) {
		numPhilosophers--;
	}
}

void phylo() {
	initPhilosophers();
	// clear();
	printStr("Press 'a' to add a philosopher, 'r' to remove one.\n");

	while (1) {
		char input = getChar();
		if (input == 'a' || input == 'r') {
			adjustPhilosophers(input);
		}
		printTable();
		sleep_miliseconds(500);
	}
}