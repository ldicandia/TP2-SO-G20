// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "time.h"

#include <stdint.h>

static unsigned long ticks = 0;
extern int _hlt();

void timer_master() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

int ms_elapsed() {
	return ticks * 5000 / 91;
}

void timer_wait(int delta) {
	int initialTicks = ticks;
	while (ticks - initialTicks < delta) {
		_hlt();
	}
}
