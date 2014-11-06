#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include "lab4.h"

int main(int argc, char *argv[]) {
    int n, b;
    char *endptr;
    if (argc < 3) {
        printf("usage: %s <N> <B>\n", argv[0]);
        exit(1);
    }
    n = strtol(argv[1], &endptr, 0);
    if (endptr == argv[1]) {
        printf("N must be a number.\n");
        exit(2);
    }
    b = strtol(argv[2], &endptr, 0);
    if (endptr == argv[2]) {
        printf("B must be a number.\n");
        exit(3);
    }

    run(n, b);

    return 0;
}

void run(int n, int b) {
    // Create variables to hold the start and end times
    struct timeval forktime, generatetime, finaltime;
    // Queue variables
    mqd_t mqdes;
    // Fork return
    pid_t pid;

    setuprand();

    setupmq(&mqdes, b);

    gettime(&forktime);

    pid = fork();
    gettime(&generatetime);
    if (pid > 0) {
        // parent
        produce(n, &mqdes);
    } else if (pid == 0)  {
        // child
        consume(n, &mqdes);
        gettime(&finaltime);

        // Output times
        printf("Time to initialize system: %u.%u seconds\n",
            generatetime.tv_sec - forktime.tv_sec,
            generatetime.tv_usec - forktime.tv_usec);
        printf("Time to transmit data: %u.%u seconds\n",
            finaltime.tv_sec - generatetime.tv_sec,
            finaltime.tv_usec - generatetime.tv_usec);
    } else { 
        perror("fork() failed");
        exit(9);
    }
}

void produce(int n, mqd_t *mqdes) {
    int i;
    size_t msg_len = sizeof(int);
    for (i = 0; i < n; ++i) {
        int r = rand() & 0xf;
        if (mq_send(*mqdes, (char *)&r, msg_len, 0)) {
            perror("mq_send() failed");
            exit(7);
        }
    }
}

void consume(int n, mqd_t *mqdes) {
    int i;
    int val;
    size_t msg_len = sizeof(val);
    for (i = 0; i < n; ++i) {
        if (mq_receive(*mqdes, (char *) &val, msg_len, NULL) == -1) {
            perror("mq_receive() failed");
            exit(8);
        }
        printf("%d is consumed\n", val);
    }
}

void setupmq(mqd_t *mqdes, int b) {
    struct mq_attr attr;
    attr.mq_maxmsg = b;
    attr.mq_msgsize = sizeof(int);
    attr.mq_flags = 0;
    *mqdes = mq_open("/e8chu", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
    if (*mqdes == -1) {
        perror("mq_open() failed");
        exit(6);
    }
}

void setuprand() {
    time_t seed = time(NULL);

    if (seed == -1) {
        perror("time() failed");
        exit(5);
    }

    srand(seed);
}

void gettime(struct timeval *tv) {
    if (gettimeofday(tv, NULL)) {
        perror("gettimeofday() failed");
        exit(4);
    }
}

