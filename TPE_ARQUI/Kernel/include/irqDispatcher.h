#ifndef IRQ_DISPATCHER_H
#define IRQ_DISPATCHER_H

#include <stdint.h>

// Define the Color struct
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

void irqDispatcher(uint64_t irq);
void int_20();
uint64_t sys_read(uint64_t fd, char *buff);
uint64_t sys_write(uint64_t fd, char buffer);
uint64_t sys_write_color(uint64_t fd, char buffer, Color fnt);
uint64_t sys_clear();
static uint64_t sys_increment_size();
static uint64_t sys_decrement_size();

static uint64_t sys_getSeconds();

static uint64_t sys_getMinutes();

static uint64_t sys_getHours();

#endif // IRQ_DISPATCHER_H