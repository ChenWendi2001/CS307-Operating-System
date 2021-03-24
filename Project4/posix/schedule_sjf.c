#include "schedulers.h"
#include "cpu.h"
#include "task.h"
#include <string.h>
#include "list.h"
#include <stdlib.h>
#include <stdio.h>

static struct node* head = NULL;
static int response=0, all_burst=0, turnaround = 0,time = 0,count=0,tid=0; 

void add(char *name, int priority, int burst){
    Task *newtask = (Task *)malloc(sizeof(Task ));
    newtask->priority = priority;
    newtask->burst = burst;
    newtask->name = (char*)malloc(sizeof(char)*100);
    newtask->tid = __sync_fetch_and_add(&tid,1);
    strcpy(newtask->name,name);
    insert(&head,newtask);

    newtask->flag = 1;
    count++;
    all_burst+=burst;
}

void schedule(){
    while(head){
        struct node*temp = head;
        struct node*min_node = head;
        int min = temp->task->burst;
        
        while(temp){
            if(temp->task->burst<min){
                min_node = temp;
                min = temp->task->burst;
            }
            temp = temp->next;
        }
        if(min_node->task->flag){
            response+=time;
            min_node->task->flag = 0;
        }
        run(min_node->task,min_node->task->burst);
        time+=min_node->task->burst;
        turnaround+=time;
        delete(&head,min_node->task);
    }

    printf("Average Turnaround:%.2f\t Average Waiting:%.2f\t Average Response:%.2f\n",
    turnaround/(float)count,(turnaround-all_burst)/(float)count,response/(float)count);
}