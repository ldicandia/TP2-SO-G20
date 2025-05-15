
#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <PCB.h>
#include <keyboard.h>
#include <time.h>

#define PCB_AMOUNT 100

typedef struct process_info {
	pid_t pid;
	priority_t priority;
	uint64_t lowest_stack_address;
	uint64_t stack_pointer;
	uint8_t status;
	uint8_t is_background;
	fd_t fds[3];
	char *name;
} process_info;

typedef struct process_info_list {
	uint64_t amount_of_processes;
	process_info *processes;
} process_info_list;

typedef int (*main_function)(char **argv, uint64_t argc);

int64_t new_process(main_function rip, priority_t priority, uint8_t killable,
					char **argv, uint64_t argc, fd_t fds[]);
PCB *get_pcb(pid_t pid);
void list_processes();
int64_t kill_process(pid_t pid);
int64_t kill_process_pcb(PCB *pcb);
pid_t wait(pid_t pid, int64_t *ret);
process_info_list *ps();
int8_t get_status(pid_t pid);
void free_ps(process_info_list *ps);
void close_fds(PCB *pcb);
void ctrl_c_handler();
int64_t make_me_zombie(int64_t retval);
#endif
