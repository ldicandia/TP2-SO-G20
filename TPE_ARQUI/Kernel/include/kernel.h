#ifndef KERNEL_H
#define KERNEL_H

#include <memoryManager.h>

MemoryManagerADT get_userland_mem();
MemoryManagerADT get_kernel_mem();
int main();
char *new_str_copy(char *string);
char **copy_argv(pid_t pid, char **argv, uint64_t argc);

#endif