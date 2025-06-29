// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process_commands.h>
#include <userLibrary.h>
#include <builtin.h>
#include <phylo.h>
#include <string.h>
#include <test_prio.h>

#define EOF (-1)

static int atoi(const char *str) {
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

int _ps_wrap(int argc, char **argv) {
	ps();
	return 0;
}
int _mem_wrap(int argc, char **argv) {
	MemoryInfo info;
	if (user_mem(&info) != 0) {
		printStr("Error obteniendo info de memoria\n");
		return 1;
	}
	printStr("\n[Memory Info]\n");
	printStr("Total: ");
	printInteger(info.totalMemory);
	printStr(" bytes\n");
	printStr("Usada: ");
	printInteger(info.usedMemory);
	printStr(" bytes\n");
	printStr("Libre: ");
	printInteger(info.freeMemory);
	printStr(" bytes\n");
	printStr("Bloques usados: ");
	printInteger(info.usedBlocks);
	printStr("\n");
	printStr("Bloques libres: ");
	printInteger(info.freeBlocks);
	printStr("\n");
	printStr("Bloques totales: ");
	printInteger(info.totalBlocks);
	printStr("\n");
	return 0;
}
int loop(int argc, char **argv) {
	if (argc < 2) {
		printStr("\nUsage: loop <miliseconds>");
		return -1;
	}
	printStr("\ninicio PID: ");
	printInteger(getpid());
	printStr("\n");
	while (1) {
		printStr("Hola desde PID: ");
		printInteger(getpid());
		printStr("\n");
		sleep_miliseconds(atoi(argv[1]));
	}
	return 0;
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

int _kill_wrap(int argc, char **argv) {
	if (argc < 2) {
		printStr("\nUsage: kill <pid>");
		return -1;
	}
	kill_process(atoi(argv[1]));
	return 0;
}
int _nice_wrap(int argc, char **argv) {
	if (argc < 3) {
		printStr("\nUsage: nice <pid> <prio>");
		return -1;
	}
	set_prio(atoi(argv[1]), atoi(argv[2]));
	return 0;
}
int _block_wrap(int argc, char **argv) {
	if (argc < 2) {
		printStr("\nUsage: block <pid>");
		return -1;
	}
	block(atoi(argv[1]));

	return 0;
}

int _unblock_wrap(int argc, char **argv) {
	if (argc < 2) {
		printStr("\nUsage: unblock <pid>");
		return -1;
	}
	unblock(atoi(argv[1]));

	return 0;
}
int wc(int argc, char **argv) {
	int lineCount = 0;
	int c;

	while ((c = getCharInt()) != EOF) {
		if (c == '\n') {
			lineCount++;
		}
		printChar(c);
	}
	printStr("\nLine count: ");
	printInteger(lineCount);
	printStr("\n");
	return 0;
}

int filter(int argc, char **argv) {
	printStr("\nFilter vowels: ");
	int c;
	char lastChar = 0;
	while ((c = getCharInt()) != EOF) {
		if (lastChar != c &&
			!(c == 'a' || c == 'A' || c == 'e' || c == 'E' || c == 'i' ||
			  c == 'I' || c == 'o' || c == 'O' || c == 'u' || c == 'U')) {
			printChar(c);
			lastChar = c;
		}
	}

	printStr("Ending Filter");
	printChar('\n');
	return 0;
}

int cat(int argc, char **argv) {
	int c;
	while ((c = getCharInt()) != EOF) {
		printChar(c);
	}
	return 0;
}

int red(int argc, char **argv) {
	int c;
	while ((c = getCharInt()) != EOF) {
		printCharColor(c, (Color) {0x00, 0x00, 0xFF});
	}
	return 0;
}

int phylo_wrap(int argc, char **argv) {
	phylo();
	return 0;
}