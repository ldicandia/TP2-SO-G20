#ifndef _PIPE_MANAGER_H
#define _PIPE_MANAGER_H

#include <stdint.h>

#include <globals.h>

typedef struct PipeManagerCDT *PipeManagerADT;

PipeManagerADT setupPipeManager();

int16_t requestNewPipeHandle();

int8_t acquirePipeAccess(uint16_t id, uint8_t mode);

int8_t grantPipeAccessToProcess(uint16_t pid, uint16_t id, uint8_t mode);

int8_t releasePipeAccess(uint16_t id);

int8_t revokePipeAccessFromProcess(uint16_t pid, uint16_t id);

int64_t retrievePipeData(uint16_t id, char *destinationBuffer, uint64_t len);

int64_t transmitPipeData(uint16_t pid, uint16_t id, char *sourceBuffer,
						 uint64_t len);

#endif