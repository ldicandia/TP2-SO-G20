CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I. 

OBJS = shared.o

all: libshared.a

#shared.o: shared.c shared.h
#	$(CC) $(CFLAGS) -c shared.c -o shared.o

libshared.a: $(OBJS)
	ar rcs libshared.a $(OBJS)

clean:
	rm -f *.o *.a

.PHONY: all clean