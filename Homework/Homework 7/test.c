#include <stdio.h>
#include <stdlib.h>

typedef struct{
    struct StackNode* next;
    int data;
}StackNode;

StackNode *top = NULL;

void push(int x,StackNode** top){
    StackNode* temp = (StackNode*)malloc(sizeof(StackNode));
    temp->next = *top;
    temp->data = x;
    *top = temp;
}

int pop(StackNode** top){
    if(top){
        int temp = (*top)->data;
		*top = (*top)->next;
        return temp;
    }
    return -1;
}

int main(){
    StackNode *top = NULL;
    push(5, &top);
    push(10, &top);
    push(15, &top);

    int value = pop(&top);
    printf("%d\n",value);
    value = pop(&top);
    printf("%d\n",value);
    value = pop(&top);
    printf("%d\n",value);
    
    return 0;
}