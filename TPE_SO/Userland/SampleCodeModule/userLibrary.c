// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <userLibrary.h>
#include <user_time.h>
#include <shared.h>

#include "./include/sys_calls.h"

typedef enum { BLOCKED = 0, READY, RUNNING, ZOMBIE, DEAD } ProcessStatus;

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define MAX_PRIORITY 4

const Color BLACK		 = {0, 0, 0};
const Color WHITE		 = {255, 255, 255};
const Color RED			 = {0, 0, 255};
const Color LIGHT_BLUE	 = {173, 216, 230};
const Color BLUE		 = {255, 0, 0};
const Color ORANGE		 = {16, 160, 255};
const Color YELLOW		 = {30, 224, 255};
const Color PURPLE		 = {255, 32, 160};
const Color PINK		 = {100, 0, 244};
const Color GREEN		 = {0, 255, 0};
const Color LIGHT_RED	 = {0, 255, 255};
const Color LIGHT_PURPLE = {255, 0, 255};
const Color LIGHT_ORANGE = {0, 160, 255};
const Color LIGHT_YELLOW = {0, 224, 255};
const Color LIGHT_PINK	 = {0, 100, 244};
const Color LIGHT_GREEN	 = {0, 255, 0};

static char *register_names[17] = {"RIP", "RAX", "RBX", "RCX", "RDX", "RSI",
								   "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10",
								   "R11", "R12", "R13", "R14", "R15"};

char printChar(char c) {
	u_sys_write(STDOUT, &c, 1);
	return c;
}

void printInteger(int n) {
	char buf[16];
	int i = 0;
	if (n < 0) {
		printChar('-');
		n = -n;
	}
	do {
		buf[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	while (i-- > 0) {
		printChar(buf[i]);
	}
}

void drawSquare(int x, int y, uint32_t fillColor) {
	u_sys_drawSquare(x, y, fillColor);
}

char getChar() {
	char c;
	u_sys_read(STDIN, &c, 1);
	return c;
}

int getCharInt() {
	char c;
	u_sys_read(STDIN, &c, 1);
	return c;
}

void clear() {
	u_sys_clear();
}

void increment_size_char() {
	u_sys_increment_size();
}

void decrement_size_char() {
	u_sys_decrement_size();
}

void printCharColor(char c, Color fnt) {
	u_sys_write_color(STDOUT, c, fnt);
}

void printStrColor(char *str, Color fnt) {
	for (int i = 0; str[i]; i++) {
		u_sys_write_color(STDOUT, str[i], fnt);
	}
}

void printStr(char *str) {
	for (int i = 0; str[i]; i++) {
		printChar(str[i]);
	}
}

int isChar(char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
		return 1;
	}
	return 0;
}

int isDigit(char c) {
	if (c >= '0' && c <= '9') {
		return 1;
	}
	return 0;
}

void inforeg() {
	char hexbuf[19];
	hexbuf[0]  = '0';
	hexbuf[1]  = 'x';
	hexbuf[18] = '\0';

	uint64_t registers[17];
	int i = u_sys_infoReg(registers);

	static Color red = {30, 30, 255};

	printChar('\n');
	if (i == 1) {
		for (int i = 0; i < 17; i++) {
			printStrColor(register_names[i], red);
			printStrColor(": ", red);
			uint64ToHex(registers[i], hexbuf + 2);
			printStr(hexbuf);
			if (i % 4 == 3)
				printChar('\n');
			else
				printStr("   ");
		}
	}
	else {
		printStr("\nPress 'SHIFT + TAB' to save registers and then inforeg");
	}
}

void exc_invopcode() {
	u_exc_invopcode();
}

void exc_zerodiv() {
	u_exc_zerodiv();
}

void printHex(uint64_t n) {
	char hexbuf[20] = {0};
	uint64ToHex(n, hexbuf);
	printStr(hexbuf);
}

void uint64ToHex(uint64_t n, char buf[16]) {
	int i = 15;
	do {
		int digit = n % 16;
		buf[i]	  = (digit < 10 ? '0' : ('A' - 10)) + digit;
		n /= 16;
	} while (i-- != 0);
}

int secondsToMiliseconds(int seconds) {
	return seconds * 1000;
}

void sleep_miliseconds(int miliseconds) {
	int start = secondsToMiliseconds(getSeconds());
	while (secondsToMiliseconds(getSeconds()) - start < miliseconds)
		;
}

void playSound(uint32_t frequence, uint64_t duration) {
	u_sys_playSound(frequence, duration);
}

void stopSound() {
	u_sys_stopSound();
}

void *allocMemory(uint64_t size) {
	return u_sys_malloc(size);
}

void freeMemory(void *address) {
	u_sys_free(address);
}

int create_process(MainFunction code, char **args, char *name,
				   uint8_t priority) {
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};
	return u_sys_create_process(code, args, name, priority, fileDescriptors);
}

