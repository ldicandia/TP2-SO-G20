// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <interrupts.h>
#include <lib.h>
#include <memoryManager.h>
#include <process.h>
#include <stdlib.h>

#define STACK_SIZE (1 << 12) // 4KB stack size

typedef struct ProcessCDT {
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
} ProcessCDT;

static char **allocArguments(char **args) {
	int argc = stringArrayLen(args), totalArgsLen = 0;
	int argsLen[argc];
	for (int i = 0; i < argc; i++) {
		argsLen[i] = strlen(args[i]) + 1;
		totalArgsLen += argsLen[i];
	}
	char **newArgsArray =
		(char **) allocMemory(totalArgsLen + sizeof(char **) * (argc + 1));
	char *charPosition = (char *) newArgsArray + (sizeof(char **) * (argc + 1));
	for (int i = 0; i < argc; i++) {
		newArgsArray[i] = charPosition;
		memcpy(charPosition, args[i], argsLen[i]);
		charPosition += argsLen[i];
	}
	newArgsArray[argc] = NULL;
	return newArgsArray;
}

// static void assignFileDescriptor(ProcessADT process, uint8_t fdIndex,
// 								 int16_t fdValue, uint8_t mode) {
// 	process->fileDescriptors[fdIndex] = fdValue;
// 	if (fdValue >= BUILT_IN_DESCRIPTORS) {
// 		// pipeOpenForPid(process->pid, fdValue, mode);
// 	}
// }

void processWrapper(MainFunction code, char **args) {
	// int len = stringArrayLen(args);
	//  int retValue = code(len, args);
}

void initProcess(ProcessADT process, uint16_t pid, uint16_t parentPid,
				 int code(int argc, char **args), char **args, char *name,
				 uint8_t priority, int16_t fileDescriptors[],
				 uint8_t unkillable) {
	process->pid		   = pid;
	process->parentPid	   = parentPid;
	process->waitingForPid = 0;
	process->stackBase	   = allocMemory(STACK_SIZE);
	if (process->stackBase == NULL) {
		process->status = DEAD;
		return;
	}
	process->argv = allocArguments(args);
	if (process->argv == NULL) {
		freeMemory(process->stackBase);
		process->status = DEAD;
		return;
	}
	strcpy(process->name, name);
	if (process->name != NULL) {
		strcpy(process->name, name);
	}
	else {
		freeMemory(process->argv);
		freeMemory(process->stackBase);
		process->status = DEAD;
		return;
	}

	process->priority = priority;
	void *stackEnd	  = (void *) ((uint64_t) process->stackBase + STACK_SIZE);

	process->stackPos =
		_initialize_stack_frame(&processWrapper, code, stackEnd, args);

	process->status = READY;

	process->unkillable = unkillable;
	process->retValue	= 0;

	// assignFileDescriptor(process, STDIN, fileDescriptors[STDIN], READ);
	// assignFileDescriptor(process, STDOUT, fileDescriptors[STDOUT], WRITE);
	// assignFileDescriptor(process, STDERR, fileDescriptors[STDERR], WRITE);
}

void freeProcess(ProcessADT process) {
	if (process == NULL)
		return;

	freeMemory(process->stackBase);
	freeMemory(process->name);
	freeMemory(process->argv);
	freeMemory(process);
}

int processIsWaiting(ProcessADT process, uint16_t pidToWait) {
	return process->status == BLOCKED && process->waitingForPid == pidToWait;
}

uint16_t get_pid(ProcessADT process) {
	if (process == NULL)
		return 0; // Return 0 or an invalid PID if the process is NULL
	return process->pid;
}

void set_stackPos(ProcessADT process, void *stackPos) {
	if (process != NULL)
		process->stackPos = stackPos;
}

void set_status(ProcessADT process, ProcessStatus status) {
	if (process != NULL)
		process->status = status;
}

ProcessStatus get_status(ProcessADT process) {
	if (process == NULL)
		return DEAD; // Return DEAD or an appropriate default status if process
					 // is NULL
	return process->status;
}

uint8_t get_priority(ProcessADT process) {
	if (process == NULL)
		return 0; // Return 0 or a default priority if process is NULL
	return process->priority;
}

void set_priority(ProcessADT process, uint8_t priority) {
	if (process != NULL)
		process->priority = priority;
}

int16_t get_fileDescriptor(ProcessADT process, uint8_t fdIndex) {
	if (process == NULL || fdIndex >= BUILT_IN_DESCRIPTORS)
		return -1; // Return -1 for invalid process or index
	return process->fileDescriptors[fdIndex];
}

// make get_stackPos
void *get_stackPos(ProcessADT process) {
	if (process == NULL)
		return NULL; // Return NULL if the process is NULL
	return process->stackPos;
}

int32_t get_retValue(ProcessADT process) {
	if (process == NULL)
		return -1; // Return -1 or an appropriate default value if process is
				   // NULL
	return process->retValue;
}
void set_retValue(ProcessADT process, int32_t retValue) {
	if (process != NULL)
		process->retValue = retValue;
}
