// // This is a personal academic project. Dear PVS-Studio, please check it.
// // PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <string.h>
// #include <signal.h>
// #include <time.h>

// #define MUTEX 10
// #define MAX_PHILOSOPHERS 32
// #define MIN_PHILOSOPHERS 3

// typedef enum { THINKING, HUNGRY, EATING } State;

// static int numPhilosophers = 5;
// static State states[MAX_PHILOSOPHERS];
// static sem_t mutex;
// static sem_t semaphores[MAX_PHILOSOPHERS];
// static pthread_t threads[MAX_PHILOSOPHERS];
// static int running = 1;

// #define LEFT(i) (((i) + numPhilosophers - 1) % numPhilosophers)
// #define RIGHT(i) (((i) + 1) % numPhilosophers)

// void *philosopher(void *arg);

// static char *intToChar(int i) {
// 	static char buffer[12];
// 	int j = 0;

// 	if (i < 0) {
// 		buffer[j++] = '-';
// 		i			= -i;
// 	}

// 	if (i == 0) {
// 		buffer[j++] = '0';
// 	}

// 	while (i > 0) {
// 		buffer[j++] = (i % 10) + '0';
// 		i /= 10;
// 	}

// 	buffer[j] = '\0';

// 	for (int k = 0; k < j / 2; k++) {
// 		char temp		  = buffer[k];
// 		buffer[k]		  = buffer[j - k - 1];
// 		buffer[j - k - 1] = temp;
// 	}

// 	return buffer;
// }

// void initPhilosophers() {
// 	sem_init(&mutex, 0, 1);

// 	for (int i = 0; i < numPhilosophers; i++) {
// 		states[i] = THINKING;
// 		if (sem_init(&semaphores[i], 0, 0) == -1) {
// 			printf("Error initializing semaphore %s\n",
// 				   intToChar(MUTEX + 1 + i));
// 			return;
// 		}
// 	}

// 	sem_wait(&mutex);
// 	for (int i = 0; i < numPhilosophers; i++) {
// 		int *id = malloc(sizeof(int));
// 		*id		= i;
// 		if (pthread_create(&threads[i], NULL, (void *) philosopher, id) != 0) {
// 			printf("Error creating philosopher process %s\n", intToChar(i));
// 			sem_post(&mutex);
// 			return;
// 		}
// 	}
// 	sem_post(&mutex);
// }

// void printTable() {
// 	for (int i = 0; i < numPhilosophers; i++) {
// 		if (states[i] == EATING) {
// 			printf("E ");
// 		}
// 		else {
// 			printf(". ");
// 		}
// 	}
// 	printf("\n");
// 	fflush(stdout);
// }

// void test(int philosopher) {
// 	if (states[philosopher] == HUNGRY && states[LEFT(philosopher)] != EATING &&
// 		states[RIGHT(philosopher)] != EATING) {
// 		states[philosopher] = EATING;
// 		sem_post(&semaphores[philosopher]);
// 	}
// }

// void takeForks(int philosopher) {
// 	sem_wait(&mutex);
// 	states[philosopher] = HUNGRY;
// 	test(philosopher);
// 	sem_post(&mutex);
// 	sem_wait(&semaphores[philosopher]);
// }

// void putForks(int philosopher) {
// 	sem_wait(&mutex);
// 	states[philosopher] = THINKING;
// 	test(LEFT(philosopher));
// 	test(RIGHT(philosopher));
// 	sem_post(&mutex);
// }

// void *philosopher(void *arg) {
// 	int id = *(int *) arg;
// 	free(arg);

// 	while (running) {
// 		usleep(20000); // sleep_miliseconds(20)
// 		takeForks(id);
// 		sem_wait(&mutex);
// 		printTable();
// 		sem_post(&mutex);
// 		usleep(30000); // sleep_miliseconds(30)
// 		putForks(id);
// 	}
// 	return NULL;
// }

// void adjustPhilosophers(char input) {
// 	sem_wait(&mutex);
// 	if (input == 'a' && numPhilosophers < MAX_PHILOSOPHERS) {
// 		numPhilosophers++;
// 		states[numPhilosophers - 1] = THINKING;
// 		if (sem_init(&semaphores[numPhilosophers - 1], 0, 0) == -1) {
// 			printf("Error initializing semaphore for new philosopher\n");
// 			numPhilosophers--;
// 		}
// 		else {
// 			int *id = malloc(sizeof(int));
// 			*id		= numPhilosophers - 1;
// 			if (pthread_create(&threads[numPhilosophers - 1], NULL,
// 							   (void *) philosopher, id) != 0) {
// 				printf("Error creating philosopher process\n");
// 				numPhilosophers--;
// 				free(id);
// 			}
// 		}
// 	}
// 	else if (input == 'r' && numPhilosophers > MIN_PHILOSOPHERS) {
// 		numPhilosophers--;
// 		printf("Removed philosopher %s (total: %s)\n",
// 			   intToChar(numPhilosophers), intToChar(numPhilosophers));
// 		pthread_cancel(threads[numPhilosophers]);
// 		sem_destroy(&semaphores[numPhilosophers]);
// 	}
// 	printTable();
// 	sem_post(&mutex);
// }

// void cleanup() {
// 	running = 0;

// 	// Wake up all waiting philosophers
// 	for (int i = 0; i < numPhilosophers; i++) {
// 		sem_post(&semaphores[i]);
// 	}

// 	// Wait for threads to finish
// 	for (int i = 0; i < numPhilosophers; i++) {
// 		pthread_join(threads[i], NULL);
// 		sem_destroy(&semaphores[i]);
// 	}

// 	sem_destroy(&mutex);
// }

// void signalHandler(int sig) {
// 	cleanup();
// 	exit(0);
// }

// void phylo() {
// 	srand(time(NULL));
// 	signal(SIGINT, signalHandler);
// 	signal(SIGTERM, signalHandler);

// 	initPhilosophers();
// 	printf("\033[2J\033[H"); // clear screen
// 	printf("Press 'a' to add a philosopher, 'r' to remove one.\n");

// 	char input;
// 	while (1) {
// 		input = getchar();
// 		if (input == 'a' || input == 'r') {
// 			adjustPhilosophers(input);
// 		}
// 		else if (input == 'q' || input == 'Q') {
// 			cleanup();
// 			break;
// 		}
// 	}
// }

// int main() {
// 	phylo();
// 	return 0;
// }