#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define SIZE 100

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];


void init(char *argv[]);
void print_ds();
void update();
int check();

int request_resources(int customer_num,int request[]);
void release_resources(int customer_num,int release[]);

int main(int argc,char *argv []){
    init(argv);
    char task[SIZE];
    char *temp;
    char * command;
    int num;
    int buffer[NUMBER_OF_RESOURCES];
    while(1){
        fgets(task,SIZE,stdin);
        temp = strdup(task);
        command = strsep(&temp," ");
        if(strcmp(command,"*\n") == 0 || strcmp(command,"*") == 0){
            print_ds();
        }
        else if(strcmp(command,"exit\n") == 0 || strcmp(command,"exit") == 0){
            free(temp);
            return 0;
        }
        else if(strcmp(command,"RL") == 0){
            num = atoi(strsep(&temp," "));
            for(int i =0;i<NUMBER_OF_RESOURCES;++i){
                buffer[i] = atoi(strsep(&temp," "));
            }
            release_resources(num,buffer);
        }
        else if(strcmp(command,"RQ") == 0){
            num = atoi(strsep(&temp," "));
            for(int i =0;i<NUMBER_OF_RESOURCES;++i){
                buffer[i] = atoi(strsep(&temp," "));
            }
            int x = request_resources(num,buffer);
            if(x==0){
                printf("Request Successfully!\n");
            }
            else{
                printf("Request Unsuccessfully!\n");
            }
        }
        free(temp);
    }
    return 0;
}

void init(char *argv[]){
    FILE * in = fopen(argv[1],"r");

    char task[SIZE];
    int request;
    char *temp;
    
    for(int i = 0;i<NUMBER_OF_CUSTOMERS;++i) {
        fgets(task,SIZE,in);
        temp = strdup(task);
        
        for(int j = 0;j<NUMBER_OF_RESOURCES;++j){
            maximum[i][j] = atoi(strsep(&temp,","));
        }
        free(temp);
    }

    for(int i = 0;i<NUMBER_OF_RESOURCES;++i){
        available[i] = atoi(argv[i+2]);
    }

    fclose(in);

    update();
}

void print_ds(){
    printf("Available:\n");
    for(int i = 0;i<NUMBER_OF_RESOURCES;++i){
        printf("%c:%d\t",i+'A',available[i]);
    }
    printf("\n\n");

    printf("Maximum:\n");
    for(int i = 0;i<NUMBER_OF_CUSTOMERS;++i){
        printf("P%d:\t",i);
        for(int j = 0;j<NUMBER_OF_RESOURCES;++j){
            printf("%c:%d\t",j+'A',maximum[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("Allocation:\n");
    for(int i = 0;i<NUMBER_OF_CUSTOMERS;++i){
        printf("P%d:\t",i);
        for(int j = 0;j<NUMBER_OF_RESOURCES;++j){
            printf("%c:%d\t",j+'A',allocation[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("Need:\n");
    for(int i = 0;i<NUMBER_OF_CUSTOMERS;++i){
        printf("P%d:\t",i);
        for(int j = 0;j<NUMBER_OF_RESOURCES;++j){
            printf("%c:%d\t",j+'A',need[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void update(){
    for(int i = 0;i<NUMBER_OF_CUSTOMERS;++i){
        for(int j = 0;j<NUMBER_OF_RESOURCES;++j){
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

int request_resources(int customer_num,int request[]){
    for(int i = 0;i<NUMBER_OF_RESOURCES;++i){
        if(need[customer_num][i]<request[i] || available[i]<request[i] )
            return -1;
    }
    for(int i = 0;i<NUMBER_OF_RESOURCES;++i){
        available[i]-=request[i];
        allocation[customer_num][i]+=request[i];
    }
    update();

    int finish[NUMBER_OF_CUSTOMERS];
    for(int i =0;i<NUMBER_OF_CUSTOMERS;++i){
        finish[i] = 0;
    }
    int work[NUMBER_OF_RESOURCES];
    for(int i =0;i<NUMBER_OF_RESOURCES;++i){
        work[i] = available[i];
    }
    int flag = 1;
    while(flag){
        flag = 0;
        for(int i = 0;i<NUMBER_OF_CUSTOMERS;++i){
            if(finish[i]) continue;
            int inner_flag = 1;
            for(int j = 0;j<NUMBER_OF_RESOURCES;++j){
                if(need[i][j]>work[j]) inner_flag=0;
            }
            if(inner_flag){
                flag = 1;
                finish[i] = 1;
                for(int j = 0;j<NUMBER_OF_RESOURCES;++j){
                    work[j]+=allocation[i][j];
                }
            }
        }
    }
    for(int i = 0;i<NUMBER_OF_CUSTOMERS;++i){
        if(finish[i]==0){
            printf("%d\n",i);
            release_resources(customer_num,request);
            return -1;
        }
    }
    return 0;
}

void release_resources(int customer_num,int release[]){
    for(int i =0;i<NUMBER_OF_RESOURCES;++i){
        allocation[customer_num][i]-=release[i];
        available[i]+=release[i];
    }
    update();
}

