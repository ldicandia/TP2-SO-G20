
#include <userLibrary.h>
#include <builtin.h>
#include <sys_calls.h>
#include <snake.h>

void help() {
	static Color WHITE = {0xFF, 0xFF, 0xFF};
	printStrColor("\n--------------------| MANUAL |--------------------",
				  WHITE);
	printStrColor("\n time                     shows actual time", WHITE);
	printStrColor("\n clear                    clears the screen", WHITE);
	printStrColor(
		"\n inforeg                  after SHIFT + TAB prints regsiter's",
		WHITE);
	printStrColor("\n zerodiv                  division by zero exception test",
				  WHITE);
	printStrColor(
		"\n invopcode                tests the invalid operation code", WHITE);
	printStrColor("\n increment                increase letter size", WHITE);
	printStrColor("\n decrement                decrease letter size", WHITE);
	printStrColor("\n testMemory [size]        tests memory allocation", WHITE);
	printStrColor("\n testProcesses [size]     tests process creation", WHITE);
	printStrColor("\n testPrio                 tests process priority", WHITE);
	printStrColor(
		"\n testSync [n] [use_sem]   tests synchronization, 0 < n < 50", WHITE);
	printStrColor("\n ps                       list processes", WHITE);
	printStrColor("\n mem                      show memory info", WHITE);
	printStrColor("\n loop [sleep ms]          launch infinite loop", WHITE);
	printStrColor("\n kill [pid]               terminate a process", WHITE);
	printStrColor("\n nice [pid] [prio]        change process priority", WHITE);
	printStrColor("\n block [pid]              block a process", WHITE);
	printStrColor("\n filter                   filter vowels from input",
				  WHITE);
	printStrColor("\n cat                      prints STDIN", WHITE);
	printStrColor("\n wc                       counts input lines", WHITE);
	printStrColor("\n---------------------------------------------------",
				  WHITE);
}

void notFound() {
	printStr("\n command not found. Try using 'help'");
	return;
}

void shell_snake_1() {
	snake(1);
}

void shell_snake_2() {
	snake(2);
}

int _shell_snake_1(int argc, char **argv) {
	snake(1);
	return 0;
}
int _shell_snake_2(int argc, char **argv) {
	snake(2);
	return 0;
}
int _inforeg_wrap(int argc, char **argv) {
	inforeg();
	return 0;
}
int _zerodiv_wrap(int argc, char **argv) {
	exc_zerodiv();
	return 0;
}
int _invop_wrap(int argc, char **argv) {
	exc_invopcode();
	return 0;
}
int _inc_wrap(int argc, char **argv) {
	increment_size_char();
	return 0;
}
int _dec_wrap(int argc, char **argv) {
	decrement_size_char();
	return 0;
}