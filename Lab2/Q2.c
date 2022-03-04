#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define BUFF_SIZE   8

pthread_t producer;
pthread_t consumer;

sem_t empty;
sem_t full;
sem_t mutex;

int idx_insert = 0;
int idx_consume = 0;

void* insert_item(void (*buffer)) {
    while (1) {
        sem_wait(&empty);
        sem_wait(&mutex);

        // insert data
        ((int*)(buffer))[idx_insert%BUFF_SIZE] = idx_insert;
        printf("PRODUCER: Value Added: %d\r\n", idx_insert);
        idx_insert++;

        sem_post(&mutex);
        sem_post(&full);
        
        sleep(rand()%3+1);
    }

}

void* remove_item(void (*buffer)) {
    int idx = 0;
    int val;
    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);
        
        // remove data
        val = ((int*)(buffer))[idx_consume%BUFF_SIZE];
        printf("CONSUMER: Value Consumed: %d\r\n", val);
        idx_consume++;
        
        sem_post(&mutex);
        sem_post(&empty);

        sleep(rand()%3+1);
    }
}

void* main(int argc, char **argv) {
    srand(time(NULL));
    
    if (argc!=4){
        printf("Not enough args\r\n");
        exit(1);
    }
    int sleep_time = atoi(argv[1]);
    int numProducers = atoi(argv[2]);
    int numConsumers = atoi(argv[3]);

    // Critical Sections
    int arr[BUFF_SIZE]; // heap allocate?
    
    sem_init(&empty, 1, BUFF_SIZE); 
    sem_init(&full , 1, 0);
    sem_init(&mutex, 1, 1);

    for (int i=0; i<numProducers; i++)
        pthread_create(&producer, NULL, insert_item, arr);
    
    for (int i=0; i<numConsumers; i++)    
        pthread_create(&consumer, NULL, remove_item, arr);

    printf("Sleeping for: %d\r\n", sleep_time);
    sleep(sleep_time);    
    printf("Sleeping DONE.. Exiting\r\n");

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    return 0;
}