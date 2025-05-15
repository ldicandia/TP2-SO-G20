#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <process.h>
typedef struct {
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rsi, rdi, rbp, rdx, rcx, rbx,
		rax;
} stack_registers;

typedef struct stack {
	stack_registers regs;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} stack;

uint64_t load_stack(uint64_t rip, uint64_t rsp, char **argv, uint64_t argc,
					pid_t pid);

#endif