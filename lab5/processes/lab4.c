#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <math.h>
#include "lab4.h"

#define MQ_NAME "/e8chu"

int n, b, p, c;
// Queue variables
mqd_t mqdes;

int main(int argc, char *argv[]) {

    if (argc != 5) {
        fprintf(stderr, "usage: %s <N> <B> <P> <C>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	n = getint(argv[1]);
	b = getint(argv[2]);
	p = getint(argv[3]);
	c = getint(argv[4]);

    run();

    return 0;
}

void run() {
    // Create variables to hold the start and end times
    double start, end;
    // Fork return
    pid_t pid;

    setupmq();

    gettime(&start);

    create_producers();
    create_consumers();
    
    wait_all();

    gettime(&end);

    printf("System execution time: %.6lf\n", end - start);

    destroymq();
}

void produce(int id) {
    size_t msg_len = sizeof(int);
    int val;
    for (val = id; val < n; val += p) {
        if (mq_send(mqdes, (char *)&val, msg_len, 0) == -1) {
            perror("mq_send() failed");
            exit(EXIT_FAILURE);
        }
    }
}

void consume(int id) {
    int val, root;
    int i;
    int count = n / c;
    if (id < n - count * c) {
        ++count;
    }
    size_t msg_len = sizeof(val);
    for (i = 0; i < count; ++i) {
        if (mq_receive(mqdes, (char *) &val, msg_len, NULL) == -1) {
            perror("mq_receive() failed");
            exit(EXIT_FAILURE);
        }

        root = sqrt(val);
        if (root * root == val) {
            printf("%d %d %d\n", id, val, root);
        }
    }
}

void setupmq() {
    struct mq_attr attr;
    attr.mq_maxmsg = b;
    attr.mq_msgsize = sizeof(int);
    attr.mq_flags = 0;
    mqdes = mq_open(MQ_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
    if (mqdes == -1) {
        perror("mq_open() failed");
        exit(EXIT_FAILURE);
    }
}

void destroymq() {
    if (mq_close(mqdes) == -1) {
        perror("mq_close() failed");
        exit(EXIT_FAILURE);
    }
    if (mq_unlink(MQ_NAME) == -1) {
        perror("mq_unlink() failed");
        exit(EXIT_FAILURE);
    }
}

void wait_all() {
    int status;
    int i;
    for (i = 0; i < c + p; ++i) {
        do {
            if (wait(&status) == -1) {
                perror("wait() failed");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

void create_producers() {
    int i;
    pid_t pid;
    for (i = 0; i < p; ++i) {
        pid = fork();
        if (pid == -1) {
            perror("fork() failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            produce(i);
            exit(EXIT_SUCCESS);
        }
    }
}

void create_consumers() {
    int i;
    pid_t pid;
    for (i = 0; i < c; ++i) {
        pid = fork();
        if (pid == -1) {
            perror("fork() failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            consume(i);
            exit(EXIT_SUCCESS);
        }
    }
}

void gettime(double *val) {
    struct timeval tv;
    if (gettimeofday(&tv, NULL)) {
        perror("gettimeofday() failed");
        exit(EXIT_FAILURE);
    }
    *val = tv.tv_sec + tv.tv_usec / 1000000.0;
}

int getint(char str[]) {
    char *endptr;
    int i;

    i = strtol(str, &endptr, 0);
    if (endptr == str) {
        fprintf(stderr, "strtol() failed\n");
        exit(EXIT_FAILURE);
    }

    return i;
}

