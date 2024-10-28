/* sampleCodeModule.c */
#include <userLibrary.h>
#include <shell.h>
#include <time.h>

char *v = (char *)0xB8000 + 79 * 2;
int started = 0;

// static int var1 = 0;
// static int var2 = 0;

Color CYAN = {0xFF, 0xFF, 0x00};

int main()
{
	printStrColor("\nITBA Shell Group 14\n", CYAN);
	printStrColor("ALFIERI - DI CANDIA - GERMANO\n", CYAN);
	shell();
	return 0;
}