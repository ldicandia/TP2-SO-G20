#ifndef BUILTIN_H
#define BUILTIN_H

// Function declarations for builtin commands
void help();
void clear();
void notFound();

// Snake game functions
void shell_snake_1();
void shell_snake_2();

// Wrapper functions that return int (for command execution)
int _shell_snake_1(int argc, char **argv);
int _shell_snake_2(int argc, char **argv);
int _inforeg_wrap(int argc, char **argv);
int _zerodiv_wrap(int argc, char **argv);
int _invop_wrap(int argc, char **argv);
int _inc_wrap(int argc, char **argv);
int _dec_wrap(int argc, char **argv);

#endif // BUILTIN_H