#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>
#include <linkedListADT.h>
#include <shared.h>
#include <globals.h>

typedef enum { BLOCKED = 0, READY, RUNNING, ZOMBIE, DEAD } ProcessStatus;

typedef struct ProcessCDT *ProcessADT;

typedef int (*MainFunction)(int argc, char **args);

void initProcess(ProcessADT process, uint16_t pid, uint16_t parentPid,
				 int code(int argc, char **args), char **args, char *name,
				 uint8_t priority, int16_t fileDescriptors[],
				 uint8_t unkillable);

int isProcessWaitingFor(ProcessADT process, uint16_t pidToWait);

void freeProcess(ProcessADT process);

void closeFileDescriptors(ProcessADT process);

// New function declarations
uint16_t getProcessId(ProcessADT process);

void setProcessStackPosition(ProcessADT process, void *stackPos);

void setProcessStatus(ProcessADT process, ProcessStatus status);

ProcessStatus getProcessStatus(ProcessADT process);

uint8_t getProcessPriority(ProcessADT process);

void setProcessPriority(ProcessADT process, uint8_t priority);

int16_t getProcessFileDescriptor(ProcessADT process, uint8_t fdIndex);

void *getProcessStackPosition(ProcessADT process);

int32_t getProcessReturnValue(ProcessADT process);

void setProcessReturnValue(ProcessADT process, int32_t retValue);

void setProcessInitialized(ProcessADT process, int initialized);

int getProcessInitializationStatus(ProcessADT process);

void *getProcessStackBase(ProcessADT process);

int64_t sizeofProcess();

char *getName(ProcessADT process);

uint8_t isUnkillable(ProcessADT process);

LinkedListADT getZombieChildren(ProcessADT process);

uint16_t getParentPid(ProcessADT process);

uint16_t getWaitingForPid(ProcessADT process);

void setWaitingForPid(ProcessADT process, uint16_t pid);

void setFileDescriptor(ProcessADT process, uint8_t fdIndex, int16_t fdValue);

void incrementWaitingTime(ProcessADT process);

uint16_t getWaitingTime(ProcessADT process);

void setWaitingTime(ProcessADT process, uint16_t waitingTime);

ProcessInfo *loadInfo(ProcessInfo *snapshot, ProcessADT process);

int collectZombieProcessInfo(int processIndex, ProcessInfo psArray[],
							 ProcessADT nextProcess);

#endif