#ifndef _SYS_CALLS_H_
#define _SYS_CALLS_H_

#include <color.h>
#include <stdint.h>

extern uint64_t u_sys_read(uint64_t fd, char *buf);

extern uint64_t u_sys_write(uint64_t fd, const char buf);

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

#endif