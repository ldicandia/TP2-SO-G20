#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdint.h>
#include <process.h>

#define MAX_PROCESSES (1 << 12)
#define QTY_READY_LEVELS 5
#define MAX_PRIORITY 4
#define MIN_PRIORITY 0
#define BLOCKED_INDEX QTY_READY_LEVELS
#define IDLE_PID 0
#define QUANTUM_COEF 2

typedef struct SchedulerCDT *SchedulerADT;

// Crea e inicializa el planificador
SchedulerADT createScheduler();

// Obtiene la instancia del planificador
SchedulerADT getSchedulerADT();

// Cambia la prioridad de un proceso
int32_t setPriority(uint16_t pid, uint8_t newPriority);

// Cambia el estado de un proceso
int8_t setStatus(uint16_t pid, uint8_t newStatus);

// Obtiene el estado de un proceso
ProcessStatus getProcessStatus(uint16_t pid);

// Planifica el siguiente proceso a ejecutar
void *schedule(void *prevStackPointer);

// Crea un nuevo proceso
uint16_t createProcess(MainFunction code, char **args, char *name,
					   uint8_t priority, int16_t fileDescriptors[]);

// Mata el proceso actual
int32_t killCurrentProcess(int32_t retValue);

// Mata un proceso específico
int32_t killProcess(uint16_t pid, int32_t retValue);

// Obtiene el PID del proceso actual
uint16_t getpid();

// Obtiene un snapshot de los procesos
// ProcessSnapshotList *getProcessSnapshot();

// Obtiene el valor de retorno de un proceso zombie
int32_t getZombieRetValue(uint16_t pid);

// Verifica si un proceso está vivo
int32_t processIsAlive(uint16_t pid);

// Fuerza la entrega del control al planificador
void yield();

// Cambia un descriptor de archivo de un proceso
int8_t changeFD(uint16_t pid, uint8_t position, int16_t newFd);

// Obtiene un descriptor de archivo del proceso actual
int16_t getCurrentProcessFileDescriptor(uint8_t fdIndex);

// Marca el proceso en primer plano para ser terminado
void killForegroundProcess();

int32_t killCurrentProcess(int32_t retValue);

int32_t killProcess(uint16_t pid, int32_t retValue);

//=========== SETTERS AND GETTERS ===========//

uint16_t getPid();

#endif // SCHEDULE_H