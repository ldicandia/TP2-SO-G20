#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>
#include <linkedListADT.h>
#define BUILT_IN_DESCRIPTORS 3 // stdin, stdout, stderr
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ 0
#define WRITE 1

typedef enum { BLOCKED = 0, READY, RUNNING, ZOMBIE, DEAD } ProcessStatus;

typedef struct ProcessCDT *ProcessADT;

typedef int (*MainFunction)(int argc, char **args);

void initProcess(ProcessADT process, uint16_t pid, uint16_t parentPid,
				 int code(int argc, char **args), char **args, char *name,
				 uint8_t priority, int16_t fileDescriptors[],
				 uint8_t unkillable);

int processIsWaiting(ProcessADT process, uint16_t pidToWait);

void freeProcess(ProcessADT process);

void closeFileDescriptors(ProcessADT process);

// New function declarations
uint16_t get_pid(ProcessADT process);

void set_stackPos(ProcessADT process, void *stackPos);

void set_status(ProcessADT process, ProcessStatus status);

ProcessStatus get_status(ProcessADT process);

uint8_t get_priority(ProcessADT process);

void set_priority(ProcessADT process, uint8_t priority);

int16_t get_fileDescriptor(ProcessADT process, uint8_t fdIndex);

void *get_stackPos(ProcessADT process);

int32_t get_retValue(ProcessADT process);

void set_retValue(ProcessADT process, int32_t retValue);

void set_initialized(ProcessADT process, int initialized);

int get_initialized(ProcessADT process);

void *get_stackBase(ProcessADT process);

int64_t sizeofProcess();

char *getName(ProcessADT process);

uint8_t isUnkillable(ProcessADT process);

LinkedListADT getZombieChildren(ProcessADT process);

uint16_t getParentPid(ProcessADT process);

uint16_t getWaitingForPid(ProcessADT process);

void setWaitingForPid(ProcessADT process, uint16_t pid);

#endif