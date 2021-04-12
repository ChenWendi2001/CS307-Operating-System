/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task queue[QUEUE_SIZE];
int head,rear;

task worktodo;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

pthread_mutex_t mutex;
sem_t full;
sem_t empty;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    pthread_mutex_lock(&mutex);
    int flag = 0;
    if(head == (rear+1)%QUEUE_SIZE) flag = 1;
    else{
        queue[rear] = t;
        rear = (rear+1)%QUEUE_SIZE;
    }
    pthread_mutex_unlock(&mutex);
    if(flag==0)sem_post(&full);
    return flag;
}

// remove a task from the queue
task dequeue() 
{
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    task work = queue[head];
    head = (head+1)%QUEUE_SIZE;
    pthread_mutex_unlock(&mutex);
    return work;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    // execute the task
    do{
        task work = dequeue();
        execute(work.function,work.data);
    }while(TRUE);

    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{

    worktodo.function = somefunction;
    worktodo.data = p;
    int state = enqueue(worktodo);
    if(state==0){
        printf("Submission succeeds.\n");
    }
    else{
        printf("Submission fails.\n");
    }
    return state;
}

// initialize the thread pool
void pool_init(void)
{
    head = 0; rear = 0;
    pthread_mutex_init(&mutex,NULL);
    sem_init(&full, 0, 0);
    //sem_init(&empty, 0, QUEUE_SIZE);
    for(int i = 0;i<NUMBER_OF_THREADS;++i){
        pthread_create(&(bee[i]),NULL,worker,NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void)
{
    for(int i = 0;i<NUMBER_OF_THREADS;++i){
        pthread_cancel(bee[i]);
        pthread_join(bee[i],NULL);
    }
}
