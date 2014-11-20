produce : lab5.o
	gcc -pthread -lm -O3 -o produce lab5.o

debug: lab5.c lab5.h
	gcc -g -Wall -lm -pthread -o d lab5.c

lab5.o : lab5.c lab5.h
	gcc -O3 -Wall -c -o lab5.o lab5.c

.PHONY : clean
clean :
	rm -f produce d lab5.o
