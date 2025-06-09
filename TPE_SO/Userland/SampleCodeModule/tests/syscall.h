#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// System call prototypes
int64_t my_getpid();
int64_t my_create_process(char *name, uint64_t argc, char *argv[]);
int64_t my_nice(uint64_t pid, uint64_t newPrio);
int64_t my_kill(uint64_t pid);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);


#endif // SYSCALL_H