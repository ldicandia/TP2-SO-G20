// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <interrupts.h>
#include <lib.h>
#include <memoryManager.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE (1 << 12) // 4KB stack size

static char **allocArguments(char **args) {
	int argc = stringArrayLen(args), totalArgsLen = 0;
	int argsLen[argc];
	for (int i = 0; i < argc; i++) {
		argsLen[i] = strlen(args[i]) + 1;
		totalArgsLen += argsLen[i];
	}
	char **newArgsArray =
		(char **) malloc(totalArgsLen + sizeof(char **) * (argc + 1));
	char *charPosition = (char *) newArgsArray + (sizeof(char **) * (argc + 1));
	for (int i = 0; i < argc; i++) {
		newArgsArray[i] = charPosition;
		memcpy(charPosition, args[i], argsLen[i]);
		charPosition += argsLen[i];
	}
	newArgsArray[argc] = NULL;
	return newArgsArray;
}

static void assignFileDescriptor(Process *process, uint8_t fdIndex,
								 int16_t fdValue, uint8_t mode) {
	process->fileDescriptors[fdIndex] = fdValue;
	if (fdValue >= BUILT_IN_DESCRIPTORS)
		pipeOpenForPid(process->pid, fdValue, mode);
}

void initProcess(Process *process, uint16_t pid, uint16_t parentPid,
				 char **args, char *name, uint8_t priority,
				 int16_t fileDescriptors[], uint8_t unkillable) {
	process->pid		   = pid;
	process->parentPid	   = parentPid;
	process->waitingForPid = 0;
	process->stackBase	   = malloc(STACK_SIZE);
	if (process->stackBase == NULL) {
		process->status = DEAD;
		return;
	}
	process->argv = allocArguments(args);
	if (process->argv == NULL) {
		free(process->stackBase);
		process->status = DEAD;
		return;
	}
	strcpy(process->name, name);
	if (process->name != NULL) {
		strcpy(process->name, name);
	}
	else {
		free(process->argv);
		free(process->stackBase);
		process->status = DEAD;
		return;
	}

	process->priority = priority;
	void *stackEnd	  = (void *) ((uint64_t) process->stackBase + STACK_SIZE);

	process->stackPos = NULL; // PLACEHOLDER. FALTA IMPLEMENTAR!!!

	process->status = READY;

	process->unkillable = unkillable;
	process->retValue	= 0;

	assignFileDescriptor(process, STDIN, fileDescriptors[STDIN], READ);
	assignFileDescriptor(process, STDOUT, fileDescriptors[STDOUT], WRITE);
	assignFileDescriptor(process, STDERR, fileDescriptors[STDERR], WRITE);
}

void freeProcess(Process *process) {
	if (process == NULL)
		return;

	freeMemory(process->stackBase);
	freeMemory(process->name);
	freeMemory(process->argv);
	freeMemory(process);
}

int processIsWaiting(Process *process, uint16_t pidToWait) {
	return process->status == BLOCKED && process->waitingForPid == pidToWait;
}