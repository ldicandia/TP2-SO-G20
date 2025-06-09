/*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */

#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <idtLoader.h>
#include <stdint.h>

void _irq_handler(void);

void _irq00handler(void);

void _exception0Handler(void);

void _interrupt_keyboardHandler();

void _interrupt_syscall(void);

void _cli(void);

void _sti(void);

void _hlt(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

void haltcpu(void);

void *_initialize_stack_frame(void *wrapper, void *rip, void *rsp, void *args);

#endif /* INTERRUPS_H_ */
