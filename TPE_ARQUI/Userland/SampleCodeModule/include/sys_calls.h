#ifndef _SYS_CALLS_H_
#define _SYS_CALLS_H_

#include <color.h>
#include <stdint.h>
#include <shared.h>

extern uint64_t u_sys_read(uint64_t fd, char *buf, uint64_t len);

extern uint64_t u_sys_write(uint64_t fd, char *buf, uint64_t len);

extern uint64_t u_sys_write_color(uint64_t fd, const char buf, Color color);

extern uint64_t u_sys_clear();

extern uint64_t u_sys_getHours();

extern uint64_t u_sys_getMinutes();

extern uint64_t u_sys_getSeconds();

extern uint64_t u_sys_increment_size();

extern uint64_t u_sys_decrement_size();

extern uint64_t u_sys_infoReg(uint64_t *registers);

extern uint64_t u_exc_invopcode();

extern uint64_t u_exc_zerodiv();

extern uint64_t u_sys_drawSquare(int x, int y, uint32_t fillColor);

extern uint64_t u_sys_sleep(int ms);

extern uint64_t u_sys_playSound(uint32_t frequence, uint64_t duration);

extern uint64_t u_sys_stopSound();

extern void *u_sys_malloc(uint64_t size);

extern uint64_t u_sys_free(void *address);

extern uint64_t u_sys_create_process(void *code, char **args, char *name,
									 uint8_t priority,
									 int16_t fileDescriptors[]);

extern uint64_t u_sys_kill_process(uint64_t pid);

extern uint64_t u_sys_unblock_process(uint64_t pid);

extern uint64_t u_sys_block_process(uint64_t pid);

extern uint64_t u_sys_set_prio(uint64_t pid, uint64_t newPrio);

extern uint64_t u_sys_get_pid();

extern uint64_t u_sys_yield();

extern uint64_t u_sys_wait_pid(uint16_t pid);

extern uint64_t u_sys_ps();

extern uint64_t u_sys_pipeOpen(uint16_t pid, uint8_t mode);

extern uint64_t u_sys_pipeClose(uint16_t pid);

extern uint64_t u_sys_getPipe();

extern uint64_t u_sys_sem_wait(uint16_t sem_id);

extern uint64_t u_sys_sem_post(uint16_t sem_id);

extern uint64_t u_sys_sem_open(uint16_t sem_id, uint64_t initialValue);

extern uint64_t u_sys_sem_close(uint16_t sem_id);

extern uint64_t u_sys_getMemoryInfo(MemoryInfo *info);

#endif