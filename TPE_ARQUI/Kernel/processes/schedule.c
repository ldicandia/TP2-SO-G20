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

SchedulerADT createScheduler() {
	SchedulerADT scheduler = (SchedulerADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < MAX_PRIORITY + 1; i++)
		scheduler->levels[i] = createLinkedListADT();
	scheduler->nextUnusedPid = 0;
	scheduler->killFgProcess = 0;
	scheduler->qtyProcesses	 = 0;
	return scheduler;
}

SchedulerADT getSchedulerADT() {
	return (SchedulerADT) SCHEDULER_ADDRESS;
}

static uint16_t getNextPid(SchedulerADT scheduler) {
	ProcessADT process = NULL;
	for (int lvl = MAX_PRIORITY; lvl >= 0 && process == NULL; lvl--) {
		if (!isEmpty(scheduler->levels[lvl]))
			process = (getFirst(scheduler->levels[lvl]))->data;
	}
	return process == NULL ? IDLE_PID : get_pid(process);
}

void *schedule(void *prevStackPointer) {
	static int firstTime   = 1;
	SchedulerADT scheduler = getSchedulerADT();

	scheduler->remainingQuantum--;
	if (!scheduler->qtyProcesses || scheduler->remainingQuantum > 0)
		return prevStackPointer;

	ProcessADT currentProcess;
	Node *currentProcessNode = scheduler->processes[scheduler->currentPid];

	if (currentProcessNode != NULL) {
		currentProcess = currentProcessNode->data;
		if (!firstTime)
			set_stackPos(currentProcess, prevStackPointer);
		else
			firstTime = 0;

		if (get_status(currentProcess) == RUNNING)
			set_status(currentProcess, READY);

		uint8_t newPriority = get_priority(currentProcess) > 0 ?
								  get_priority(currentProcess) - 1 :
								  get_priority(currentProcess);
		setPriority(get_pid(currentProcess), newPriority);
	}

	// driver_printStr("Scheduler: ", (Color) {0xFF, 0xFF, 0xFF});
	// driver_printNum(scheduler->currentPid, (Color) {0xFF, 0xFF, 0xFF});

	scheduler->currentPid = getNextPid(scheduler);
	currentProcess		  = scheduler->processes[scheduler->currentPid]->data;

	// if (scheduler->killFgProcess &&
	// 	get_fileDescriptor(currentProcess, STDIN) == STDIN) {
	// 	scheduler->killFgProcess = 0;
	// 	if (killCurrentProcess(-1) != -1)
	// 		forceTimerTick();
	// }

	scheduler->remainingQuantum =
		(MAX_PRIORITY - get_priority(currentProcess)) * QUANTUM_COEF;
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
					   uint8_t priority, int16_t fileDescriptors[]) {
	// driver_printStr(name, (Color) {0xFF, 0xFF, 0xFF}); debug
	// driver_printStr("\nPID: ", (Color) {0xFF, 0xFF, 0xFF});
	// char *str = numToString(getSchedulerADT()->nextUnusedPid);
	// if (str == NULL) {
	// 	return -1;
	// }
	// driver_printStr(str, (Color) {0xFF, 0xFF, 0xFF});
	// freeMemory(str); // Free the allocated memory

	SchedulerADT scheduler = getSchedulerADT();
	if (scheduler->qtyProcesses >= MAX_PROCESSES) {
		driver_printStr("Error: Too many processes\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}

	ProcessADT process = (ProcessADT) allocMemory(sizeof(ProcessADT));
	if (process == NULL) {
		driver_printStr("Error: Memory allocation failed\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}
	initProcess(process, scheduler->nextUnusedPid, scheduler->currentPid, code,
				args, name, priority, fileDescriptors, 0);

	Node *node = appendElement(scheduler->levels[priority], process);
	scheduler->processes[scheduler->nextUnusedPid] = node;
	while (scheduler->processes[scheduler->nextUnusedPid] != NULL)
		scheduler->nextUnusedPid =
			(scheduler->nextUnusedPid + 1) % MAX_PROCESSES;
	scheduler->qtyProcesses++;
	return get_pid(process);
}

//=======================TODO=======================//

// static void destroyZombie(SchedulerADT scheduler, ProcessADT zombie);

int32_t killCurrentProcess(int32_t retValue) {
	SchedulerADT scheduler	  = getSchedulerADT();
	Node *currentProcessNode  = scheduler->processes[scheduler->currentPid];
	ProcessADT currentProcess = currentProcessNode->data;
	scheduler->processes[scheduler->currentPid] = NULL;

	scheduler->qtyProcesses--;
	removeNode(scheduler->levels[get_priority(currentProcess)],
			   currentProcessNode);

	return 0;
}

int32_t killProcess(uint16_t pid, int32_t retValue) {
	if (pid == 0) {
		driver_printStr("Error: Cannot kill idle process\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}
	SchedulerADT scheduler	= getSchedulerADT();
	Node *processToKillNode = scheduler->processes[pid];
	if (processToKillNode == NULL) {
		driver_printStr("Error: Process not found\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}
	ProcessADT processToKill  = (ProcessADT) processToKillNode->data;
	scheduler->processes[pid] = NULL;

	scheduler->qtyProcesses--;
	int aux = get_priority(processToKill);
	removeNode(scheduler->levels[aux], processToKillNode);
	// TODO: Guardar retValue en PCB para zombie?
	// TODO: llamar al timer tick si el proceso es el current
	// TODO: free heap?
	// driver_printStr("Freeing processToKillNode\n", (Color) {0xFF, 0xFF,
	// 0xFF});

	// Liberar el PID eliminado
	if (pid < scheduler->nextUnusedPid) {
		scheduler->nextUnusedPid = pid;
	}

	if (processToKillNode != NULL) {
		freeMemory(processToKillNode);
	}
	// ARREGLAR
	if (processToKill != NULL) {
		freeMemory(processToKill);
	}
	return retValue;
}

void killForegroundProcess() {
	SchedulerADT scheduler	 = getSchedulerADT();
	scheduler->killFgProcess = 1;
	// forceTimerTick();
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

void yield() {
	SchedulerADT scheduler		= getSchedulerADT();
	scheduler->remainingQuantum = 0;
	// forceTimerTick();
}