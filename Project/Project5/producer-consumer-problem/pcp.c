#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include "buffer.h"

#define true 1
#define MAX 100

pthread_mutex_t mutex;
sem_t full;
sem_t empty;

buffer_item buffer[BUFFER_SIZE];
pthread_t bee[MAX];

int head,rear;

int insert_item(buffer_item item){
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    buffer[rear] = item;
    rear = (rear+1)%BUFFER_SIZE;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    return 0;
}

int remove_item(buffer_item *item){
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    *item = buffer[head];
    head = (head+1)%BUFFER_SIZE;
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return 0;
}

void *producer(void *param){
    buffer_item item;
    while(true){
        sleep(rand()%3);
        item = rand()%100;
        if(insert_item(item))
            fprintf(stdin,"report error condition");
        else
            printf("producer produced %d\n",item);
    }
}

void *consumer(void *param){
    buffer_item item;
    while(true){
        sleep(rand()%3);
        if(remove_item(&item))
            fprintf(stdin,"report error condition");
        else
            printf("consumer consumed %d\n",item);
    }
}

int main(int argc, char *argv[]){
    // parser
    if(argc!=4) return -1;
    int sleep_time = atoi(argv[1]);
    int num_of_producer = atoi(argv[2]);
    int num_of_consumer = atoi(argv[3]);
    if(num_of_consumer+num_of_producer>MAX) return -1;

    //initialize
    head = 0;rear = 0;
    pthread_mutex_init(&mutex,NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    //create threads
    int i = 0;
    for(;i<num_of_producer;i++){
        pthread_create(&(bee[i]),NULL,producer,NULL);
    }
    for(;i<num_of_consumer+num_of_producer;i++){
        pthread_create(&(bee[i]),NULL,consumer,NULL);
    }

    //sleep
    sleep(sleep_time);

    // terminate threads
    for(i=0;i<num_of_consumer+num_of_producer;i++){
        pthread_cancel(bee[i]);
        pthread_join(bee[i],NULL);
    }
    return 0;
}

