#include <stdint.h>
#include <string.h>
#include "lib.h"
#include <moduleLoader.h>
#include <naiveConsole.h>
#include "videoDriver.h"
#include "keyboard.h"
#include "idtLoader.h"
#include "time.h"
#include "interrupts.h"



extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

extern void _hlt();

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	// char buffer[10];
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}


// void write (){

//     char i = 0, ni = 0;
// 	while (1) {
//         ni = getCharFromKeyboard();

//         if (ni != 0) {
//             if (ni != i){
//                 dv_print(ni, WHITE, BLACK);
//             }
//         }
//         i = ni;
// 	}
// }

// void loop(){
//    int s, ns;
//    s = seconds_elapsed();
//    while (1){
//        ns = seconds_elapsed();
//        if (ns != s){
//            dv_printDec(ns,WHITE,BLACK);
//            dv_newline();
//        }
//        s = ns;
//    }
// }


int main()
{	
	load_idt();
	// dv_prints("bienvenidos al kernel\n",RED,BLACK, 22);

	// int start_ms = ms_elapsed();
    // do { _hlt(); } while (ms_elapsed() - start_ms < 1500);
    // dv_prints("\nno me importa nada",WHITE,BLACK,19);

	// int start_ms2 = ms_elapsed();
    // do { _hlt(); } while (ms_elapsed() - start_ms2 < 1500);
    // uint64_t fd = 1;

    //loop();

	clearScanCode();
    ((EntryPoint)sampleCodeModuleAddress)();

    while(1) _hlt();
    return 0;
}

