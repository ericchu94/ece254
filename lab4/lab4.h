#ifndef LAB4_H
#define LAB4_H

void run(int n, int b);
void produce(int n, mqd_t *mqdes);
void consume(int n, mqd_t *mqdes);
void setupmq(mqd_t *mqdes, int b);
void setuprand();
void gettime(struct timeval *tv);

#endif
