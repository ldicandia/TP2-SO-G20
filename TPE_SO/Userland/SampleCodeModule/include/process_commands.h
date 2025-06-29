#ifndef PROCESS_COMMANDS_H
#define PROCESS_COMMANDS_H

#include <stdint.h>

int _ps_wrap(int argc, char **argv);
int _mem_wrap(int argc, char **argv);
int _kill_wrap(int argc, char **argv);
int _nice_wrap(int argc, char **argv);
int _block_wrap(int argc, char **argv);
int _unblock_wrap(int argc, char **argv);

int loop(int argc, char **argv);
int wc(int argc, char **argv);
int filter(int argc, char **argv);
int cat(int argc, char **argv);
int red(int argc, char **argv);

int phylo_wrap(int argc, char **argv);

void test_prio_wrapper(uint64_t argc, char *argv[]);

#endif // PROCESS_COMMANDS_H