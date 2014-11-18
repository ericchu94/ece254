produce : lab5.o
	cc -pthread -O3 -o produce lab5.o

lab5.o : lab5.c lab5.h
	cc -O3 -Wall -c -o lab5.o lab5.c

.PHONY : clean
clean :
	rm -f produce lab5.o
