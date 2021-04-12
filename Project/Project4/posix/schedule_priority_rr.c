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
        struct node*temp = head->next;
        struct node*max_node = head;
        int max = head->task->priority;
        int max_count = 1;
        

        while(temp){
            if(temp->task->priority>max){
                max_node = temp;
                max = temp->task->priority;
                max_count = 1;
            }
            else if(temp->task->priority==max){
                max_count ++;
            }  
            temp = temp->next; 
        }
        
        //printf("max_count:%d\n",max_count);

        if(max_count==1){
            if(max_node->task->flag){
                max_node->task->flag=0;
                response+=time;
            }
            run(max_node->task,max_node->task->burst);
            time+=max_node->task->burst;
            turnaround+=time;
            
            delete(&head,max_node->task);
        }
        else{
            while(1){
                int flag = 0;
                temp = head;
                while(temp){
                    if(temp->task->priority==max){
                        //printf("process:%s\n",temp->task->name);
                        flag=1;
                        if(temp->task->burst>QUANTUM){
                            if(temp->task->flag){
                                temp->task->flag=0;
                                response+=time;
                            }
                            run(temp->task,QUANTUM);
                            time+=QUANTUM;

                            temp->task->burst = temp->task->burst-QUANTUM;
                            temp = temp->next;
                        }
                        else{
                            if(temp->task->flag){
                                temp->task->flag=0;
                                response+=time;
                            }
                            run(temp->task,temp->task->burst);
                            time+=temp->task->burst;
                            turnaround+=time;

                            struct node* next = temp->next;
                            delete(&head,temp->task);
                            temp = next;
                        }
                    }
                    else{
                        temp = temp->next;
                    }
                }
                if(flag==0) break;
            }
        }
    }
    printf("Average Turnaround:%.2f\t Average Waiting:%.2f\t Average Response:%.2f\n",
    turnaround/(float)count,(turnaround-all_burst)/(float)count,response/(float)count);
}