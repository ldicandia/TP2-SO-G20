// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <lib.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <schedule.h>
#include <stdlib.h>
#include <videoDriver.h>
#include <shared.h>

#define QTY_READY_LEVELS 5
#define MAX_PRIORITY 4
#define MIN_PRIORITY 0
#define BLOCKED_INDEX QTY_READY_LEVELS
#define MAX_PROCESSES (1 << 12)
#define IDLE_PID 0
#define QUANTUM_COEF 2
#define AGING_THRESHOLD 10 // Threshold for aging processes

#define SCHEDULER_ADDRESS 0x60000

typedef struct SchedulerCDT {
	Node *processes[MAX_PROCESSES];
	LinkedListADT levels[QTY_READY_LEVELS];
	LinkedListADT blockedProcesses; // Separate list for blocked processes
	uint16_t currentPid;
	uint16_t nextUnusedPid;
	uint16_t qtyProcesses;
	int8_t remainingQuantum;
	int8_t killFgProcess;
	uint8_t currentLevel;
} SchedulerCDT;

extern void forceTimerTick();

SchedulerADT createScheduler() {
	SchedulerADT scheduler = (SchedulerADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < QTY_READY_LEVELS; i++)
		scheduler->levels[i] = createLinkedListADT();
	scheduler->blockedProcesses = createLinkedListADT();
	scheduler->nextUnusedPid	= 0;
	scheduler->killFgProcess	= 0;
	scheduler->currentPid		= 0;
	scheduler->currentLevel		= 0;
	scheduler->qtyProcesses		= 0; ////OJO CON ESTO
	return scheduler;
}

SchedulerADT getSchedulerADT() {
	return (SchedulerADT) SCHEDULER_ADDRESS;
}

static uint16_t getNextPid(SchedulerADT scheduler) {
	ProcessADT process = NULL;

	// Rotamos niveles de prioridad en orden circular
	for (int i = 0; i < QTY_READY_LEVELS; i++) {
		int lvl = (scheduler->currentLevel + i) % QTY_READY_LEVELS;
		if (!isEmpty(scheduler->levels[lvl])) {
			Node *node = getFirst(scheduler->levels[lvl]);
			process	   = node->data;

			// Rotación dentro del mismo nivel (round-robin interno)
			if (getLength(scheduler->levels[lvl]) > 1) {
				removeNode(scheduler->levels[lvl], node);
				appendNode(scheduler->levels[lvl], node);
			}

			scheduler->currentLevel = (lvl + 1) % QTY_READY_LEVELS;

			return get_pid(process);
		}
	}
	return IDLE_PID;
}

void applyAging(SchedulerADT scheduler) {
	for (int lvl = 0; lvl < MAX_PRIORITY; lvl++) {
		Node *node = getFirst(scheduler->levels[lvl]);
		while (node != NULL) {
			ProcessADT process = node->data;
			node			   = node->next;

			incrementWaitingTime(process);

			if (getWaitingTime(process) >= AGING_THRESHOLD) {
				// Promover de prioridad
				removeNode(scheduler->levels[lvl],
						   scheduler->processes[get_pid(process)]);
				set_priority(process, lvl + 1);
				appendNode(scheduler->levels[lvl + 1],
						   scheduler->processes[get_pid(process)]);
				setWaitingTime(process, 0);
			}
		}
	}
}

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
	Color c = (Color) {0xFF, 0xFF, 0xFF};
	driver_printStr("\n[Processes]\n", c);
	// Cabecera
	driver_printStr("PID   Name      Status   Prio\n", c);
	driver_printStr("---   ----      ------   ----\n", c);

	for (int i = 0; i < MAX_PROCESSES; i++) {
		Node *n = scheduler->processes[i];
		if (!n)
			continue;
		ProcessADT p = n->data;
		if (!p)
			continue;

		// PID
		driver_printNum(get_pid(p), c);
		driver_printStr("     ", c);

		// Nombre (asumimos <=8 chars; si no, ajusta el tabulado)
		driver_printStr(getName(p), c);
		driver_printStr("        ", c);

		// Estado
		driver_printStr(statusToString(get_status(p)), c);
		driver_printStr("   ", c);

		// Prioridad
		driver_printNum(get_priority(p), c);
		driver_printStr("\n", c);
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
		driver_printStr(" Processes: ", (Color) {0xAA, 0xFF, 0xFF});
		Node *currentNode = getFirst(scheduler->levels[i]);
		while (currentNode != NULL) {
			ProcessADT process = currentNode->data;
			driver_printStr(getName(process), (Color) {0xAA, 0xFF, 0xFF});
			currentNode = currentNode->next;
			if (currentNode != NULL)
				driver_printStr(", ", (Color) {0xAA, 0xFF, 0xFF});
		}
		driver_printStr("\n", (Color) {0xAA, 0xFF, 0xFF});
	}
}

