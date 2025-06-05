#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

#define SYSTEM_VARIABLES 0x5A00

#define MEMORY_MANAGER_ADDRESS 0x50000
#define SCHEDULER_ADDRESS 0x60000
#define SEMAPHORE_MANAGER_ADDRESS 0x70000
#define PIPE_MANAGER_ADDRESS 0x80000 // PipeManagerCDT

#define SYSTEM_VARIABLES 0x5A00
#define PIPE_SIZE (1 << 12)
#define MAX_PIPES (1 << 12)
#define STACK_SIZE (1 << 12)   // 4KB stack size
#define BUILT_IN_DESCRIPTORS 3 // stdin, stdout, stderr
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ 0
#define WRITE 1

#define MAX_PROCESSES (1 << 12)
#define QTY_READY_LEVELS 5
#define MAX_PRIORITY 4
#define MIN_PRIORITY 0
#define BLOCKED_INDEX QTY_READY_LEVELS
#define IDLE_PID 0
#define QUANTUM_COEF 2

#define EOF (-1)
#define DEV_NULL -1

typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} Color;

typedef int (*MainFunction)(int argc, char **args);

#endif // GLOBALS_H