// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <idtLoader.h>
#include <keyboard.h>
#include <lib.h>
#include <moduleLoader.h>
#include <stdint.h>
#include <time.h>
#include <videoDriver.h>
#include <memoryManager.h>
#include <process.h>
#include <schedule.h>
#include <semaphoreManager.h>
#include <test_processes.h>
#include <interrupts.h>
#include <pipeManager.h>
#include <globals.h>

#define SAMPLE_CODE_MODULE_ADDR 0x400000UL
#define SAMPLE_DATA_MODULE_ADDR 0x500000UL
#define MEM_AMOUNT_OFFSET 132UL

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress =
	(void *) (uintptr_t) SAMPLE_CODE_MODULE_ADDR; //-V566
static void *const sampleDataModuleAddress =
	(void *) (uintptr_t) SAMPLE_DATA_MODULE_ADDR; //-V566
static void *const memAmount =
	(void *) (uintptr_t) (SYSTEM_VARIABLES + MEM_AMOUNT_OFFSET); //-V566

typedef int (*EntryPoint)();
int idle(int argc, char **argv);

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
	return (void *) ((uint64_t) &endOfKernel + PageSize * 8 - sizeof(uint64_t));
}

void initializeKernelBinary() {
	void *moduleAddresses[] = {sampleCodeModuleAddress,
							   sampleDataModuleAddress};
	uint64_t userlandSize	= loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	uint64_t memAmountBytes = *((uint32_t *) memAmount) * (1 << 20);

	uint64_t availableMem =
		1L << log(memAmountBytes -
					  (uint64_t) (sampleDataModuleAddress + userlandSize),
				  2);

	createMemoryManager((void *) MEMORY_MANAGER_ADDRESS,
						sampleDataModuleAddress + userlandSize, availableMem);
	createScheduler();
	initSemaphores();
	initializeKeyboard();
	setupPipeManager();
}

int idle(int argc, char **argv);

int main() {
	char *argsIdle[3]		  = {"idle", "Hm?", NULL};
	int16_t fileDescriptors[] = {DEV_NULL, DEV_NULL, STDERR};
	createProcess((MainFunction) &idle, argsIdle, "idle", 0, fileDescriptors,
				  1);
	load_idt();
	return 0;
}

int idle(int argc, char **argv) {
	char *argsShell[2]		  = {"shell", NULL};
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};
	createProcess((MainFunction) sampleCodeModuleAddress, argsShell, "shell", 0,
				  fileDescriptors, 1);
	while (1) {
		_hlt();
	}
	return 0;
}