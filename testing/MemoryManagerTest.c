// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../TPE_ARQUI/Kernel/include/memoryManager.h"
#include "./include/CuTest.h"
#include "./include/MemoryManagerTest.h"

#define MANAGED_MEMORY_SIZE 20480
#define ALLOCATION_SIZE 1024
#define WRITTEN_VALUE 'a'

void testAllocMemory(CuTest *const cuTest);
void testTwoAllocations(CuTest *const cuTest);
void testWriteMemory(CuTest *const cuTest);

static const size_t TestQuantity = 3;
static const Test MemoryManagerTests[] = {testAllocMemory, testTwoAllocations,
                                          testWriteMemory};

static inline void givenAMemoryManager(CuTest *const cuTest);
static inline void givenAMemoryAmount(void);
static inline void givenAnAllocation(void);

static inline void whenMemoryIsAllocated(void);
static inline void whenMemoryIsWritten(void);

static inline void thenSomeMemoryIsReturned(CuTest *const cuTest);
static inline void thenTheTwoAdressesAreDifferent(CuTest *const cuTest);
static inline void thenBothDoNotOverlap(CuTest *const cuTest);
static inline void thenMemorySuccessfullyWritten(CuTest *const cuTest);

static MemoryManagerADT memoryManager;

static size_t memoryToAllocate;

static void *allocatedMemory = NULL;
static void *anAllocation = NULL;

CuSuite *getMemoryManagerTestSuite(void) {
  CuSuite *const suite = CuSuiteNew();

  for (size_t i = 0; i < TestQuantity; i++)
    SUITE_ADD_TEST(suite, MemoryManagerTests[i]);

  return suite;
}

void testAllocMemory(CuTest *const cuTest) {
  givenAMemoryManager(cuTest);
  givenAMemoryAmount();

  whenMemoryIsAllocated();

  thenSomeMemoryIsReturned(cuTest);
}

void testTwoAllocations(CuTest *const cuTest) {
  givenAMemoryManager(cuTest);
  givenAMemoryAmount();
  givenAnAllocation();

  whenMemoryIsAllocated();

  thenSomeMemoryIsReturned(cuTest);
  thenTheTwoAdressesAreDifferent(cuTest);
  thenBothDoNotOverlap(cuTest);
}

void testWriteMemory(CuTest *const cuTest) {
  givenAMemoryManager(cuTest);
  givenAMemoryAmount();
  givenAnAllocation();

  whenMemoryIsWritten();

  thenMemorySuccessfullyWritten(cuTest);
}

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;
static void *const memAmount = (void *)(SYSTEM_VARIABLES + 132);

inline void givenAMemoryManager(CuTest *const cuTest) {
  void *managedMemory = malloc(MANAGED_MEMORY_SIZE);
  if (managedMemory == NULL) {
    CuFail(cuTest, "[givenAMemoryManager] Managed Memory cannot be null");
  }

  uint64_t userlandSize = 1000;

  uint64_t availableMem = 1000;

  void *memory = (void *)((uintptr_t)sampleDataModuleAddress + userlandSize);

  memoryManager =
      createMemoryManager((void *)MEMORY_MANAGER_ADDRESS, memory, availableMem);

  if (memoryManager == NULL) {
    CuFail(cuTest, "[givenAMemoryManager] Failed to create Memory Manager");
  }
}

inline void givenAMemoryAmount(void) { memoryToAllocate = ALLOCATION_SIZE; }

inline void givenAnAllocation(void) {
  anAllocation = allocMemory(memoryToAllocate);
  if (anAllocation == NULL) {
    fprintf(stderr, "Error: Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

inline void whenMemoryIsAllocated(void) {
  allocatedMemory = allocMemory(memoryToAllocate);
  if (allocatedMemory == NULL) {
    fprintf(stderr, "Error: Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

inline void whenMemoryIsWritten(void) {
  if (anAllocation == NULL) {
    fprintf(stderr, "Error: Attempting to write to a null pointer.\n");
    exit(EXIT_FAILURE);
  }
  *((char *)anAllocation) = WRITTEN_VALUE;
}

inline void thenSomeMemoryIsReturned(CuTest *const cuTest) {
  CuAssertPtrNotNull(cuTest, allocatedMemory);
}

inline void thenTheTwoAdressesAreDifferent(CuTest *const cuTest) {
  CuAssertTrue(cuTest, anAllocation != allocatedMemory);
}

inline void thenBothDoNotOverlap(CuTest *const cuTest) {
  if (anAllocation == NULL || allocatedMemory == NULL) {
    CuFail(cuTest, "[thenBothDoNotOverlap] Null pointer detected.");
    return;
  }

  int distance = (char *)anAllocation - (char *)allocatedMemory;
  distance = abs(distance);

  CuAssertTrue(cuTest, distance >= ALLOCATION_SIZE);
}

inline void thenMemorySuccessfullyWritten(CuTest *const cuTest) {
  uint8_t writtenValue = WRITTEN_VALUE;
  uint8_t readValue = *((uint8_t *)anAllocation);

  CuAssertIntEquals(cuTest, writtenValue, readValue);
}
