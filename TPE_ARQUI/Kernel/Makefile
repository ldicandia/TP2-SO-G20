include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

# Objetivo por defecto
all: my_mm

# Compilación con Buddy Memory Manager
buddy: CFLAGS += -DUSE_BUDDY_MEMORY_MANAGER
buddy: $(KERNEL)

# Compilación con el Memory Manager personalizado
my_mm: CFLAGS += -DUSE_MY_MM
my_mm: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
    $(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
    $(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(KERNEL_ELF) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
    $(GCC) $(GCCFLAGS) -fno-pie -no-pie -I./include -c $< -o $@

%.o : %.asm
    $(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
    $(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
    rm -rf asm/*.o *.o *.bin

.PHONY: all clean buddy my_mm
