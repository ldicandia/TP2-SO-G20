include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c tests/*.c lib/*.c programs/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_SHARED=$(wildcard ../../Shared/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHARED=$(SOURCES_SHARED:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)

all: $(MODULE)

$(MODULE): $(OBJECTS) $(OBJECTS_SHARED) $(OBJECTS_ASM)
	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(OBJECTS) $(OBJECTS_SHARED) -o ../$(MODULE) 
	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(OBJECTS_ASM) $(OBJECTS) $(OBJECTS_SHARED) -o ../0000-sampleCodeModule.elf 

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

obj/%.asm.o: asm/%.asm
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf obj *.o */*.o ../../Shared/*.o

.PHONY: all clean print

