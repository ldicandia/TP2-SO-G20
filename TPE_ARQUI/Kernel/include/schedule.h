#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdint.h>
#include <process.h>
#include <shared.h>

#define MAX_PROCESSES (1 << 12)
#define QTY_READY_LEVELS 5
#define MAX_PRIORITY 4
#define MIN_PRIORITY 0
#define BLOCKED_INDEX QTY_READY_LEVELS
#define IDLE_PID 0
#define QUANTUM_COEF 2

typedef struct SchedulerCDT *SchedulerADT;

SchedulerADT createScheduler();

SchedulerADT getSchedulerADT();

int32_t setPriority(uint16_t pid, uint8_t newPriority);

int8_t setStatus(uint16_t pid, uint8_t newStatus);

ProcessStatus getProcessStatus(uint16_t pid);

void *schedule(void *prevStackPointer);

uint16_t createProcess(MainFunction code, char **args, char *name,
					   uint8_t priority, int16_t fileDescriptors[],
					   uint8_t unkillable);

int32_t killCurrentProcess(int32_t retValue);

int32_t killProcess(uint16_t pid, int32_t retValue);

uint16_t getpid();

ProcessInfoList *getProcessInfoList();

int32_t getZombieRetValue(uint16_t pid);

int32_t processIsAlive(uint16_t pid);

void yield();

int8_t changeFD(uint16_t pid, uint8_t position, int16_t newFd);

int16_t getCurrentProcessFileDescriptor(uint8_t fdIndex);

void killForegroundProcess();

int32_t killCurrentProcess(int32_t retValue);

int32_t killProcess(uint16_t pid, int32_t retValue);

int32_t unblockProcess(uint16_t pid);

int32_t blockProcess(uint16_t pid);

uint16_t getPid();

int getQtyProcesses(SchedulerADT scheduler);

int getNextUnusedPid(SchedulerADT scheduler);

void printAllProcesses(SchedulerADT scheduler);

ProcessADT getCurrentProcess();

#endif