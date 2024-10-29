#ifndef IRQ_DISPATCHER_H
#define IRQ_DISPATCHER_H

#include <stdint.h>
#include <videoDriver.h>
// Define the Color struct

/*
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} Color;
*/
void irqDispatcher(uint64_t irq);

uint64_t sys_sleep(uint64_t ms);

#endif  // IRQ_DISPATCHER_H