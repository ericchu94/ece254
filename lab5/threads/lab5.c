#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include <math.h>
#include <sys/time.h>
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
    double start, end;
    struct thread producers[p];
    struct thread consumers[c];

    // also initialises fill/empty & mutex
    initialise_buffer();

    gettime(&start);

    create_producers(producers);
    create_consumers(consumers);

    join_threads(producers, p);
    join_threads(consumers, c);

    gettime(&end);

    printf("System execution time: %.6lf\n", end - start);

    // destroy buffer and sem/mut
    destroy_buffer();
}

static void * produce(void *arg) {
    int id = *(int *)arg;

    int val;
    for (val = id; val < n; val += p) {
        wait(&empty);

        lock(&mutex);
        
        push(val);

        unlock(&mutex);

        post(&fill);
    }

    return NULL;
}

static void * consume(void *arg) {
    int id = *(int *)arg;
    int val;
    int root;

    int i;
    int count = n / c;
    if (id < n - count * c) {
        ++count;
    }

    for (i = 0; i < count; ++i) {
        wait(&fill);

        lock(&mutex);

        val = shift();

        unlock(&mutex);

        post(&empty);

        root = sqrt(val);
        if (root * root == val) {
            printf("%d %d %d\n", id ,val, root);
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

void unlock(pthread_mutex_t *mutex) {
    int err = pthread_mutex_unlock(mutex);
    if (err) {
        errno = err;
        perror("pthread_mutex_unlock() failed");
        exit(EXIT_FAILURE);
    }
}

void lock(pthread_mutex_t *mutex) {
    int err = pthread_mutex_lock(mutex);
    if (err) {
        errno = err;
        perror("pthread_mutex_lock() failed");
        exit(EXIT_FAILURE);
    }
}

void post(sem_t *sem) {
    if (sem_post(sem) == -1) {
        perror("sem_post() failed");
        exit(EXIT_FAILURE);
    }
}

void wait(sem_t *sem) {
    if (sem_wait(sem) == -1) {
        perror("sem_wait() failed");
        exit(EXIT_FAILURE);
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

void destroy_buffer() {
    int err = pthread_mutex_destroy(&mutex);
    if (err) {
        errno = err;
        perror("pthread_mutex_destroy() failed");
        exit(EXIT_FAILURE);
    }
    if (sem_destroy(&fill) == -1) {
        perror("sem_destroy() failed");
        exit(EXIT_FAILURE);
    }
    if (sem_destroy(&empty) == -1) {
        perror("sem_destroy() failed");
        exit(EXIT_FAILURE);
    }
    free(buffer);
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
