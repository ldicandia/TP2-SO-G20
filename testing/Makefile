include ./include/Makefile.inc

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
TARGET  := MemoryManagerTest
MEMORY_MANAGER_OBJ := ./memoryManager.o

#include "../../TPE_ARQUI/Kernel/include/memoryManager.h"


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(LINKER_FLAGS) $(OBJECTS) $(MEMORY_MANAGER_OBJ) -o $(TARGET).out

%.o : %.c
	$(COMPILER) $< $(COMPILER_FLAGS) -o $@

clean:
	@rm -rf $(OBJECTS)
	@rm -rf ../$(TARGET).out

.PHONY: all clean $(TARGET)