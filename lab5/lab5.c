#include <stdio.h>
#include <stdlib.h>
#include "lab5.h"

int main(int argc, char *argv[]) {
    int n, b, p, c;

    if (argc != 5) {
        fprintf(stderr, "usage: %s <N> <B> <P> <C>\n", argv[0]);
    }

    n = getint(argv[1]);
    b = getint(argv[2]);
    p = getint(argv[3]);
    c = getint(argv[4]);

    run(n, b, p, c);

    return 0;
}

void run(int n, int b, int p, int c) {

}

int getint(char str[]) {
    char *endptr;
    int i;

    i = strtol(str, &endptr, 0);
    if (endptr == str) {
        fprintf(stderr, "strtol() failed\n");
        exit(1);
    }

    return i;
}