void printCurrentProcess(SchedulerADT scheduler) {
	driver_printStr("\n[Scheduler Current Process]: ",
					(Color) {0xAA, 0xFF, 0xFF});
	driver_printStr("\nPID: ", (Color) {0xAA, 0xFF, 0xFF});
	driver_printNum(scheduler->currentPid, (Color) {0xAA, 0xFF, 0xFF});
	driver_printStr(" \nName: ", (Color) {0xAA, 0xFF, 0xFF});
	driver_printStr(getName(scheduler->processes[scheduler->currentPid]->data),
					(Color) {0xAA, 0xFF, 0xFF});
}

static int firstTime = 1;

void *schedule(void *prevStackPointer) {
	// static int firstTime   = 1;
	SchedulerADT scheduler = getSchedulerADT();

	// printAllProcesses(scheduler);
	if (scheduler->currentPid != 0 && scheduler->currentPid != 1) {
		// printAllProcesses(scheduler);
		// printLevels(scheduler);
	}

	if (!firstTime) {
		// printCurrentProcess(scheduler);
		// driver_printNum(scheduler->remainingQuantum,
		//				(Color) {0xAA, 0xFF, 0xFF});
	}
	scheduler->remainingQuantum--;

	if (scheduler->remainingQuantum > 0) {
		return prevStackPointer;
	}

	ProcessADT currentProcess;
	Node *currentProcessNode = scheduler->processes[scheduler->currentPid];

	if (currentProcessNode != NULL) {
		currentProcess = (ProcessADT) currentProcessNode->data;
		if (!firstTime)
			set_stackPos(currentProcess, prevStackPointer);
		else
			firstTime = 0;

		if (get_status(currentProcess) == RUNNING)
			set_status(currentProcess, READY);
		// uint8_t newPriority;
		// newPriority = get_priority(currentProcess) > 0 ?
		// 				  get_priority(currentProcess) - 1 :
		// 				  get_priority(currentProcess);
		// setPriority(get_pid(currentProcess), newPriority);
	}

	// applyAging(scheduler);
	scheduler->currentPid = getNextPid(scheduler);
	currentProcess		  = scheduler->processes[scheduler->currentPid]->data;

	if (scheduler->killFgProcess &&
		get_fileDescriptor(currentProcess, STDIN) == STDIN) {
		scheduler->killFgProcess = 0;
		if (killCurrentProcess(-1) != -1)
			forceTimerTick();
	}

	scheduler->remainingQuantum = get_priority(currentProcess) + 1;

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
	SchedulerADT scheduler = getSchedulerADT();
	if (scheduler->qtyProcesses >= MAX_PROCESSES) {
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
	initProcess(process, scheduler->nextUnusedPid, scheduler->currentPid, code,
				args, name, priority, fileDescriptors, unkillable);

	Node *processNode;
	if (get_pid(process) != IDLE_PID) {
		processNode = appendElement(scheduler->levels[get_priority(process)],
									(void *) process);
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
	uint16_t zombiePid = get_pid(zombie);
	Node *zombieNode   = scheduler->processes[zombiePid];

	// never destroy the shell
	if (zombiePid == 1)
		return;

	scheduler->qtyProcesses--;
	scheduler->processes[zombiePid] = NULL;

	// Update nextUnusedPid only if we need to
	// If we don't have any processes, start from lowest PID
	if (scheduler->qtyProcesses == 0) {
		scheduler->nextUnusedPid = 2; // Start from 1 to avoid IDLE_PID (0)
	}
	// Otherwise, only update if this is lower than the current next pid
	// and we're not already at this slot
	else if (zombiePid < scheduler->nextUnusedPid &&
			 scheduler->processes[zombiePid] == NULL) {
		scheduler->nextUnusedPid = zombiePid;
	}

	freeProcess(zombie);
	freeMemory(zombieNode);
}

int32_t killCurrentProcess(int32_t retValue) {
	SchedulerADT scheduler = getSchedulerADT();
	driver_printStr("\n[Scheduler]: Killing current process\n",
					(Color) {0xFF, 0x00, 0x00});
	driver_printStr("PID: ", (Color) {0xFF, 0x00, 0x00});
	driver_printNum(scheduler->currentPid, (Color) {0xFF, 0x00, 0x00});
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

	closeFileDescriptors(processToKill);

	if (get_status(processToKill) == BLOCKED) {
		removeNode(scheduler->blockedProcesses,
				   processToKillNode); // Changed from levels[BLOCKED_INDEX]
	}
	else {
		removeNode(scheduler->levels[get_priority(processToKill)],
				   processToKillNode);
	}

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
		appendNode(getZombieChildren(parent), processToKillNode);
		if (processIsWaiting(parent, get_pid(processToKill))) {
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
	appendNode(scheduler->blockedProcesses, node);
	return 0;
}

void printBlockedProcesses() {
	SchedulerADT scheduler = getSchedulerADT();
	driver_printStr("\n[Blocked Processes]: ", (Color) {0xAA, 0xFF, 0xFF});
	Node *node = getFirst(scheduler->blockedProcesses);
	while (node != NULL) {
		ProcessADT process = node->data;
		driver_printStr("\nPID: ", (Color) {0xAA, 0xFF, 0xFF});
		driver_printNum(get_pid(process), (Color) {0xAA, 0xFF, 0xFF});
		node = node->next;
	}
}

int32_t unblockProcess(uint16_t pid) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node			   = scheduler->processes[pid];
	if (!node)
		return -1;

	// printBlockedProcesses();

	ProcessADT process	 = node->data;
	ProcessStatus status = get_status(process);
	if (status != BLOCKED)
		return status == ZOMBIE ? -1 : 0;

	// detach & free old
	removeNode(scheduler->blockedProcesses, node);
	freeMemory(node);

	// mark READY & re-enqueue with a new node
	set_status(process, READY);
	Node *newNode = appendElement(scheduler->levels[get_priority(process)],
								  (void *) process);
	scheduler->processes[pid] = newNode;
	// printBlockedProcesses();

	// force reschedule
	scheduler->remainingQuantum = 0;
	return 0;
}

ProcessInfoList *getProcessInfoList() {
	SchedulerADT scheduler			= getSchedulerADT();
	ProcessInfoList *snapshotsArray = allocMemory(sizeof(ProcessInfoList));
	if (snapshotsArray == NULL) {
		return NULL; // Error al asignar memoria
	}

	ProcessInfo *psArray =
		allocMemory(scheduler->qtyProcesses * sizeof(ProcessInfo));
	if (psArray == NULL) {
		freeMemory(snapshotsArray);
		return NULL; // Error al asignar memoria
	}

	int processIndex = 0;

	// Cargar información del proceso IDLE
	loadInfo(&psArray[processIndex++],
			 (ProcessADT) scheduler->processes[IDLE_PID]->data);

	// Cargar información de los procesos en los niveles de prioridad
	for (int lvl = QTY_READY_LEVELS; lvl >= 0; lvl--) {
		begin(scheduler->levels[lvl]);
		while (hasNext(scheduler->levels[lvl])) {
			ProcessADT nextProcess = (ProcessADT) next(scheduler->levels[lvl]);
			loadInfo(&psArray[processIndex], nextProcess);
			processIndex++;
			if (get_status(nextProcess) != ZOMBIE) {
				getZombiesInfo(processIndex, psArray, nextProcess);
				processIndex += getLength(getZombieChildren(nextProcess));
			}
		}
	}

	snapshotsArray->length		 = scheduler->qtyProcesses;
	snapshotsArray->snapshotList = psArray;
	return snapshotsArray;
}

//=======================TODO=======================//

//=========== SETTERS AND GETTERS ===========//

uint16_t getPid() {
	SchedulerADT scheduler = getSchedulerADT();
	return scheduler->currentPid;
}

int checkPriority(uint8_t priority) {
	return (priority >= QTY_READY_LEVELS ||
			priority < 0); /// SIN ESTO NO FUNCIONA DEBERIA SER PRIORITY < 0
}

int32_t setPriority(uint16_t pid, uint8_t newPriority) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *node			   = scheduler->processes[pid];

	if (node == NULL || pid == IDLE_PID)
		return -1;

	ProcessADT process = node->data;
	if (checkPriority(newPriority)) { // cambiar por checkPriority
		// driver_printStr("Error: Invalid priority\n",
		//				(Color) {0xFF, 0x00, 0x00});
		return -1;
	}

	if (get_status(process) == READY || get_status(process) == RUNNING) {
		removeNode(scheduler->levels[get_priority(process)], node);
		scheduler->processes[get_pid(process)] =
			appendNode(scheduler->levels[newPriority], node);
	}
	set_priority(process, newPriority);
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
		appendNode(scheduler->blockedProcesses, node);
	}
	else if (oldStatus == BLOCKED) {
		removeNode(scheduler->blockedProcesses, node);
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

int32_t getZombieRetValue(uint16_t pid) { /// ARREGLAR
	SchedulerADT scheduler = getSchedulerADT();

	if (pid >= MAX_PROCESSES) {
		driver_printStr("Error: Invalid PID\n", (Color) {0xFF, 0x00, 0x00});
		return -1;
	}

	Node *zombieNode = scheduler->processes[pid];
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

int8_t changeFD(uint16_t pid, uint8_t position, int16_t newFd) {
	SchedulerADT scheduler = getSchedulerADT();
	Node *processNode	   = scheduler->processes[pid];
	if (pid == IDLE_PID || processNode == NULL)
		return -1;
	ProcessADT process = (ProcessADT) processNode->data;
	setFileDescriptor(process, position, newFd);
	return 0;
}

ProcessADT getCurrentProcess() {
	SchedulerADT scheduler = getSchedulerADT();
	Node *currentNode	   = scheduler->processes[scheduler->currentPid];
	if (currentNode == NULL)
		return NULL;
	return (ProcessADT) currentNode->data;
}
int16_t getCurrentProcessFileDescriptor(uint8_t fdIndex) {
	SchedulerADT scheduler = getSchedulerADT();
	ProcessADT process	   = scheduler->processes[scheduler->currentPid]->data;
	return get_fileDescriptor(process, fdIndex);
}
