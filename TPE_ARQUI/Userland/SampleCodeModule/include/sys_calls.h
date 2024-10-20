#ifndef _SYS_CALLS_H_
#define _SYS_CALLS_H_

#include <stdint.h>

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} Color;


extern uint64_t u_sys_read(uint64_t fd, char *buf);

extern uint64_t u_sys_write(uint64_t fd, const char buf);

extern uint64_t u_sys_write_color(uint64_t fd, const char buf, Color color);

#endif