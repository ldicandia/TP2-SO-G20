// filepath: TPE_ARQUI/Kernel/processes/test_schedule.c

#include <assert.h>
#include <stdio.h>
#include "schedule.h"
#include "memoryManager.h"
#include "linkedListADT.h"
#include <videoDriver.h>

void test_createScheduler() {
	SchedulerADT scheduler = createScheduler();
	assert(scheduler != NULL);
	assert(getQtyProcesses(scheduler) == 0);
	assert(getNextUnusedPid(scheduler) == 0);
	driver_printStr("test_createScheduler passed\n",
					(Color) {0x00, 0xFF, 0x00});
}

void test_createProcess() {
	SchedulerADT scheduler = createScheduler();
	char *args[]		   = {NULL};
	uint16_t pid		   = createProcess(NULL, args, "test_process", 2, NULL);
	assert(pid != -1);
	assert(getQtyProcesses(scheduler) == 1);
	driver_printStr("test_createProcess passed\n", (Color) {0x00, 0xFF, 0x00});
}

void test_schedule() {
	SchedulerADT scheduler = createScheduler();
	char *args[]		   = {NULL};
	createProcess(NULL, args, "process1", 2, NULL);
	createProcess(NULL, args, "process2", 1, NULL);

	void *prevStackPointer = NULL;
	void *nextStackPointer = schedule(prevStackPointer);
	assert(nextStackPointer != NULL);
	driver_printStr("test_schedule passed\n", (Color) {0x00, 0xFF, 0x00});
}

void test_killProcess() {
	SchedulerADT scheduler = createScheduler();
	char *args[]		   = {NULL};
	uint16_t pid = createProcess(NULL, args, "process_to_kill", 2, NULL);
	assert(pid != -1);

	int result = killProcess(pid, 0);
	assert(result == 0);
	assert(getQtyProcesses(scheduler) == 0);
	driver_printStr("test_killProcess passed\n", (Color) {0x00, 0xFF, 0x00});
}

void test_setPriority() {
	SchedulerADT scheduler = createScheduler();
	char *args[]		   = {NULL};
	uint16_t pid = createProcess(NULL, args, "priority_test", 2, NULL);
	assert(pid != -1);

	int result = setPriority(pid, 3);
	assert(result == 3);
	driver_printStr("test_setPriority passed\n", (Color) {0x00, 0xFF, 0x00});
}

void test_setStatus() {
	SchedulerADT scheduler = createScheduler();
	char *args[]		   = {NULL};
	uint16_t pid		   = createProcess(NULL, args, "status_test", 2, NULL);
	assert(pid != -1);

	int result = setStatus(pid, BLOCKED);
	assert(result == BLOCKED);
	driver_printStr("test_setStatus passed\n", (Color) {0x00, 0xFF, 0x00});
}

int main() {
	test_createScheduler();
	test_createProcess();
	test_schedule();
	test_killProcess();
	test_setPriority();
	test_setStatus();
	driver_printStr("All tests passed!\n", (Color) {0x00, 0xFF, 0x00});
	return 0;
}