int create_process_with_fds(MainFunction code, char **args, char *name,
							uint8_t priority, int16_t fileDescriptors[]) {
	return u_sys_create_process(code, args, name, priority, fileDescriptors);
}

int kill_process(uint64_t pid) {
	return u_sys_kill_process(pid);
}

int set_prio(uint64_t pid, uint64_t newPrio) {
	return u_sys_set_prio(pid, newPrio);
}

int block(uint64_t pid) {
	return u_sys_block_process(pid);
}

int unblock(uint64_t pid) {
	return u_sys_unblock_process(pid);
}

int getpid() {
	return u_sys_get_pid();
}

int yield() {
	return u_sys_yield();
}

int wait_pid(uint16_t pid) {
	return u_sys_wait_pid(pid);
}

int ps() {
	char *statusNames[4]	   = {"BLOCKED", "READY", "RUNNING", "ZOMBIE"};
	ProcessInfoList *snapshots = (ProcessInfoList *) u_sys_ps();
	printStrColor("\n========================================================"
				  "==========================\n",
				  WHITE);
	for (int i = 0; i < snapshots->length; i++) {
		ProcessInfo *snapshot = &snapshots->snapshotList[i];
		// printf("%s\n", statusNames[snapshot->status]);
		// printStr("\n");
		printStrColor("PID: ", LIGHT_GREEN);
		printInteger(snapshot->pid);
		printStr(" ");
		printStrColor("Name:", LIGHT_GREEN);
		printStr(snapshot->name);
		printStr(" ");
		printStrColor("Status:", LIGHT_GREEN);
		printStr(statusNames[snapshot->status]);
		printStr(" ");
		printStrColor("Priority:", LIGHT_GREEN);
		printInteger(snapshot->priority);
		printStr(" ");
		printStrColor("Memory Used:", LIGHT_GREEN);
		printInteger(snapshot->memoryUsed);
		printStr("\n");
		printStrColor("Stack Pointer:", LIGHT_GREEN);
		printHex(snapshot->stackPointer);
		printStr(" ");
		printStrColor("Base Pointer:", LIGHT_GREEN);
		printHex(snapshot->basePointer);
		printStr(" ");
		printStrColor("Foreground:", LIGHT_GREEN);
		printInteger(snapshot->foreground);
		printStr("\n");
		printStrColor("========================================================"
					  "==========================\n",
					  WHITE);
		// free(snapshots->snapshotList[i].name);
	}
	freeMemory(snapshots->snapshotList);
	freeMemory(snapshots);
	return 0;
}

int acquirePipeAccess(uint16_t pid, uint8_t mode) {
	return u_sys_pipeOpen(pid, mode);
}

int releasePipeAccess(uint16_t pid) {
	return u_sys_pipeClose(pid);
}

int getPipe() {
	return u_sys_getPipe();
}

int user_sem_wait(uint16_t sem_id) {
	return u_sys_sem_wait(sem_id);
}

int user_sem_post(uint16_t sem_id) {
	return u_sys_sem_post(sem_id);
}

int user_sem_open(uint16_t sem_id, uint64_t initialValue) {
	return u_sys_sem_open(sem_id, initialValue);
}

int user_sem_close(uint16_t sem_id) {
	return u_sys_sem_close(sem_id);
}

int user_mem(MemoryInfo *info) {
	return u_sys_getMemoryInfo(info);
}
