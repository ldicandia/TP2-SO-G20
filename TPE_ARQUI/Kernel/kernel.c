// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <idtLoader.h>
#include <keyboard.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <time.h>
#include <videoDriver.h>
#include <memoryManager.h>
#include <process.h>
#include <schedule.h>
#include <test_mm.h>
#include <test_processes.h>
#include <videoDriver.h>
#include <interrupts.h>

#define STDOUT 0
#define STDERR 1
#define STDIN 2

#define HEAP_SIZE 0x1000000 // 16MB CHEQUEAR ESTO
// #define HEAP_SIZE 0x100000 // 1MB
#define BLOCK_SIZE 0x1000

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

extern void _hlt();

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *) 0x400000;
static void *const sampleDataModuleAddress = (void *) 0x500000;
// static void *const memAmount			   = (void *) (SYSTEM_VARIABLES + 132);
static void *const memAmount = (void *) 0x600000;

static void *const shell_code_module_address =
	(void *) *(&sampleCodeModuleAddress);
static void *const shell_data_module_address =
	(void *) *(&sampleDataModuleAddress);
static void *const heap = (void *) *(&memAmount);

static MemoryManagerADT kernel_mem, userland_mem;
typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
	return (void *) ((uint64_t) &endOfKernel +
					 PageSize * 8		// The size of the stack itself, 32KiB
					 - sizeof(uint64_t) // Begin at the top of the stack
	);
}

void *initializeKernelBinary() {
	void *moduleAddresses[] = {shell_code_module_address,
							   shell_data_module_address};
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}

void idle_process() {
	while (1) {
		_hlt();
	}
}

MemoryManagerADT get_userland_mem() {
	return userland_mem;
}
MemoryManagerADT get_kernel_mem() {
	return kernel_mem;
}

char *new_str_copy(char *string) {
	if (string == NULL) {
		return NULL;
	}
	uint64_t len = strlen_lib(string) + 1;
	char *copy	 = allocMemory(len);
	if (copy == NULL) {
		return NULL;
	}
	memcpy(copy, string, len);
	return copy;
}

char **copy_argv(pid_t pid, char **argv, uint64_t argc) {
	if (argc == 0 || argv == NULL) {
		return NULL;
	}

	char **ans = allocMemory(sizeof(char *) * (argc + 1));

	if (ans == NULL) {
		return NULL;
	}

	for (uint64_t i = 0; i < argc; i++) {
		ans[i] = new_str_copy(argv[i]);
		if (ans[i] == NULL) {
			for (uint64_t j = 0; j < i; j++) {
				freeMemory((void *) ans[j]);
			}
			freeMemory((void *) ans);
			return NULL;
		}
	}
	return ans;
}

int main() {
	load_idt();
	kernel_mem		   = allocMemory((size_t) heap);
	userland_mem	   = allocMemory((size_t) heap + HEAP_SIZE);
	char *argv_idle[]  = {"idle"};
	char *argv_shell[] = {"sh"};
	fd_t idle_fds[3]   = {-1, -1, -1};
	fd_t shell_fds[3]  = {STDOUT, STDERR, STDIN};
	driver_printStr("AAAAAAAAAAAAAA\n", (Color) {0xAA, 0xFF, 0xFF});
	initialize_scheduler(new_process((main_function) shell_code_module_address,
									 HIGH, 0, argv_shell, 1, shell_fds),
						 new_process((main_function) idle_process, LOW, 0,
									 argv_idle, 1, idle_fds));
	driver_printStr("Kernel loaded\n", (Color) {0xAA, 0xFF, 0xFF});
	// pipe_init();
	init_timer_handler();
	// forceTimerTick();
	//__asm__("int $0x20");

	return 0;
}
