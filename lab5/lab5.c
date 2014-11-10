#include <stdio.h>

int main(int argc, char *argv[]) {

    if (argc != 5) {
        fprintf(stderr, "usage: %s <N> <B> <P> <C>\n", argv[0]);
    }

    return 0;
}
