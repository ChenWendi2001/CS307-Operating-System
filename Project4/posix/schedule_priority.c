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
        struct node*max_node = head;
        int max = temp->task->priority;
        
        while(temp){
            if(temp->task->priority>max){
                max_node = temp;
                max = temp->task->priority;
            }
            temp = temp->next;
        }
        if(max_node->task->flag){
            response+=time;
            max_node->task->flag = 0;
        }
        run(max_node->task,max_node->task->burst);
        time+=max_node->task->burst;
        turnaround+=time;
        delete(&head,max_node->task);
    }
    printf("Average Turnaround:%.2f\t Average Waiting:%.2f\t Average Response:%.2f\n",
    turnaround/(float)count,(turnaround-all_burst)/(float)count,response/(float)count);
}