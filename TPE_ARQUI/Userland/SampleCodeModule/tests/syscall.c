// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <userLibrary.h>

int64_t my_getpid() {
	return 0;
}

int64_t my_create_process(char *name, uint64_t argc, char *argv[]) {
	return 0;
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
	return set_prio(pid, newPrio);
}

int64_t my_kill(uint64_t pid) {
	return kill_process(pid);
}

int64_t my_block(uint64_t pid) {
	return block(pid);
}

int64_t my_unblock(uint64_t pid) {
	return unblock(pid);
}

