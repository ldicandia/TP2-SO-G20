// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "irqDispatcher.h"

#include <keyboard.h>
#include <sound.h>
#include <stdint.h>
#include <time.h>
#include <videoDriver.h>
#include <memoryManager.h>
#include <process.h>
#include <schedule.h>

#define SYS_CALLS_QTY 3

extern uint8_t hasInforeg;
extern const uint64_t inforeg[17];
extern uint64_t getSeconds();
extern uint64_t getMinutes();
extern uint64_t getHours();
extern int _hlt();
extern int _interrupt_keyboardHandler();

static Color WHITE = {255, 255, 255};
static void int_20();
static void int_21();

void irqDispatcher(uint64_t irq) {
	switch (irq) {
		case 0:
			int_20();
			break;
		case 1:
			int_21();
			break;
	}
	return;
}

void int_21() {
	_interrupt_keyboardHandler();
}

void int_20() {
	timer_master();
}

static uint64_t sys_read(uint64_t fd, char *buff) {
	if (fd != 0) {
		return -1;
	}

	*buff = getCharFromKeyboard();

	return 0;
}

static uint64_t sys_write(uint64_t fd, char buffer) {
	if (fd != 1) {
		return -1;
	}

	driver_printChar(buffer, WHITE);
	return 1;
}

static uint64_t sys_write_color(uint64_t fd, char buffer, Color fnt) {
	if (fd != 1) {
		return -1;
	}

	driver_printChar(buffer, fnt);
	return 1;
}

static uint64_t sys_clear() {
	driver_clear();
	return 1;
}

static uint64_t sys_getSeconds() {
	uint64_t seconds = getSeconds();

	return ((seconds / 16) * 10) + (seconds % 16);
}

static uint64_t sys_getMinutes() {
	uint64_t minutes = getMinutes();

	return ((minutes / 16) * 10) + (minutes % 16);
}

static uint64_t sys_getHours() {
	uint64_t hours = getHours();

	return ((hours / 16) * 10) + (hours % 16);
}

static uint64_t sys_increment_size() {
	driver_increment_size();
	return 1;
}

static uint64_t sys_decrement_size() {
	driver_decrement_size();
	return 1;
}

static uint64_t sys_drawSquare(int x, int y, uint32_t fillColor) {
	drawSquare(x, y, fillColor);
	return 1;
}

static uint64_t sys_inforeg(uint64_t registers[17]) {
	if (hasInforeg) {
		for (uint8_t i = 0; i < 17; i++) {
			registers[i] = inforeg[i];
		}
	}
	return hasInforeg;
}

uint64_t sys_sleep(uint64_t ms) {
	if (ms > 0) {
		int start_ms = ms_elapsed();
		do {
			_hlt();
		} while (ms_elapsed() - start_ms < ms);
	}
	return 1;
}

static uint64_t sys_playSound(uint32_t freq, uint64_t duration) {
	beep(freq, duration);
	return 1;
}

static uint64_t sys_stopSound() {
	stopSpeaker();
	return 1;
}

// Malloc
static uint64_t *syscall_malloc(uint64_t size) {
	return allocMemory(size);
}

// Free
static void syscall_free(void *ptr) {
	freeMemory(ptr);
	return;
}

static int checkParams(uint8_t priority, int16_t fileDescriptors[]) {
	driver_printStr("\nPriority: ", (Color) {0xAA, 0xFF, 0xFF});
	driver_printNum(priority, (Color) {0xAA, 0xFF, 0xFF});
	if (priority > MAX_PRIORITY) {
		driver_printStr("\n[Kernel]: ", (Color) {0xAA, 0xFF, 0xFF});
		driver_printStr("Error: Priority too high\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}

	if (fileDescriptors[STDIN] < 0 || fileDescriptors[STDOUT] < 0 ||
		fileDescriptors[STDERR] < 0) {
		driver_printStr("\n[Kernel]: ", (Color) {0xAA, 0xFF, 0xFF});
		driver_printStr("Error: Invalid file descriptor\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}
	return 1;
}

// Create process
static int16_t syscall_createProcess(MainFunction code, char **args, char *name,
									 uint8_t priority,
									 int16_t fileDescriptors[]) {
	driver_printStr("\nCreating process of Userland: ",
					(Color) {0xAA, 0xFF, 0xFF});
	driver_printStr(name, (Color) {0xAA, 0xFF, 0xFF});
	if (checkParams(priority, fileDescriptors) == -1) {
		return -1;
	}
	return createProcess(code, args, name, 1, fileDescriptors, 0);
}

// kill process
static uint64_t sys_kill_process(uint64_t pid, uint64_t retValue) {
	if (pid < 0) {
		return -1;
	}
	return killProcess(pid, retValue);
	// return 0;
}

static uint64_t sys_unblock(uint16_t pid) {
	return unblockProcess(pid);
}

static uint64_t sys_set_priority(uint64_t pid, uint64_t newPriority) {
	if (newPriority > MAX_PRIORITY) {
		return -1;
	}
	return setPriority(pid, newPriority);
}

static uint64_t sys_block(uint16_t pid) {
	return blockProcess(pid);
}

static uint64_t sys_getpid() {
	return getPid();
}

static uint64_t sys_yield() {
	yield();
	return 1;
}

static uint64_t (*sys_masters[])(uint64_t, uint64_t, uint64_t, uint64_t,
								 uint64_t) = {
	(void *) sys_read,				// 0
	(void *) sys_write,				// 1
	(void *) sys_write_color,		// 2
	(void *) sys_clear,				// 3
	(void *) sys_increment_size,	// 4
	(void *) sys_decrement_size,	// 5
	(void *) sys_getHours,			// 6
	(void *) sys_getMinutes,		// 7
	(void *) sys_getSeconds,		// 8
	(void *) sys_inforeg,			// 9
	(void *) sys_drawSquare,		// 10
	(void *) sys_sleep,				// 11
	(void *) sys_playSound,			// 12
	(void *) sys_stopSound,			// 13
	(void *) syscall_malloc,		// 14
	(void *) syscall_free,			// 15
	(void *) syscall_createProcess, // 16
	(void *) sys_kill_process,		// 17
	(void *) sys_set_priority,		// 18
	(void *) sys_unblock,			// 19
	(void *) sys_block,				// 20
	(void *) sys_getpid,			// 21
	(void *) sys_yield,				// 22
};

uint64_t sys_master(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10,
					uint64_t r8, uint64_t rax) {
	if (rax < sizeof(sys_masters) / sizeof(sys_masters[0]) &&
		sys_masters[rax] != 0) {
		return sys_masters[rax](rdi, rsi, rdx, r10, r8);
	}

	return 0;
}
