#ifndef LAB5_H
#define LAB5_H

struct thread {
    pthread_t thread;
    int id;
};

int getint(char *str);
void run();
static void * produce(void *arg);
static void * consume(void *arg);
void push(int val);
int shift();
void create_thread(pthread_t *thread, void *(*start_routine) (void *), void *arg);
void create_producers(struct thread producers[]);
void create_consumers(struct thread consumers[]);
void join_threads(struct thread threads[], int thread_count);
void lock(pthread_mutex_t *mutex);
void unlock(pthread_mutex_t *mutex);
void wait(sem_t *sem);
void post(sem_t *sem);
void initialise_buffer();
void destroy_buffer();
void gettime(double *val);

#endif
