// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <interrupts.h>
#include <lib.h>
#include <memoryManager.h>
#include <process.h>
#include <stdlib.h>
#include <videoDriver.h>
#include <schedule.h>
#include <linkedListADT.h>
#include <pipeManager.h>
#include <shared.h>
#include <globals.h>

typedef struct ProcessCDT {
	uint16_t processId;
	uint16_t parentProcessId;
	uint16_t targetWaitPid; // espera especificamente a un hijo
	void *stackBaseAddr;	// base del stack
	void *stackCurrentPos;	// top del  stack
	char **argumentsArray;
	char *processName; // nombre del proceso, para q sea mas facil de
					   // identificar q x su pid
	uint8_t cannotBeKilled;
	uint8_t processPriority;
	ProcessStatus currentStatus;
	int16_t descriptorTable[BUILT_IN_DESCRIPTORS];
	int32_t exitValue;
	int hasBeenInitialized;
	LinkedListADT deadChildrenList;
	uint16_t blockedTime;

	// creo q aca nos faltaria una lista de zombieChildren;
} ProcessCDT;

//=== UTILITY FUNCTIONS ===

static char **allocArguments(char **inputArgs) {
	int argumentCount = stringArrayLen(inputArgs), totalArgumentsLength = 0;
	int individualArgsLength[argumentCount];
	for (int i = 0; i < argumentCount; i++) {
		individualArgsLength[i] = strlen(inputArgs[i]) + 1;
		totalArgumentsLength += individualArgsLength[i];
	}
	char **newArgumentsArray = (char **) allocMemory(
		totalArgumentsLength + sizeof(char **) * (argumentCount + 1));
	char *stringPosition =
		(char *) newArgumentsArray + (sizeof(char **) * (argumentCount + 1));
	for (int i = 0; i < argumentCount; i++) {
		newArgumentsArray[i] = stringPosition;
		memcpy(stringPosition, inputArgs[i], individualArgsLength[i]);
		stringPosition += individualArgsLength[i];
	}
	newArgumentsArray[argumentCount] = NULL;
	return newArgumentsArray;
}

void processWrapper(MainFunction codeFunction, char **argumentsArray) {
	if (argumentsArray[0] == NULL) {
		driver_printStr("Error: args is NULL\n", (Color) {0xFF, 0x00, 0x00});
		killCurrentProcess(-1);
	}
	// driver_printStr(argumentsArray[0], (Color) {0xFF, 0x00, 0x00});
	int argumentCount	   = stringArrayLen(argumentsArray);
	int processReturnValue = codeFunction(argumentCount, argumentsArray);
	killCurrentProcess(processReturnValue);
}

int64_t sizeofProcess() {
	return sizeof(ProcessCDT);
}

//=== FILE DESCRIPTOR MANAGEMENT ===

static void assignFileDescriptor(ProcessADT processPtr, uint8_t descriptorIndex,
								 int16_t descriptorValue, uint8_t accessMode) {
	processPtr->descriptorTable[descriptorIndex] = descriptorValue;
	if (descriptorValue >= BUILT_IN_DESCRIPTORS)
		grantPipeAccessToProcess(processPtr->processId, descriptorValue,
								 accessMode);
}

static void closeFileDescriptor(uint16_t processId, int16_t descriptorValue) {
	if (descriptorValue >= BUILT_IN_DESCRIPTORS)
		revokePipeAccessFromProcess(processId, descriptorValue);
}

void closeFileDescriptors(ProcessADT processPtr) {
	closeFileDescriptor(processPtr->processId,
						processPtr->descriptorTable[STDIN]);
	closeFileDescriptor(processPtr->processId,
						processPtr->descriptorTable[STDOUT]);
	closeFileDescriptor(processPtr->processId,
						processPtr->descriptorTable[STDERR]);
}

int16_t getProcessFileDescriptor(ProcessADT processPtr,
								 uint8_t descriptorIndex) {
	if (processPtr == NULL || descriptorIndex >= BUILT_IN_DESCRIPTORS)
		return -1;
	return processPtr->descriptorTable[descriptorIndex];
}

void setFileDescriptor(ProcessADT processPtr, uint8_t descriptorIndex,
					   int16_t descriptorValue) {
	if (processPtr != NULL && descriptorIndex < BUILT_IN_DESCRIPTORS)
		processPtr->descriptorTable[descriptorIndex] = descriptorValue;
}

//=== PROCESS LIFECYCLE ===

void initProcess(ProcessADT processPtr, uint16_t processId,
				 uint16_t parentProcessId, MainFunction codeFunction,
				 char **argumentsArray, char *processName,
				 uint8_t processPriority, int16_t descriptorTable[],
				 uint8_t cannotBeKilled) {
	processPtr->processId		= processId;
	processPtr->parentProcessId = parentProcessId;
	processPtr->targetWaitPid	= 0;
	processPtr->blockedTime		= 0;
	processPtr->stackBaseAddr	= allocMemory(STACK_SIZE);
	processPtr->argumentsArray	= allocArguments(argumentsArray);
	processPtr->processName		= allocMemory(strlen(processName) + 1);
	strcpy(processPtr->processName, processName);
	processPtr->processPriority = processPriority;
	void *stackEndAddr =
		(void *) ((uint64_t) processPtr->stackBaseAddr + STACK_SIZE);
	processPtr->stackCurrentPos =
		_initialize_stack_frame(&processWrapper, codeFunction, stackEndAddr,
								(void *) processPtr->argumentsArray);
	processPtr->currentStatus	 = READY;
	processPtr->deadChildrenList = createLinkedListADT();
	processPtr->cannotBeKilled	 = cannotBeKilled;

	assignFileDescriptor(processPtr, STDIN, descriptorTable[STDIN], READ);
	assignFileDescriptor(processPtr, STDOUT, descriptorTable[STDOUT], WRITE);
	assignFileDescriptor(processPtr, STDERR, descriptorTable[STDERR], WRITE);
}

