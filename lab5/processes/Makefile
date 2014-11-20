all: produce

produce: lab4.o
	gcc -lrt -O3 -o produce lab4.o

lab4.o: lab4.c
	gcc -O3 -c lab4.c

clean:
	rm -rf *.o produce
