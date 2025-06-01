#ifndef USER_LIBRARY_H
#define USER_LIBRARY_H

#include <color.h>
#include <stdint.h>
#include <userLibrary.h>
#include <shared.h>

typedef int (*MainFunction)(int argc, char **args);

void printChar(char c);

char getChar();

void printCharColor(char c, Color fnt);

void printStrColor(char *str, Color fnt);

void printStr(char *str);

void printInteger(int n);

int isChar(char c);

int isDigit(char c);

void increment_size_char();

void decrement_size_char();

void exc_invopcode();

void exc_zerodiv();

void clear();

void inforeg();

void printHex(uint64_t n);

void uint64ToHex(uint64_t n, char buf[16]);

void drawSquare(int x, int y, uint32_t fillColor);

void sleep_miliseconds(int miliseconds);

void playSound(uint32_t frequence, uint64_t duration);

void stopSound();

void *allocMemory(uint64_t size);

void freeMemory(void *address);

int create_process(MainFunction code, char **args, char *name,
				   uint8_t priority);

int create_process_with_fds(MainFunction code, char **args, char *name,
							uint8_t priority, int16_t fileDescriptors[]);

int kill_process(uint64_t pid);

int set_prio(uint64_t pid, uint64_t newPrio);

int block(uint64_t pid);

int unblock(uint64_t pid);

int getpid();

int yield();

int wait_pid(uint16_t pid);

int ps();

int pipeOpen(uint16_t pid, uint8_t mode);

int pipeClose(uint16_t pid);

int getPipe();

int getCharInt();

int user_sem_open(uint16_t sem_id, uint64_t initialValue);
int user_sem_wait(uint16_t sem_id);
int user_sem_post(uint16_t sem_id);
int user_sem_close(uint16_t sem_id);

int user_mem(MemoryInfo *info);

#endif // USER_LIBRARY_H