#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

pthread_t producer;
pthread_t consumer;

sem_t empty;
sem_t full;
sem_t mutex;

typedef struct {
    int val;
} item;

void* insert_item(item* buffer) {
    while (1) {
        sem_wait(&empty);
        sem_wait(&mutex);
        // insert data
        sem_post(&mutex);
        sem_post(&full);
    }

}

void* remove_item(item* buffer) {
    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);
        // remove data
        sem_post(&mutex);
        sem_post(&empty);
    }
}

void* main(int argc, char **argv) {
    item buffer[10];
    int in = 0;
    int out = 0;
    
    int sleep = atoi(argv[1]);
    int numProducers = atoi(argv[2]);
    int numConsumers = atoi(argv[3]);
    
    pthread_create(&producer, NULL, insert_item, buffer);
    pthread_create(&consumer, NULL, remove_item, buffer);
    
    sem_init(&empty, 0, 0); 
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 0); // binary
    
    printf("Hello World");

    return 0;
}