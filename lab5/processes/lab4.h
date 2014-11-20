#ifndef LAB4_H
#define LAB4_H

void run();
void produce(int id);
void consume(int id);
void setupmq();
void destroymq();
void gettime(double *val);
void wait_all();
void create_producers();
void create_consumers();

#endif
