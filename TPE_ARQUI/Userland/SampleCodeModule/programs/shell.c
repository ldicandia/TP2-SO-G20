

#include "shell.h"

#include <snake.h>
#include <stdio.h>
#include <stdlib.h>
#include <user_time.h>
#include <test_mm.h>
#include <test_processes.h>
#include <test_prio.h>
#include <sys_calls.h>
#include <test_sync.h>

#include "userLibrary.h"
#define MAX_BUFFER 254
#define COMMANDS_SIZE 22
#define STDIN 0
#define STDOUT 1
#define STDERR 2

// Añadido: buffer para último comando y función de copia
static char last_command[MAX_BUFFER + 1] = {0};

static void copy_command(char *dest, const char *src) {
	int i = 0;
	while (src[i] && i < MAX_BUFFER) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

typedef int (*MainFunction)(int argc, char **args);

static void printLine(char c);
int linePos = 0;
char lastc;
int lastEnter			  = 0;
char line[MAX_BUFFER + 1] = {0};

char command[MAX_BUFFER + 1]   = {0};
char parameter[MAX_BUFFER + 1] = {0};

char buffer[MAX_BUFFER] = {0};

char *command_names[] = {
	"help",		  "time",		   "clear",		"snake 1",	 "snake 2",
	"inforeg",	  "zerodiv",	   "invopcode", "increment", "decrement",
	"testMemory", "testProcesses", "testPrio",	"testSync",	 "ps",
	"mem",		  "loop",		   "kill",		"nice",		 "block",
	"ws",		  "filter"};

int atoi(const char *str) {
	int res	 = 0;
	int sign = 1;

	if (*str == '-') {
		sign = -1;
		str++;
	}

	while (*str >= '0' && *str <= '9') {
		res = res * 10 + (*str - '0');
		str++;
	}

	return sign * res;
}

// 1) Creamos los wrappers para los comandos “externos”:
static int _shell_snake_1(int argc, char **argv) {
	snake(1);
	return 0;
}
static int _shell_snake_2(int argc, char **argv) {
	snake(2);
	return 0;
}
static int _inforeg_wrap(int argc, char **argv) {
	inforeg();
	return 0;
}
static int _zerodiv_wrap(int argc, char **argv) {
	exc_zerodiv();
	return 0;
}
static int _invop_wrap(int argc, char **argv) {
	exc_invopcode();
	return 0;
}
static int _inc_wrap(int argc, char **argv) {
	increment_size_char();
	return 0;
}
static int _dec_wrap(int argc, char **argv) {
	decrement_size_char();
	return 0;
}
static int _ps_wrap(int argc, char **argv) {
	ps(); // asume que existe ps()
	return 0;
}
static int _mem_wrap(int argc, char **argv) {
	MemoryInfo info;
    if (user_mem(&info) != 0) {//usa user_mem
        printStr("Error obteniendo info de memoria\n");
        return 1;
    }
    printStr("\n[Memory Info]\n");
    printStr("Total: "); printInteger(info.totalMemory); printStr(" bytes\n");
    printStr("Usada: "); printInteger(info.usedMemory); printStr(" bytes\n");
    printStr("Libre: "); printInteger(info.freeMemory); printStr(" bytes\n");
    printStr("Bloques usados: "); printInteger(info.usedBlocks); printStr("\n");
    printStr("Bloques libres: "); printInteger(info.freeBlocks); printStr("\n");
    printStr("Bloques totales: "); printInteger(info.totalBlocks); printStr("\n");
	return 0;
}
static int _loop_wrap(int argc, char **argv) {
	// infiniteLoop(0, NULL);
	return 0;
}
static int _kill_wrap(int argc, char **argv) {
	if (argc < 2) {
		printStr("\nUsage: kill <pid>");
		return -1;
	}
	kill_process(atoi(argv[1]));
	return 0;
}
static int _nice_wrap(int argc, char **argv) {
	if (argc < 3) {
		printStr("\nUsage: nice <pid> <prio>");
		return -1;
	}
	set_prio(atoi(argv[1]), atoi(argv[2]));
	return 0;
}
static int _block_wrap(int argc, char **argv) {
	if (argc < 2) {
		printStr("\nUsage: block <pid>");
		return -1;
	}
	block(atoi(argv[1]));

	return 0;
}
static int _ws_wrap(int argc, char **argv) {
	// ws(); // muestra working set
	return 0;
}

static int filter(int argc, char **argv) { // filter vowels
	printStr("\nFilter vowels: ");
	char c;
	char lastChar = 0;
	while ((c = getCharInt()) != EOF) {
		if (lastChar != c &&
			!(c == 'a' || c == 'A' || c == 'e' || c == 'E' || c == 'i' ||
			  c == 'I' || c == 'o' || c == 'O' || c == 'u' || c == 'U')) {
			printChar(c);
			lastChar = c;
		}
	}

	// si fue '\n', lo imprimimos y volvemos al shell
	if (c == '\n') {
		printChar('\n');
	}
	return 0;
}

// 2) Cambiamos el array de handlers para que apunte a MainFunction:
MainFunction command_func[COMMANDS_SIZE] = {(MainFunction) help, // built-ins
											(MainFunction) user_time,
											(MainFunction) clear,
											_shell_snake_1,
											_shell_snake_2,
											_inforeg_wrap,
											_zerodiv_wrap,
											_invop_wrap,
											_inc_wrap,
											_dec_wrap,
											(MainFunction) test_mm,
											(MainFunction) test_processes,
											(MainFunction) test_prio,
											(MainFunction) test_sync,
											_ps_wrap,
											_mem_wrap,
											_loop_wrap,
											_kill_wrap,
											_nice_wrap,
											_block_wrap,
											_ws_wrap,
											(MainFunction) filter};

void infiniteLoop(uint64_t argc, char *argv[]) {
	while (1) {
		printChar('a');
		yield();
	}
}

void test_prio_wrapper(uint64_t argc, char *argv[]) {
	argc		= 1;
	int64_t pid = test_prio();
	if (pid == -1) {
		printStr("\nError creating processes test\n");
	}
	else {
		printStr("\nProcesses test created with PID: ");
		printInteger(pid);
		printStr("\n");
	}
}

void endless_A(int argc, char **argv) {
	while (1) {
		printStr("A");
		for (int i = 0; i < 100000000; i++)
			;
		yield(); // Cede el control manualmente
	}
}

void endless_B(int argc, char **argv) {
	while (1) {
		printStr("B");
		for (int i = 0; i < 100000000; i++)
			;
		yield(); // No hace yield, depende del quantum para ser expulsado
	}
}

static int strlen(const char *str) {
	int len = 0;
	while (str[len] != '\0') {
		len++;
	}
	return len;
}

#define READ 0
#define WRITE 1

#define TEST_PIPE 200
static int testWriter(int argc, char **argv) {
	pipeOpen(TEST_PIPE, WRITE);
	char *message = "***SECRET MESSAGE FROM WRITER***                   ";
	printStr("Soy el escritor, enviando mensaje...\n");
	u_sys_write(TEST_PIPE, message, strlen(message) + 1);
	pipeClose(TEST_PIPE);
	return 0;
}

#define TEST_BUFFER_LEN 100
static int testReader(int argc, char **argv) {
	pipeOpen(TEST_PIPE, READ);
	char received[TEST_BUFFER_LEN] = {0};
	u_sys_read(TEST_PIPE, received, TEST_BUFFER_LEN);
	printStr("Soy el lector, mensaje:");
	printStr(received);
	printStr("\n");
	pipeClose(TEST_PIPE);
	return 0;
}

int testNamedPipes(int argc, char **argv) {
	char *paramsReader[] = {"test_reader", NULL};
	uint16_t pidReader =
		create_process((void *) &testReader, paramsReader, "test_reader", 4);
	char *paramsWriter[] = {"test_writer", NULL};
	uint16_t pidWriter =
		create_process((void *) &testWriter, paramsWriter, "test_writer", 4);
	wait_pid(pidReader);
	wait_pid(pidWriter);
	return 0;
}

void shell() {
	printStrColor("\nITBA Shell Group 20\n", (Color) {0xFF, 0xFF, 0x00});
	printStrColor("ALFIERI - DI CANDIA - DIAZ VARELA\n",
				  (Color) {0xFF, 0xFF, 0x00});
	char c;

	printStr("\n--------------------| SHELL |--------------------\n");

	// testProcesses();

	char *argsTest[] = {"test_sync", NULL};

	// create_process((MainFunction) test_sync, argsTest, "test_sync", 4);

	// test_sync(3, (char *[]) {"test", "1000", "1", NULL});

	// char *argsA[] = {"endless_loopA", NULL};
	// char *argsB[] = {"endless_loopB", NULL};

	// int pid = create_process(endless_A, argsA, "print_A", 4); // prioridad
	// alta create_process(endless_B, argsB, "print_B", 0);			  //
	// prioridad media

	// char *argsTest[] = {"test_named_pipes", NULL};

	// create_process(testNamedPipes, argsTest, "test_named_pipes", 4);

	while (1) {
		c = getChar();
		if (lastc != c) {
			printLine(c);
		}
	}
}

static void printLine(char c) {
	// Si se escribe '=' al inicio, recupero el último comando
	if (c == '=' && lastEnter == 0 && last_command[0] != '\0') {
		int i = 0;
		while (last_command[i]) {
			buffer[lastEnter++] = last_command[i];
			printChar(last_command[i]);
			i++;
		}
		lastc = c;
		return;
	}

	if (isChar(c) || isDigit(c) || c == ' ' || c == '|' || c == '&') {
		buffer[lastEnter] = c;
		lastEnter++;
	}
	if (c == '\n') {
		buffer[lastEnter] = '\0';

		readCommand();

		buffer[0] = '\0';
		printStr(buffer);
		lastEnter = 0;
	}

	if (c == '\b' && lastEnter > 0) {
		lastEnter--;
		buffer[lastEnter] = '\0';
	}
	else if (c == '\b' && lastEnter == 0) {
		lastc = c;
		return;
	}

	printChar(c);
	lastc = c;
}

void help() {
	static Color WHITE = {0xFF, 0xFF, 0xFF};
	printStrColor("\n--------------------| MANUAL |--------------------",
				  WHITE);
	printStrColor("\n time                     shows actual time", WHITE);
	printStrColor("\n clear                    clears the screen", WHITE);
	printStrColor("\n snake [qty players]      starts de Snake game", WHITE);
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
	// nuevos comandos
	printStrColor("\n ps                       list processes", WHITE);
	printStrColor("\n mem                      show memory info", WHITE);
	printStrColor("\n loop                     launch infinite loop", WHITE);
	printStrColor("\n kill <pid>               terminate a process", WHITE);
	printStrColor("\n nice <pid> <prio>        change process priority", WHITE);
	printStrColor("\n block <pid>              block a process", WHITE);
	printStrColor("\n ws                       show working set", WHITE);
	printStrColor("\n filter                   filter vowels from input",
				  WHITE);
	printStrColor("\n---------------------------------------------------",
				  WHITE);
}

static int strcmp_shell(const char *str, const char *cmd) {
	int i = 0;
	// comparamos mientras cmd no termine
	while (cmd[i] != '\0') {
		if (str[i] != cmd[i]) {
			return 0;
		}
		i++;
	}
	// tras coincidir todo cmd, str[i] debe ser espacio o '\0'
	return (str[i] == ' ' || str[i] == '\0');
}

static int strchr(const char *str, char c) {
	while (*str) {
		if (*str == c) {
			return 1;
		}
		str++;
	}
	return 0;
}

static char *strchrAndReturn(const char *str, char c) {
	while (*str) {
		if (*str == c) {
			return (char *) str;
		}
		str++;
	}
	return NULL;
}

static char *strtok(char *str, const char *delim) {
	static char *last = NULL;
	if (str == NULL) {
		str = last;
	}
	if (str == NULL) {
		return NULL;
	}
	while (*str && strchr(delim, *str)) {
		str++;
	}
	if (*str == '\0') {
		last = NULL;
		return NULL;
	}
	char *start = str;
	while (*str && !strchr(delim, *str)) {
		str++;
	}
	if (*str) {
		*str++ = '\0';
	}
	last = str;
	return start;
}

void readCommand() {
	if (buffer[0] != '\0') {
		copy_command(last_command, buffer);
	}
	if (strchr(buffer, '|')) {
		// split on '|'
		char *bar		= strchrAndReturn(buffer, '|');
		*bar			= '\0';
		char *left_cmd	= buffer;
		char *right_cmd = bar + 1;
		while (*right_cmd == ' ')
			right_cmd++;

		// parse each side into argv arrays
		char *argv1[5] = {NULL}, *argv2[5] = {NULL};
		int argc1 = 0, argc2 = 0;
		char *tok;
		// left side
		tok = strtok(left_cmd, " ");
		while (tok && argc1 < 4) {
			argv1[argc1++] = tok;
			tok			   = strtok(NULL, " ");
		}
		argv1[argc1] = NULL;
		// right side
		tok = strtok(right_cmd, " ");
		while (tok && argc2 < 4) {
			argv2[argc2++] = tok;
			tok			   = strtok(NULL, " ");
		}
		argv2[argc2] = NULL;

		// locate handlers by name
		int i1 = -1, i2 = -1;
		for (int i = 0; i < COMMANDS_SIZE; i++) {
			if (strcmp_shell(argv1[0], command_names[i]))
				i1 = i;
			if (strcmp_shell(argv2[0], command_names[i]))
				i2 = i;
		}
		if (i1 < 0 || i2 < 0) {
			notFound();
			return;
		}

		// create a pipe in the kernel (your own syscall, not libc pipe())
		int pipeId = getPipe();

		printStr("\nSpawning left command: ");
		printStr(command_names[i1]);
		int16_t fileDescriptors1[] = {0, pipeId, 2};
		// spawn left, redirect its stdout → fds[1]
		int pid1 = create_process_with_fds(
			command_func[i1], argv1, command_names[i1], 1, fileDescriptors1);

		printStr("\nSpawning right command: ");
		printStr(command_names[i2]);
		int16_t fileDescriptors2[] = {pipeId, 1, 2};
		// spawn right, redirect its stdin ← fds[0]
		int pid2 = create_process_with_fds(
			command_func[i2], argv2, command_names[i2], 1, fileDescriptors2);

		// // close both ends in the shell
		pipeClose(pid1);
		pipeClose(pid2);

		// wait both
		wait_pid(pid1);
		wait_pid(pid2);
		return;
	}

	for (int i = 0; i < COMMANDS_SIZE; i++) {
		if (strcmp_shell(buffer, command_names[i])) {
			// built-ins (índices 0,1,2)
			if (i <= 2) {
				command_func[i](0, NULL);
			}
			else {
				// parseo muy sencillo de argumentos
				char *argv[4] = {NULL};
				int argc	  = 0;
				char *tok	  = strtok(buffer, " ");
				while (tok && argc < 3) {
					argv[argc++] = tok;
					tok			 = strtok(NULL, " ");
				}
				argv[argc] = NULL;
				int pid =
					create_process(command_func[i], argv, command_names[i], 1);
				wait_pid(pid); // Espera a que el proceso termine
			}
			return;
		}
	}
	notFound();
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

