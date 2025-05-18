// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <defs.h>
#include <lib.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <schedule.h>
#include <stdlib.h>
#include <videoDriver.h>

#define QTY_READY_LEVELS 5
#define MAX_PRIORITY 4
#define MIN_PRIORITY 0
#define BLOCKED_INDEX QTY_READY_LEVELS
#define MAX_PROCESSES (1 << 12)
#define IDLE_PID 0
#define QUANTUM_COEF 2

#define SCHEDULER_ADDRESS 0x60000

typedef struct SchedulerCDT {
	Node *processes[MAX_PROCESSES];
	LinkedListADT levels[QTY_READY_LEVELS];
	uint16_t currentPid;
	uint16_t nextUnusedPid;
	uint16_t qtyProcesses;
	int8_t remainingQuantum;
	int8_t killFgProcess;
} SchedulerCDT;

extern void forceTimerTick();
extern int checkKeyboardActivity();

SchedulerADT createScheduler() {
	SchedulerADT scheduler = (SchedulerADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < QTY_READY_LEVELS + 1; i++)
		scheduler->levels[i] = createLinkedListADT();
	scheduler->nextUnusedPid = 0;
	scheduler->killFgProcess = 0;
	// scheduler->currentPid	 = IDLE_PID;
	return scheduler;
}

SchedulerADT getSchedulerADT() {
	return (SchedulerADT) SCHEDULER_ADDRESS;
}

static uint16_t getNextPid(SchedulerADT scheduler) {
	ProcessADT process = NULL;
	for (int lvl = MAX_PRIORITY; lvl >= 0 && process == NULL; lvl--) {
		if (!isEmpty(scheduler->levels[lvl])) {
			process = (getFirst(scheduler->levels[lvl]))->data;
		}
	}
	return process == NULL ? IDLE_PID : get_pid(process);
}