void freeProcess(ProcessADT processPtr) {
	if (processPtr == NULL)
		return;
	freeLinkedListADT(processPtr->deadChildrenList);
	freeMemory(processPtr->stackBaseAddr);
	freeMemory(processPtr->processName);
	freeMemory(processPtr->argumentsArray);
	freeMemory(processPtr);
}

//=== PROCESS ID ===

uint16_t getProcessId(ProcessADT processPtr) {
	if (processPtr == NULL)
		return 0;
	return processPtr->processId;
}

uint16_t getParentPid(ProcessADT processPtr) {
	if (processPtr == NULL)
		return 0;
	return processPtr->parentProcessId;
}

//=== PROCESS NAME ===

char *getName(ProcessADT processPtr) {
	if (processPtr == NULL)
		return NULL;
	return processPtr->processName;
}

//=== PROCESS STATUS ===

ProcessStatus getProcessStatus(ProcessADT processPtr) {
	if (processPtr == NULL)
		return DEAD;
	return processPtr->currentStatus;
}

void setProcessStatus(ProcessADT processPtr, ProcessStatus processStatus) {
	if (processPtr != NULL)
		processPtr->currentStatus = processStatus;
}

//=== PROCESS PRIORITY ===

uint8_t getProcessPriority(ProcessADT processPtr) {
	if (processPtr == NULL)
		return 0;
	return processPtr->processPriority;
}

void setProcessPriority(ProcessADT processPtr, uint8_t priorityLevel) {
	if (processPtr != NULL)
		processPtr->processPriority = priorityLevel;
}

//=== STACK MANAGEMENT ===

void *getProcessStackPosition(ProcessADT processPtr) {
	if (processPtr == NULL)
		return NULL;
	return processPtr->stackCurrentPos;
}

void setProcessStackPosition(ProcessADT processPtr, void *stackPosition) {
	if (processPtr != NULL)
		processPtr->stackCurrentPos = stackPosition;
}

void *getProcessStackBase(ProcessADT processPtr) {
	if (processPtr == NULL)
		return NULL;
	return processPtr->stackBaseAddr;
}

//=== PROCESS RETURN VALUE ===

int32_t getProcessReturnValue(ProcessADT processPtr) {
	if (processPtr == NULL)
		return -1;
	return processPtr->exitValue;
}

void setProcessReturnValue(ProcessADT processPtr, int32_t returnValue) {
	if (processPtr != NULL)
		processPtr->exitValue = returnValue;
}

//=== PROCESS INITIALIZATION ===

int getProcessInitializationStatus(ProcessADT processPtr) {
	return processPtr->hasBeenInitialized;
}

void setProcessInitialized(ProcessADT processPtr, int initializationFlag) {
	processPtr->hasBeenInitialized = initializationFlag;
}

//=== PROCESS PROPERTIES ===

uint8_t isUnkillable(ProcessADT processPtr) {
	if (processPtr == NULL)
		return 0;
	return processPtr->cannotBeKilled;
}

//=== WAITING MANAGEMENT ===

uint16_t getWaitingForPid(ProcessADT processPtr) {
	if (processPtr == NULL)
		return 0;
	return processPtr->targetWaitPid;
}

void setWaitingForPid(ProcessADT processPtr, uint16_t processId) {
	if (processPtr != NULL)
		processPtr->targetWaitPid = processId;
}

uint16_t getWaitingTime(ProcessADT processPtr) {
	if (processPtr == NULL)
		return 0;
	return processPtr->blockedTime;
}

void setWaitingTime(ProcessADT processPtr, uint16_t waitingTimeValue) {
	if (processPtr != NULL)
		processPtr->blockedTime = waitingTimeValue;
}

void incrementWaitingTime(ProcessADT processPtr) {
	if (processPtr != NULL)
		processPtr->blockedTime++;
}

int isProcessWaitingFor(ProcessADT processPtr, uint16_t pidToWaitFor) {
	return processPtr->targetWaitPid == pidToWaitFor &&
		   processPtr->currentStatus == BLOCKED;
}

//=== ZOMBIE CHILDREN MANAGEMENT ===

LinkedListADT getZombieChildren(ProcessADT processPtr) {
	if (processPtr == NULL)
		return NULL;
	return processPtr->deadChildrenList;
}

//=== PROCESS INFO ===

ProcessInfo *loadInfo(ProcessInfo *infoSnapshot, ProcessADT processPtr) {
	infoSnapshot->name = allocMemory(strlen(processPtr->processName) + 1);
	strcpy(infoSnapshot->name, processPtr->processName);
	infoSnapshot->pid		   = processPtr->processId;
	infoSnapshot->stackPointer = (uint64_t) processPtr->stackBaseAddr;
	infoSnapshot->priority	   = processPtr->processPriority;
	infoSnapshot->status	   = processPtr->currentStatus;
	infoSnapshot->foreground   = processPtr->descriptorTable[STDIN] == STDIN;
	return infoSnapshot;
}

int collectZombieProcessInfo(int currentProcessIndex,
							 ProcessInfo processStatusArray[],
							 ProcessADT nextProcessPtr) {
	LinkedListADT deadChildrenList = nextProcessPtr->deadChildrenList;
	begin(deadChildrenList);
	while (hasNext(deadChildrenList))
		loadInfo(&processStatusArray[currentProcessIndex++],
				 (ProcessADT) next(deadChildrenList));
	return currentProcessIndex;
}