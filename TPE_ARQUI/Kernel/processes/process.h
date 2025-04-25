#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>
#define BUILT_IN_DESCRIPTORS 3 // stdin, stdout, stderr
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ 0
#define WRITE 1

typedef enum { BLOCKED = 0, READY, RUNNING, ZOMBIE, DEAD } ProcessStatus;

typedef struct Process {
	uint16_t pid;
	uint16_t parentPid;
	uint16_t waitingForPid; // espera especificamente a un hijo
	void *stackBase;		// base del stack
	void *stackPos;			// top del  stack
	char **argv;
	char *name; // nombre del proceso, para q sea mas facil de identificar q x
				// su pid
	uint8_t unkillable;
	uint8_t priority;
	ProcessStatus status;
	int16_t fileDescriptors[BUILT_IN_DESCRIPTORS];
	int32_t retValue;
	// creo q aca nos faltaria una lista de zombieChildren;
} Process;

void initProcess(Process *process, uint16_t pid, uint16_t parentPid,
				 char **args, char *name, uint8_t priority,
				 int16_t fileDescriptors[], uint8_t unkillable);

int processIsWaiting(Process *process, uint16_t pidToWait);

void freeProcess(Process *process);

void closeFileDescriptors(Process *process);

#endif