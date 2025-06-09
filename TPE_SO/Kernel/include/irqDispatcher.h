#ifndef IRQ_DISPATCHER_H
#define IRQ_DISPATCHER_H

#include <stdint.h>
#include <videoDriver.h>

void irqDispatcher(uint64_t irq);

uint64_t sys_sleep(uint64_t ms);

#endif // IRQ_DISPATCHER_H