static int strcmp(char *str1, char *str2) {
	while (*str1 && *str2 && *str1 == *str2) {
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

void printAllProcesses(SchedulerADT scheduler) {
	driver_printStr("\n[Processes]: ", (Color) {0xAA, 0xFF, 0xFF});
	for (int i = 0; i < MAX_PROCESSES; i++) {
		if (scheduler->processes[i] != NULL) {
			ProcessADT process = scheduler->processes[i]->data;
			if (process != NULL) {
				driver_printStr("\nPID: ", (Color) {0xAA, 0xFF, 0xFF});
				driver_printNum(get_pid(process), (Color) {0xAA, 0xFF, 0xFF});
				driver_printStr(" Name: ", (Color) {0xAA, 0xFF, 0xFF});
				driver_printStr(getName(process), (Color) {0xAA, 0xFF, 0xFF});
			}
		}
	}
}

void printLevels(SchedulerADT scheduler) {
	driver_printStr("\n[Levels]: ", (Color) {0xAA, 0xFF, 0xFF});
	for (int i = 0; i < QTY_READY_LEVELS; i++) {
		driver_printStr("\nLevel ", (Color) {0xAA, 0xFF, 0xFF});
		driver_printNum(i, (Color) {0xAA, 0xFF, 0xFF});
		driver_printStr(": ", (Color) {0xAA, 0xFF, 0xFF});
		driver_printNum(getLength(scheduler->levels[i]),
						(Color) {0xAA, 0xFF, 0xFF});
	}
}

void printCurrentProcess(SchedulerADT scheduler) {
	driver_printStr("\n[Scheduler Current Process]: ",
					(Color) {0xAA, 0xFF, 0xFF});
	driver_printStr("\nPID: ", (Color) {0xAA, 0xFF, 0xFF});
	driver_printNum(scheduler->currentPid, (Color) {0xAA, 0xFF, 0xFF});
}

void *schedule(void *prevStackPointer) {
	static int firstTime   = 1;
	SchedulerADT scheduler = getSchedulerADT();

	// printAllProcesses(scheduler);
	// printLevels(scheduler);
	// printCurrentProcess(scheduler);

	// if (get_pid(scheduler->processes[scheduler->currentPid]->data) !=
	// 		IDLE_PID &&
	// 	get_pid(scheduler->processes[scheduler->currentPid]->data) != 1) {
	// driver_printStr(getName(scheduler->processes[scheduler->currentPid]->data),
	// 				(Color) {0xAA, 0xFF, 0xFF});
	// driver_printStr("\n[Scheduler Quantum]: ", (Color) {0xAA, 0xFF, 0xFF});
	// driver_printNum(scheduler->remainingQuantum, (Color) {0xAA, 0xFF, 0xFF});
	// driver_printStr("\nPID: ", (Color) {0xAA, 0xFF, 0xFF});
	// driver_printNum(scheduler->currentPid, (Color) {0xAA, 0xFF, 0xFF});

	// driver_printStr("\n[Processes]: ", (Color) {0xAA, 0xFF, 0xFF});
	// driver_printNum(scheduler->qtyProcesses, (Color) {0xAA, 0xFF, 0xFF});
	// driver_printChar('\n', (Color) {0xAA, 0xFF, 0xFF});
	// }

	// if (checkKeyboardActivity()) {
	// 	for (int i = 0; i < MAX_PROCESSES; i++) {
	// 		if (scheduler->processes[i] != NULL) {
	// 			ProcessADT process = scheduler->processes[i]->data;
	// 			if (process != NULL && strcmp(getName(process), "shell") == 0) {
	// 				setPriority(get_pid(process), MAX_PRIORITY);
	// 				if (scheduler->currentPid == get_pid(process)) {
	// 					scheduler->remainingQuantum +=
	// 						5; // Dale un poco más de tiempo
	// 				}
	// 				break;
	// 			}
	// 		}
	// 	}
	// }

	scheduler->remainingQuantum--;

	if (!scheduler->qtyProcesses || scheduler->remainingQuantum > 0) {
		return prevStackPointer;
	}

	ProcessADT currentProcess;
	Node *currentProcessNode = scheduler->processes[scheduler->currentPid];

	// driver_printStr("\nnext PID: ", (Color) {0xAA, 0xFF, 0xFF});
	// driver_printNum(scheduler->currentPid, (Color) {0xAA, 0xFF, 0xFF});

	if (currentProcessNode != NULL) {
		currentProcess = (ProcessADT) currentProcessNode->data;
		if (!firstTime)
			set_stackPos(currentProcess, prevStackPointer);
		else
			firstTime = 0;

		if (get_status(currentProcess) == RUNNING)
			set_status(currentProcess, READY);

		// Decae la prioridad de los procesos que no sean la shell
		uint8_t newPriority;
		// if (strcmp(getName(currentProcess), "shell") != 0) {
		newPriority = get_priority(currentProcess) > 0 ?
						  get_priority(currentProcess) - 1 :
						  get_priority(currentProcess);
		setPriority(get_pid(currentProcess), newPriority);
		// }
	}

	scheduler->currentPid = getNextPid(scheduler);
	currentProcess		  = scheduler->processes[scheduler->currentPid]->data;

	if (scheduler->killFgProcess &&
		get_fileDescriptor(currentProcess, STDIN) == STDIN) {
		scheduler->killFgProcess = 0;
		if (killCurrentProcess(-1) != -1)
			forceTimerTick();
	}

	scheduler->remainingQuantum = (MAX_PRIORITY - get_priority(currentProcess));

	set_status(currentProcess, RUNNING);
	return get_stackPos(currentProcess);
}
char *numToString(int number) {
	int length = (number == 0) ? 1 : 0;
	int temp   = number;
	while (temp != 0) {
		temp /= 10;
		length++;
	}
	char *str = (char *) allocMemory(length + 1);
	if (str == NULL) {
		return NULL;
	}
	str[length] = '\0';
	for (int i = length - 1; i >= 0; i--) {
		str[i] = (number % 10) + '0';
		number /= 10;
	}
	return str;
}

uint16_t createProcess(MainFunction code, char **args, char *name,
					   uint8_t priority, int16_t fileDescriptors[],
					   uint8_t unkillable) {
	// driver_printStr("\n[Kenrel]: Creating Process... ",
	// 				(Color) {0xAA, 0xFF, 0xFF});
	// driver_printStr(name, (Color) {0xAA, 0xFF, 0xFF});

	SchedulerADT scheduler = getSchedulerADT();
	if (scheduler->qtyProcesses >= MAX_PROCESSES) {
		driver_printStr("Error: Too many processes\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}

	ProcessADT process = (ProcessADT) allocMemory(sizeof(sizeofProcess()));
	if (process == NULL) {
		driver_printStr("Error: Memory allocation failed\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}
	initProcess(process, scheduler->nextUnusedPid, scheduler->currentPid, code,
				args, name, priority, fileDescriptors, unkillable);

	Node *processNode;
	if (get_pid(process) != IDLE_PID) {
		processNode = appendElement(scheduler->levels[get_priority(process)],
									(void *) process); // seg fault aca
	}
	else {
		processNode		  = allocMemory(sizeof(Node));
		processNode->data = (void *) process;
	}
	scheduler->processes[get_pid(process)] = processNode;
	while (scheduler->processes[scheduler->nextUnusedPid] != NULL)
		scheduler->nextUnusedPid =
			(scheduler->nextUnusedPid + 1) % MAX_PROCESSES;
	scheduler->qtyProcesses++;
	return get_pid(process);
}

//=======================TODO=======================//

static void destroyZombie(SchedulerADT scheduler, ProcessADT zombie) {
	Node *zombieNode = scheduler->processes[get_pid(zombie)];
	scheduler->qtyProcesses--;
	scheduler->processes[get_pid(zombie)] = NULL;
	freeProcess(zombie);
	freeMemory(zombieNode);
}

int32_t killCurrentProcess(int32_t retValue) {
	SchedulerADT scheduler = getSchedulerADT();
	driver_printStr("\nKill current process: ", (Color) {0xAA, 0xFF, 0xFF});
	driver_printStr(getName(scheduler->processes[scheduler->currentPid]->data),
					(Color) {0xAA, 0xFF, 0xFF});
	return killProcess(scheduler->currentPid, retValue);
}

int32_t killProcess(uint16_t pid, int32_t retValue) {
	SchedulerADT scheduler	= getSchedulerADT();
	Node *processToKillNode = scheduler->processes[pid];
	if (processToKillNode == NULL)
		return -1;

	ProcessADT processToKill = (ProcessADT) processToKillNode->data;
	if (get_status(processToKill) == ZOMBIE ||
		isUnkillable(processToKill)) /// para activar el ctrl+c saca el !
		return -1;

	// closeFileDescriptors(processToKill);

	uint8_t priorityIndex = get_status(processToKill) != BLOCKED ?
								get_priority(processToKill) :
								BLOCKED_INDEX;
	removeNode(scheduler->levels[priorityIndex], processToKillNode);
	set_retValue(processToKill, retValue);

	set_status(processToKill, ZOMBIE);

	begin(getZombieChildren(processToKill));
	while (hasNext(getZombieChildren(processToKill))) {
		destroyZombie(scheduler,
					  (ProcessADT) next(getZombieChildren(processToKill)));
	}

	Node *parentNode = scheduler->processes[getParentPid(processToKill)];
	if (parentNode != NULL &&
		get_status((ProcessADT) parentNode->data) != ZOMBIE) {
		ProcessADT parent = (ProcessADT) parentNode->data;
		appendNode(getZombieChildren(processToKill), processToKillNode);
		if (processIsWaiting(parent, get_pid(processToKill))) {
			setStatus(get_pid(processToKill), READY);
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
	SchedulerADT scheduler	 = getSchedulerADT();
	scheduler->killFgProcess = 1;
	forceTimerTick();
}

int32_t blockProcess(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();

	Node *node = scheduler->processes[pid];

	if (node == NULL)
		return -1;

	ProcessADT process	 = node->data;
	ProcessStatus status = get_status(process);

	if (status == BLOCKED || status == ZOMBIE)
		return 0;

	set_status(process, BLOCKED);
	removeNode(scheduler->levels[get_priority(process)], node);

	// driver_printStr("\nBlock process: ", (Color) {0xAA, 0xFF, 0xFF});
	// driver_printNum(pid, (Color) {0xAA, 0xFF, 0xFF});

	return 0;
}

int32_t unblockProcess(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node			   = scheduler->processes[pid];

	if (node == NULL /*|| pid == IDLE_PID*/) {
		return -1;
	}
	ProcessADT process			= node->data;
	ProcessStatus currentStatus = get_status(process);
	if (currentStatus == ZOMBIE) {
		return -1;
	}
	if (currentStatus == READY || currentStatus == RUNNING) {
		return 0;
	}
	if (currentStatus != BLOCKED) {
		return -1;
	}
	set_status(process, READY);
	scheduler->remainingQuantum = 0;

	// driver_printStr("\nUnblock process: ", (Color) {0xAA, 0xFF, 0xFF});
	// driver_printNum(pid, (Color) {0xAA, 0xFF, 0xFF});

	return 0; // ProcessStatus status);
}

//=======================TODO=======================//

//=========== SETTERS AND GETTERS ===========//

uint16_t getPid() {
	SchedulerADT scheduler = getSchedulerADT();
	return scheduler->currentPid;
}

int32_t setPriority(uint16_t pid, uint8_t newPriority) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node			   = scheduler->processes[pid];
	if (node == NULL || pid == IDLE_PID)
		return -1;

	ProcessADT process = node->data;
	if (newPriority >= QTY_READY_LEVELS)
		return -1;

	if (get_status(process) == READY || get_status(process) == RUNNING) {
		removeNode(scheduler->levels[get_priority(process)], node);
		scheduler->processes[get_pid(process)] =
			appendNode(scheduler->levels[newPriority], node);
	}
	set_priority(process, newPriority);

	// driver_printStr("\nSet Priority: ", (Color) {0xAA, 0xFF, 0xFF});
	// driver_printNum(newPriority, (Color) {0xAA, 0xFF, 0xFF});

	return newPriority;
}

int8_t setStatus(uint16_t pid, uint8_t newStatus) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node			   = scheduler->processes[pid];
	if (node == NULL || pid == IDLE_PID)
		return -1;

	ProcessADT process		= node->data;
	ProcessStatus oldStatus = get_status(process);

	if (newStatus == RUNNING || newStatus == ZOMBIE || oldStatus == ZOMBIE)
		return -1;

	if (newStatus == get_status(process))
		return newStatus;

	set_status(process, newStatus);
	if (newStatus == BLOCKED) {
		removeNode(scheduler->levels[get_priority(process)], node);
		appendNode(scheduler->levels[BLOCKED_INDEX], node);
	}
	else if (oldStatus == BLOCKED) {
		removeNode(scheduler->levels[BLOCKED_INDEX], node);
		set_priority(process, MAX_PRIORITY);
		prependNode(scheduler->levels[get_priority(process)], node);
		scheduler->remainingQuantum = 0;
	}
	return newStatus;
}

int getQtyProcesses(SchedulerADT scheduler) {
	return scheduler->qtyProcesses;
}

int getNextUnusedPid(SchedulerADT scheduler) {
	return scheduler->nextUnusedPid;
}

int32_t getZombieRetValue(uint16_t pid) {
	// driver_printStr("\nGet Zombie Ret Value: ", (Color) {0xAA, 0xFF, 0xFF});
	// driver_printNum(pid, (Color) {0xAA, 0xFF, 0xFF});

	SchedulerADT scheduler = getSchedulerADT();
	Node *zombieNode	   = scheduler->processes[pid];
	if (zombieNode == NULL)
		return -1;
	ProcessADT zombieProcess = (ProcessADT) zombieNode->data;
	if (getParentPid(zombieProcess) != scheduler->currentPid)
		return -1;

	ProcessADT parent =
		(ProcessADT) scheduler->processes[scheduler->currentPid]->data;
	setWaitingForPid(parent, pid);

	if (get_status(zombieProcess) != ZOMBIE) {
		setStatus(get_pid(parent), BLOCKED);
		yield();
	}
	removeNode(getZombieChildren(parent), zombieNode);
	destroyZombie(scheduler, zombieProcess);
	return get_retValue(zombieProcess);
}

int32_t processIsAlive(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *processNode	   = scheduler->processes[pid];
	return processNode != NULL &&
		   get_status((ProcessADT) processNode->data) != ZOMBIE;
}

void yield() {
	SchedulerADT scheduler		= getSchedulerADT();
	scheduler->remainingQuantum = 0;
	forceTimerTick(); // Este llama a int 0x20
}