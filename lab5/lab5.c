#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include "math.h"
#include "lab5.h"

int n, b, p, c;
int *buffer;
int buffer_start, buffer_end;
sem_t empty, fill;
pthread_mutex_t mutex;

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
    struct thread producers[p];
    struct thread consumers[c];

    // also initialises fill/empty & mutex
    initialise_buffer();

    create_producers(producers);
    create_consumers(consumers);

    join_threads(producers, p);
    join_threads(consumers, c);

    // TODO extract and free mutex/sem
    free(buffer);
}

static void * produce(void *arg) {
    int id = *(int *)arg;
    int err;

    int val;
    for (val = id; val < n; val += p) {
        if (sem_wait(&empty) == -1) {
            perror("sem_wait() failed");
            exit(EXIT_FAILURE);
        }

        err = pthread_mutex_lock(&mutex);
        if (err) {
            perror("pthread_mutex_lock() failed");
            exit(EXIT_FAILURE);
        }
        
        push(val);

        err = pthread_mutex_unlock(&mutex);
        if (err) {
            perror("pthread_mutex_unlock() failed");
            exit(EXIT_FAILURE);
        }
        if (sem_post(&fill) == -1) {
            perror("sem_post() failed");
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}

static void * consume(void *arg) {
    int id = *(int *)arg;
    int val;
    int err;
    int root;

    int i;
    int count = n / c;
    if (id < n - count * c) {
        ++count;
    }

    for (i = 0; i < count; ++i) {
        if (sem_wait(&fill) == -1) {
            perror("sem_wait() failed");
            exit(EXIT_FAILURE);
        }

        err = pthread_mutex_lock(&mutex);
        if (err) {
            perror("pthread_mutex_lock() failed");
            exit(EXIT_FAILURE);
        }

        val = shift();
        root = sqrt(val);
        if (root * root == val) {
            printf("%d %d %d\n", id ,val, root);
        }

        err = pthread_mutex_unlock(&mutex);
        if (err) {
            perror("pthread_mutex_unlock() failed");
            exit(EXIT_FAILURE);
        }

        if (sem_post(&empty) == -1) {
            perror("sem_post() failed");
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}

void push(int val) {
    // this function assumes buffer is not full
    if (buffer_end < b - 1) {
        ++buffer_end;
    } else {
        buffer_end = 0;
    }
    buffer[buffer_end] = val;
}

int shift() {
    // this function assumes buffer is not empty
    if (buffer_start < b - 1) {
        ++buffer_start;
    } else {
        buffer_start = 0;
    }
    return buffer[buffer_start];
}

void create_thread(pthread_t *thread, void *(*start_routine) (void *), void *arg) {
   int err = pthread_create(thread, NULL, start_routine, arg);

   if (err) {
        errno = err;
        perror("pthread_create() failed");
        exit(EXIT_FAILURE);
   }
}

void create_producer(struct thread *thread) {
    create_thread(&thread->thread, &produce, &thread->id);
}

void create_consumer(struct thread *thread) {
    create_thread(&thread->thread, &consume, &thread->id);
}

void create_producers(struct thread producers[]) {
    int i;
    for (i = 0; i < p; ++i) {
        producers[i].id = i;
        create_producer(&producers[i]);
    }
}

void create_consumers(struct thread consumers[]) {
    int i;
    for (i = 0; i < c; ++i) {
        consumers[i].id = i;
        create_consumer(&consumers[i]);
    }
}

void join_thread(pthread_t thread) {
    int err = pthread_join(thread, NULL);

    if (err) {
        errno = err;
        perror("pthread_join() failed");
        exit(EXIT_FAILURE);
    }
}

void join_threads(struct thread threads[], int thread_count) {
    int i;
    for (i = 0; i < thread_count; ++i) {
        join_thread(threads[i].thread);
    }
}

void initialise_buffer() {
    int err = pthread_mutex_init(&mutex, NULL);
    if (err) {
        errno = err;
        perror("pthread_mutex_init() failed");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&fill, 0, 0) == -1) {
        perror("sem_init() failed");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&empty, 0, b) == -1) {
        perror("sem_init() failed");
        exit(EXIT_FAILURE);
    }
    buffer = malloc(sizeof(int) * b);
    if (buffer == NULL) {
        perror("malloc() failed");
        exit(EXIT_FAILURE);
    }
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
