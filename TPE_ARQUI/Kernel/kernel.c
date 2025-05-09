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
static void *const memAmount			   = (void *) (SYSTEM_VARIABLES + 132);

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
	void *moduleAddresses[] = {sampleCodeModuleAddress,
							   sampleDataModuleAddress};
	uint64_t userlandSize	= loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	uint64_t memAmountBytes = *((uint32_t *) memAmount) * (1 << 20);

	uint64_t availableMem =
		1 << log(memAmountBytes -
					 (uint64_t) (sampleDataModuleAddress + userlandSize),
				 2);

	createMemoryManager((void *) MEMORY_MANAGER_ADDRESS,
						sampleDataModuleAddress + userlandSize, availableMem);
	createScheduler();

	return getStackBase();
}

int idle(int argc, char **argv) {
	char *argsShell[2]		  = {"shell", NULL};
	int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};
	int16_t pid = createProcess((MainFunction) sampleCodeModuleAddress,
								argsShell, "shell", 4, fileDescriptors);
	if (pid == -1) {
		driver_printStr("Error creating shell process\n",
						(Color) {0xFF, 0x00, 0x00});
		return -1;
	}

	while (1)
		_hlt();
	return 0;
}

int main() {
	char *argsIdle[3] = {"idle", "Hm?", NULL};

	createProcess((MainFunction) &idle, argsIdle, "init", 4, NULL);
	load_idt();
	clearScanCode();
	((EntryPoint) sampleCodeModuleAddress)();

	while (1)
		_hlt();

	return 0;
}
