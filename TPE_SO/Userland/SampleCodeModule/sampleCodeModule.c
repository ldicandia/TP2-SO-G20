// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* sampleCodeModule.c */
#include <shell.h>
#include <time.h>
#include <userLibrary.h>

char *v		= (char *) 0xB8000 + 79 * 2;
int started = 0;

Color CYAN = {0xFF, 0xFF, 0x00};

int main() {
	shell();
	return 0x000CAC71;
}