/* sampleCodeModule.c */
#include <shell.h>
#include <time.h>
#include <userLibrary.h>

char *v		= (char *) 0xB8000 + 79 * 2;
int started = 0;

Color CYAN = {0xFF, 0xFF, 0x00};

int main() {
	printStrColor("\nITBA Shell Group 20\n", CYAN);
	printStrColor("ALFIERI - DI CANDIA - DIAZ VARELA\n", CYAN);
	shell();
	return 0;
}