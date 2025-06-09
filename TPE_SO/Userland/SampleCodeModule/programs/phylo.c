// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <stdio.h>
#include <stdlib.h>
#include <sys_calls.h>
#include <userLibrary.h>
#include <phylo.h>

#define DEV_NULL -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define MUTEX 10
#define MAX_PHILOSOPHERS 32
#define MIN_PHILOSOPHERS 3
#define MAX_CONSECUTIVE_MEALS 3
#define HUNGER_THRESHOLD 5

static int numPhilosophers		  = 5;
static int pids[MAX_PHILOSOPHERS] = {0};
static int indexPids			  = 0;
static State states[MAX_PHILOSOPHERS];
static int semaphores[MAX_PHILOSOPHERS];
static int consecutiveMeals[MAX_PHILOSOPHERS] = {0};
static int totalMeals[MAX_PHILOSOPHERS]		  = {0};
static int hungerTime[MAX_PHILOSOPHERS]		  = {0};

#define LEFT(i) (((i) + numPhilosophers - 1) % numPhilosophers)
#define RIGHT(i) (((i) + 1) % numPhilosophers)

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
		states[i]			= THINKING;
		consecutiveMeals[i] = 0;
		totalMeals[i]		= 0;
		hungerTime[i]		= 0;
		pids[i]				= 0;

		int semID	  = MUTEX + 1 + i;
		semaphores[i] = semID;
		if (user_sem_open(semID, 0) == -1) {
			printStr("Error initializing semaphore ");
			printStr(intToChar(semID));
			printStr("\n");
			return;
		}
	}

	for (int i = 0; i < numPhilosophers; i++) {
		char *idStr		= intToChar(i);
		char *args[]	= {"philo", idStr, NULL};
		pids[indexPids] = create_process_with_fds(
			(MainFunction) philosopher, args, "philosopher", 4,
			(int16_t[]) {DEV_NULL, STDOUT, STDERR});

		if (pids[indexPids++] == -1) {
			printStr("Error creating philosopher process ");
			printStr(idStr);
			printStr("\n");
			return;
		}
	}
}

void printTable() {
	printStr("State: ");
	for (int i = 0; i < numPhilosophers; i++) {
		if (states[i] == EATING) {
			printChar('E');
		}
		else if (states[i] == THINKING) {
			printChar('.');
		}
		else if (states[i] == HUNGRY) {
			printChar('.');
		}
		printChar(' ');
	}

	printStr(" | Meals: ");
	for (int i = 0; i < numPhilosophers; i++) {
		printStr(intToChar(totalMeals[i]));
		printChar(' ');
	}

	printStr(" | Hunger: ");
	for (int i = 0; i < numPhilosophers; i++) {
		printStr(intToChar(hungerTime[i]));
		printChar(' ');
	}

	printChar('\n');
}

void test(int philosopher) {
	if (states[philosopher] == HUNGRY && states[LEFT(philosopher)] != EATING &&
		states[RIGHT(philosopher)] != EATING &&
		(consecutiveMeals[philosopher] < MAX_CONSECUTIVE_MEALS ||
		 hungerTime[philosopher] > HUNGER_THRESHOLD)) {
		states[philosopher] = EATING;
		consecutiveMeals[philosopher]++;
		totalMeals[philosopher]++;
		hungerTime[philosopher] = 0;
		user_sem_post(semaphores[philosopher]);
	}
}

void takeForks(int philosopher) {
	user_sem_wait(MUTEX);
	states[philosopher]		= HUNGRY;
	hungerTime[philosopher] = 0;
	test(philosopher);
	user_sem_post(MUTEX);
	user_sem_wait(semaphores[philosopher]);
}

void putForks(int philosopher) {
	user_sem_wait(MUTEX);
	states[philosopher] = THINKING;

	int hungryNeighbors = (states[LEFT(philosopher)] == HUNGRY) +
						  (states[RIGHT(philosopher)] == HUNGRY);
	if (hungryNeighbors > 0) {
		consecutiveMeals[philosopher] = 0;
	}

	test(LEFT(philosopher));
	test(RIGHT(philosopher));
	user_sem_post(MUTEX);
}

int philosopher(int argc, char **argv) {
	if (argc < 2)
		return -1;

	int id = atoi(argv[1]);
	if (id < 0 || id >= numPhilosophers)
		return -1;

	while (1) {
		sleep_miliseconds(2000 + (id * 10) % 100);
		takeForks(id);

		user_sem_wait(MUTEX);
		printTable();
		user_sem_post(MUTEX);

		sleep_miliseconds(3000 + (id * 15) % 100);
		putForks(id);

		user_sem_wait(MUTEX);
		for (int i = 0; i < numPhilosophers; i++) {
			if (i != id && states[i] == HUNGRY) {
				hungerTime[i]++;
			}
		}
		user_sem_post(MUTEX);
	}
}

void adjustPhilosophers(char input) {
	user_sem_wait(MUTEX);

	if (input == 'a' && numPhilosophers < MAX_PHILOSOPHERS) {
		int newId				= numPhilosophers;
		states[newId]			= THINKING;
		consecutiveMeals[newId] = 0;
		totalMeals[newId]		= 0;
		hungerTime[newId]		= 0;

		int semID		  = MUTEX + 1 + newId;
		semaphores[newId] = semID;

		if (user_sem_open(semID, 0) != -1) {
			numPhilosophers++;

			char *idStr		= intToChar(newId);
			char *args[]	= {"philo", idStr, NULL};
			pids[indexPids] = create_process_with_fds(
				(MainFunction) philosopher, args, "philosopher", 4,
				(int16_t[]) {DEV_NULL, STDOUT, STDERR});

			if (pids[indexPids++] == -1) {
				printStr("Error creating philosopher process\n");
				numPhilosophers--;
				user_sem_close(semID);
			}
		}
		else {
			printStr("Error initializing semaphore for new philosopher\n");
		}
	}
	else if (input == 'r' && numPhilosophers > MIN_PHILOSOPHERS) {
		numPhilosophers--;
		user_sem_close(semaphores[numPhilosophers]);

		consecutiveMeals[numPhilosophers] = 0;
		totalMeals[numPhilosophers]		  = 0;
		hungerTime[numPhilosophers]		  = 0;

		if (numPhilosophers > 0) {
			int leftNeighbor  = LEFT(numPhilosophers);
			int rightNeighbor = RIGHT(numPhilosophers);
			test(leftNeighbor);
			if (rightNeighbor != leftNeighbor) {
				test(rightNeighbor);
			}
		}
		kill_process(pids[indexPids]);
		pids[indexPids--] = 0;

		printStr("Philosopher removed\n");
	}

	printTable();
	user_sem_post(MUTEX);
}

void phylo() {
	initPhilosophers();
	clear();
	printStr("Dining Philosophers Problem\n");
	printStr(
		"Press 'a' to add a philosopher, 'r' to remove one, 'q' to quit\n");
	printStr("E = Eating, . = Thinking, . = Hungry\n\n");

	while (1) {
		char input = getChar();
		if (input == 'a' || input == 'r') {
			adjustPhilosophers(input);
		}
		else if (input == 'q') {
			printStr("Exiting...\n");
			for (int i = 0; i < numPhilosophers; i++) {
				user_sem_close(semaphores[i]);
				if (pids[i] != 0) {
					kill_process(pids[i]);
				}
			}

			break;
		}
		yield();
	}
}