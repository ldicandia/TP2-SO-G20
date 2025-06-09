// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <lib.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <schedule.h>
#include <stdlib.h>
#include <videoDriver.h>
#include <shared.h>
#include <globals.h>

#define AGING_THRESHOLD 10

typedef struct SchedulerCDT {
	Node *processTable[MAX_PROCESSES];
	uint16_t processCount;
	uint16_t nextAvailablePid;
	LinkedListADT readyLevels[QTY_READY_LEVELS];
	LinkedListADT blockedProcesses;
	uint16_t currentPid;
	uint8_t currentLevel;
	int8_t remainingQuantum;
	int8_t shouldKillForegroundProcess;
} SchedulerCDT;

extern void forceTimerTick();

// ================================
// SCHEDULER INITIALIZATION
// ================================

SchedulerADT createScheduler() {
	SchedulerADT scheduler = (SchedulerADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processTable[i] = NULL;
	for (int i = 0; i < QTY_READY_LEVELS; i++)
		scheduler->readyLevels[i] = createLinkedListADT();
	scheduler->blockedProcesses			   = createLinkedListADT();
	scheduler->nextAvailablePid			   = 0;
	scheduler->shouldKillForegroundProcess = 0;
	scheduler->currentPid				   = 0;
	scheduler->currentLevel				   = 0;
	scheduler->processCount				   = 0;
	return scheduler;
}

SchedulerADT getSchedulerADT() {
	return (SchedulerADT) SCHEDULER_ADDRESS;
}

// ================================
// SCHEDULING CORE FUNCTIONS
// ================================

static uint16_t getNextPid(SchedulerADT scheduler) {
	ProcessADT process = NULL;
	for (int i = 0; i < QTY_READY_LEVELS; i++) {
		int currentLevelIndex =
			(scheduler->currentLevel + i) % QTY_READY_LEVELS;
		if (!isEmpty(scheduler->readyLevels[currentLevelIndex])) {
			Node *node = getFirst(scheduler->readyLevels[currentLevelIndex]);
			process	   = node->data;
			if (getLength(scheduler->readyLevels[currentLevelIndex]) > 1) {
				removeNode(scheduler->readyLevels[currentLevelIndex], node);
				appendNode(scheduler->readyLevels[currentLevelIndex], node);
			}

			scheduler->currentLevel =
				(currentLevelIndex + 1) % QTY_READY_LEVELS;

			return getProcessId(process);
		}
	}
	return IDLE_PID;
}

static int firstTime = 1;

void *schedule(void *prevStackPointer) {
	SchedulerADT scheduler = getSchedulerADT();
	scheduler->remainingQuantum--;

	if (scheduler->remainingQuantum > 0) {
		return prevStackPointer;
	}

	ProcessADT currentProcess;
	Node *currentProcessNode = scheduler->processTable[scheduler->currentPid];

	if (currentProcessNode != NULL) {
		currentProcess = (ProcessADT) currentProcessNode->data;
		if (!firstTime)
			setProcessStackPosition(currentProcess, prevStackPointer);
		else
			firstTime = 0;

		if (getProcessStatus(currentProcess) == RUNNING)
			setProcessStatus(currentProcess, READY);
	}

	scheduler->currentPid = getNextPid(scheduler);
	currentProcess = scheduler->processTable[scheduler->currentPid]->data;

	if (scheduler->shouldKillForegroundProcess &&
		getProcessFileDescriptor(currentProcess, STDIN) == STDIN) {
		scheduler->shouldKillForegroundProcess = 0;
		if (killCurrentProcess(-1) != -1)
			forceTimerTick();
	}

	scheduler->remainingQuantum = getProcessPriority(currentProcess) + 1;

	setProcessStatus(currentProcess, RUNNING);
	return getProcessStackPosition(currentProcess);
}

void yield() {
	SchedulerADT scheduler		= getSchedulerADT();
	scheduler->remainingQuantum = 0;
	forceTimerTick();
}

// ================================
// PROCESS CREATION AND DESTRUCTION
// ================================

uint16_t createProcess(MainFunction code, char **args, char *name,
					   uint8_t priority, int16_t fileDescriptors[],
					   uint8_t unkillable) {
	SchedulerADT scheduler = getSchedulerADT();
	if (scheduler->processCount >= MAX_PROCESSES) {
		driver_printStr("Error: Too many processes\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}

	ProcessADT process = (ProcessADT) allocMemory(sizeofProcess());
	if (process == NULL) {
		driver_printStr("Error: Memory allocation failed\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}
	initProcess(process, scheduler->nextAvailablePid, scheduler->currentPid,
				code, args, name, priority, fileDescriptors, unkillable);

	Node *processNode;
	if (getProcessId(process) != IDLE_PID) {
		processNode =
			appendElement(scheduler->readyLevels[getProcessPriority(process)],
						  (void *) process);
	}
	else {
		processNode		  = allocMemory(sizeof(Node));
		processNode->data = (void *) process;
	}
	scheduler->processTable[getProcessId(process)] = processNode;
	while (scheduler->processTable[scheduler->nextAvailablePid] != NULL)
		scheduler->nextAvailablePid =
			(scheduler->nextAvailablePid + 1) % MAX_PROCESSES;
	scheduler->processCount++;
	return getProcessId(process);
}

static void destroyZombie(SchedulerADT scheduler, ProcessADT zombie) {
	uint16_t zombiePid = getProcessId(zombie);
	Node *zombieNode   = scheduler->processTable[zombiePid];

	if (zombiePid == 1)
		return;

	scheduler->processCount--;
	scheduler->processTable[zombiePid] = NULL;

	if (scheduler->processCount == 0) {
		scheduler->nextAvailablePid = 2;
	}

	else if (zombiePid < scheduler->nextAvailablePid) {
		scheduler->nextAvailablePid = zombiePid;
	}

	freeProcess(zombie);
	freeMemory(zombieNode);
}

// ================================
// PROCESS KILLING FUNCTIONS
// ================================

int32_t killCurrentProcess(int32_t retValue) {
	SchedulerADT scheduler = getSchedulerADT();
	return killProcess(scheduler->currentPid, retValue);
}

int32_t killProcess(uint16_t pid, int32_t retValue) {
	SchedulerADT scheduler	= getSchedulerADT();
	Node *processToKillNode = scheduler->processTable[pid];
	if (processToKillNode == NULL)
		return -1;

	ProcessADT processToKill = (ProcessADT) processToKillNode->data;
	if (getProcessStatus(processToKill) == ZOMBIE ||
		isUnkillable(processToKill))
		return -1;

	closeFileDescriptors(processToKill);

	if (getProcessStatus(processToKill) == BLOCKED) {
		removeNode(scheduler->blockedProcesses, processToKillNode);
	}
	else {
		removeNode(scheduler->readyLevels[getProcessPriority(processToKill)],
				   processToKillNode);
	}

	setProcessReturnValue(processToKill, retValue);
	setProcessStatus(processToKill, ZOMBIE);

	begin(getZombieChildren(processToKill));
	while (hasNext(getZombieChildren(processToKill))) {
		destroyZombie(scheduler,
					  (ProcessADT) next(getZombieChildren(processToKill)));
	}

	Node *parentNode = scheduler->processTable[getParentPid(processToKill)];
	if (parentNode != NULL &&
		getProcessStatus((ProcessADT) parentNode->data) != ZOMBIE) {
		ProcessADT parent = (ProcessADT) parentNode->data;
		appendNode(getZombieChildren(parent), processToKillNode);
		if (isProcessWaitingFor(parent, getProcessId(processToKill))) {
			setStatus(getParentPid(processToKill), READY);
		}
	}
	else {
		destroyZombie(scheduler, processToKill);
	}
	if (pid == scheduler->currentPid)
		yield();
	return 0;
}

void killForegroundProcess() {
	SchedulerADT scheduler				   = getSchedulerADT();
	scheduler->shouldKillForegroundProcess = 1;
	forceTimerTick();
}

// ================================
// PROCESS BLOCKING AND UNBLOCKING
// ================================

int32_t blockProcess(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();

	Node *node = scheduler->processTable[pid];

	if (node == NULL)
		return -1;

	ProcessADT process	 = node->data;
	ProcessStatus status = getProcessStatus(process);

	if (status == BLOCKED || status == ZOMBIE)
		return 0;

	setProcessStatus(process, BLOCKED);
	removeNode(scheduler->readyLevels[getProcessPriority(process)], node);
	appendNode(scheduler->blockedProcesses, node);
	return 0;
}

int32_t unblockProcess(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node			   = scheduler->processTable[pid];
	if (!node)
		return -1;

	ProcessADT process	 = node->data;
	ProcessStatus status = getProcessStatus(process);
	if (status != BLOCKED)
		return status == ZOMBIE ? -1 : 0;

	removeNode(scheduler->blockedProcesses, node);
	freeMemory(node);

	setProcessStatus(process, READY);
	Node *newNode = appendElement(
		scheduler->readyLevels[getProcessPriority(process)], (void *) process);
	scheduler->processTable[pid] = newNode;
	scheduler->remainingQuantum	 = 0;
	return 0;
}

// ================================
// PROCESS STATUS AND PRIORITY MANAGEMENT
// ================================

int checkPriority(uint8_t priority) {
	return (priority >= QTY_READY_LEVELS);
}

int32_t setPriority(uint16_t pid, uint8_t newPriority) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node			   = scheduler->processTable[pid];

	if (node == NULL || pid == IDLE_PID)
		return -1;

	ProcessADT process = node->data;
	if (checkPriority(newPriority)) {
		return -1;
	}

	if (getProcessStatus(process) == READY ||
		getProcessStatus(process) == RUNNING) {
		removeNode(scheduler->readyLevels[getProcessPriority(process)], node);
		scheduler->processTable[getProcessId(process)] =
			appendNode(scheduler->readyLevels[newPriority], node);
	}
	setProcessPriority(process, newPriority);
	return newPriority;
}

int8_t setStatus(uint16_t pid, uint8_t newStatus) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node			   = scheduler->processTable[pid];
	if (node == NULL || pid == IDLE_PID)
		return -1;

	ProcessADT process		= node->data;
	ProcessStatus oldStatus = getProcessStatus(process);

	if (newStatus == RUNNING || newStatus == ZOMBIE || oldStatus == ZOMBIE)
		return -1;

	if (newStatus == getProcessStatus(process))
		return newStatus;

	setProcessStatus(process, newStatus);
	if (newStatus == BLOCKED) {
		removeNode(scheduler->readyLevels[getProcessPriority(process)], node);
		appendNode(scheduler->blockedProcesses, node);
	}
	else if (oldStatus == BLOCKED) {
		removeNode(scheduler->blockedProcesses, node);
		setProcessPriority(process, MAX_PRIORITY);
		prependNode(scheduler->readyLevels[getProcessPriority(process)], node);
		scheduler->remainingQuantum = 0;
	}
	return newStatus;
}

// ================================
// ZOMBIE PROCESS HANDLING
// ================================

int32_t getZombieRetValue(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();

	if (pid >= MAX_PROCESSES) {
		driver_printStr("Error: Invalid PID\n", (Color) {0xFF, 0x00, 0x00});
		return -1;
	}

	Node *zombieNode = scheduler->processTable[pid];
	if (zombieNode == NULL)
		return -1;
	ProcessADT zombieProcess = (ProcessADT) zombieNode->data;
	if (getParentPid(zombieProcess) != scheduler->currentPid)
		return -1;

	ProcessADT parent =
		(ProcessADT) scheduler->processTable[scheduler->currentPid]->data;
	setWaitingForPid(parent, pid);

	if (getProcessStatus(zombieProcess) != ZOMBIE) {
		setStatus(getProcessId(parent), BLOCKED);
		yield();
	}
	removeNode(getZombieChildren(parent), zombieNode);
	destroyZombie(scheduler, zombieProcess);
	return getProcessReturnValue(zombieProcess);
}

// ================================
// PROCESS INFORMATION AND GETTERS
// ================================

ProcessInfoList *getProcessInfoList() {
	SchedulerADT scheduler			 = getSchedulerADT();
	ProcessInfoList *processInfoList = allocMemory(sizeof(ProcessInfoList));
	if (processInfoList == NULL) {
		return NULL;
	}

	ProcessInfo *processArray =
		allocMemory(scheduler->processCount * sizeof(ProcessInfo));
	if (processArray == NULL) {
		freeMemory(processInfoList);
		return NULL;
	}
	int processIndex = 0;
	loadInfo(&processArray[processIndex++],
			 (ProcessADT) scheduler->processTable[IDLE_PID]->data);
	for (int levelIndex = QTY_READY_LEVELS - 1; levelIndex >= 0; levelIndex--) {
		begin(scheduler->readyLevels[levelIndex]);
		while (hasNext(scheduler->readyLevels[levelIndex])) {
			ProcessADT nextProcess =
				(ProcessADT) next(scheduler->readyLevels[levelIndex]);
			loadInfo(&processArray[processIndex], nextProcess);
			processIndex++;
			if (getProcessStatus(nextProcess) != ZOMBIE) {
				collectZombieProcessInfo(processIndex, processArray,
										 nextProcess);
				processIndex += getLength(getZombieChildren(nextProcess));
			}
		}
	}

	processInfoList->length		  = scheduler->processCount;
	processInfoList->snapshotList = processArray;
	return processInfoList;
}

uint16_t getPid() {
	SchedulerADT scheduler = getSchedulerADT();
	return scheduler->currentPid;
}

int getQtyProcesses(SchedulerADT scheduler) {
	return scheduler->processCount;
}

int getNextUnusedPid(SchedulerADT scheduler) {
	return scheduler->nextAvailablePid;
}

int32_t processIsAlive(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *processNode	   = scheduler->processTable[pid];
	return processNode != NULL &&
		   getProcessStatus((ProcessADT) processNode->data) != ZOMBIE;
}

ProcessADT getCurrentProcess() {
	SchedulerADT scheduler = getSchedulerADT();
	Node *currentNode	   = scheduler->processTable[scheduler->currentPid];
	if (currentNode == NULL)
		return NULL;
	return (ProcessADT) currentNode->data;
}

int16_t getCurrentProcessFileDescriptor(uint8_t fdIndex) {
	SchedulerADT scheduler = getSchedulerADT();
	ProcessADT process = scheduler->processTable[scheduler->currentPid]->data;
	return getProcessFileDescriptor(process, fdIndex);
}

// ================================
// FILE DESCRIPTOR MANAGEMENT
// ================================

int8_t changeFD(uint16_t pid, uint8_t position, int16_t newFd) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *processNode	   = scheduler->processTable[pid];
	if (pid == IDLE_PID || processNode == NULL)
		return -1;
	ProcessADT process = (ProcessADT) processNode->data;
	setFileDescriptor(process, position, newFd);
	return 0;
}

// ================================
// DEBUG AND PRINTING FUNCTIONS
// ================================

static char *statusToString(ProcessStatus s) {
	switch (s) {
		case READY:
			return "READY";
		case RUNNING:
			return "RUNNING";
		case BLOCKED:
			return "BLOCKED";
		case ZOMBIE:
			return "ZOMBIE";
		default:
			return "UNKNOWN";
	}
}

void printAllProcesses(SchedulerADT scheduler) {
	Color whiteColor = (Color) {0xFF, 0xFF, 0xFF};
	driver_printStr("\n[Processes]\n", whiteColor);
	driver_printStr("PID   Name      Status   Prio\n", whiteColor);
	driver_printStr("---   ----      ------   ----\n", whiteColor);

	for (int i = 0; i < MAX_PROCESSES; i++) {
		Node *processNode = scheduler->processTable[i];
		if (!processNode)
			continue;
		ProcessADT currentProcess = processNode->data;
		if (!currentProcess)
			continue;

		// PID
		driver_printNum(getProcessId(currentProcess), whiteColor);
		driver_printStr("     ", whiteColor);

		// Nombre (asumimos <=8 chars; si no, ajusta el tabulado)
		driver_printStr(getName(currentProcess), whiteColor);
		driver_printStr("        ", whiteColor);

		// Estado
		driver_printStr(statusToString(getProcessStatus(currentProcess)),
						whiteColor);
		driver_printStr("   ", whiteColor);

		// Prioridad
		driver_printNum(getProcessPriority(currentProcess), whiteColor);
		driver_printStr("\n", whiteColor);
	}
}

void printLevels(SchedulerADT scheduler) {
	Color cyanColor = (Color) {0xAA, 0xFF, 0xFF};
	driver_printStr("\n[Levels]: ", cyanColor);
	for (int i = 0; i < QTY_READY_LEVELS; i++) {
		driver_printStr("\nLevel ", cyanColor);
		driver_printNum(i, cyanColor);
		driver_printStr(": ", cyanColor);
		driver_printNum(getLength(scheduler->readyLevels[i]), cyanColor);
		driver_printStr(" Processes: ", cyanColor);
		Node *currentNode = getFirst(scheduler->readyLevels[i]);
		while (currentNode != NULL) {
			ProcessADT process = currentNode->data;
			driver_printStr(getName(process), cyanColor);
			currentNode = currentNode->next;
			if (currentNode != NULL)
				driver_printStr(", ", cyanColor);
		}
		driver_printStr("\n", cyanColor);
	}
}

void printCurrentProcess(SchedulerADT scheduler) {
	Color cyanColor = (Color) {0xAA, 0xFF, 0xFF};
	driver_printStr("\n[Scheduler Current Process]: ", cyanColor);
	driver_printStr("\nPID: ", cyanColor);
	driver_printNum(scheduler->currentPid, cyanColor);
	driver_printStr(" \nName: ", cyanColor);
	driver_printStr(
		getName(scheduler->processTable[scheduler->currentPid]->data),
		cyanColor);
}

void printBlockedProcesses() {
	SchedulerADT scheduler = getSchedulerADT();
	Color cyanColor		   = (Color) {0xAA, 0xFF, 0xFF};
	driver_printStr("\n[Blocked Processes]: ", cyanColor);
	Node *currentNode = getFirst(scheduler->blockedProcesses);
	while (currentNode != NULL) {
		ProcessADT process = currentNode->data;
		driver_printStr("\nPID: ", cyanColor);
		driver_printNum(getProcessId(process), cyanColor);
		currentNode = currentNode->next;
	}
}