# Makefile
# User Id:          e8chu
# Student number:   20491864
# Course:           ECE254
# Date:             September 11, 2014

CC=gcc
CFLAGS=-Wall
LD=gcc
LDFLAGS=
OBJS=main.o

all: main.out

main.out: $(OBJS)
	$(LD) $(CFLAGS) $(LDFLAGS) -o myls $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

.PHONY: clean
clean:
	rm -f *.o myls

