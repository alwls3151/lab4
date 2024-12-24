#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

int buffer[BUFFER_SIZE];
int count = 0; 
int in = 0; 
int out = 0; 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER; 

void *producer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        int item = rand() % 100; // 생산 아이템 생성
        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE) { 
            pthread_cond_wait(&not_full, &mutex);
        }

       
        buffer[in] = item;
        printf("Producer %d produced: %d\n", id, item);
        in = (in + 1) % BUFFER_SIZE;
        count++;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == 0) { 
            pthread_cond_wait(&not_empty, &mutex);
        }


        int item = buffer[out];
        printf("Consumer %d consumed: %d\n", id, item);
        out = (out + 1) % BUFFER_SIZE;
        count--;

        pthread_cond_signal(&not_full); 
        pthread_mutex_unlock(&mutex);

        sleep(1); 
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    // 생산자 쓰레드 생성
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // 소비자 쓰레드 생성
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // 쓰레드가 종료되지 않도록 유지
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}
