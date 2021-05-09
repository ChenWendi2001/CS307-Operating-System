#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100

int max_size;

struct node
{
    int use_flag;
    int start;
    int end;
    char *name;
    struct node *next;
};

struct node *head = NULL;
void init(int size);
void request(char *name, int size, char mode);
void release(char *name);
void compact();
void print();

void clear();

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Parameter error.\n");
        return -1;
    }
    max_size = atoi(argv[1]);
    init(atoi(argv[1]));
    char task[SIZE];
    char *temp;
    char *command;
    char *name;
    int size;
    char *mode;
    while (1)
    {
        printf("allocator> ");
        fgets(task, SIZE, stdin);
        temp = strdup(task);
        command = strsep(&temp, " ");
        if (strcmp(command, "X\n") == 0 || strcmp(command, "X") == 0)
        {
            free(temp);
            clear();
            return 0;
        }
        else if (strcmp(command, "STAT\n") == 0 || strcmp(command, "STAT") == 0)
        {
            print();
        }
        else if (strcmp(command, "RQ") == 0)
        {
            name = strsep(&temp, " ");
            size = atoi(strsep(&temp, " "));
            mode = strsep(&temp, " ");
            request(name, size, mode[0]);
        }
        else if (strcmp(command, "RL") == 0)
        {
            name = strsep(&temp, " ");
            int i = 0;
            while (name[i] != '\0')
            {
                if (name[i] == '\n')
                {
                    name[i] = '\0';
                    break;
                }
                i++;
            }
            release(name);
        }
        else if (strcmp(command, "C\n") == 0 || strcmp(command, "C") == 0)
        {
            compact();
        }
        else
        {
            printf("Invalid command.\n");
        }
        free(temp);
    }

    return 0;
}

void init(int size)
{
    head = (struct node *)malloc(sizeof(struct node));
    head->use_flag = 0;
    head->start = 0;
    head->end = size;
    head->name = NULL;
    head->next = NULL;
}

void request(char *name, int size, char mode)
{

    struct node *p = head, *temp = NULL;
    int cur_size = -1;
    if (mode == 'F' || mode == 'f')
    {
        while (p)
        {
            if (p->use_flag == 0)
            {
                if (p->end - p->start >= size)
                {
                    temp = p;
                    break;
                }
            }
            p = p->next;
        }
    }

    if (mode == 'B' || mode == 'b')
    {
        while (p)
        {
            if (p->use_flag == 0)
            {
                if (p->end - p->start >= size)
                {
                    if (cur_size < 0 || p->end - p->start < cur_size)
                    {
                        cur_size = p->end - p->start;
                        temp = p;
                    }
                }
            }
            p = p->next;
        }
    }

    if (mode == 'W' || mode == 'w')
    {
        while (p)
        {
            if (p->use_flag == 0)
            {
                if (p->end - p->start >= size)
                {
                    if (cur_size < 0 || p->end - p->start > cur_size)
                    {
                        cur_size = p->end - p->start;
                        temp = p;
                    }
                }
            }
            p = p->next;
        }
    }

    if (temp == NULL)
    {
        printf("No enough memory.\n");
        return;
    }

    if (temp->end - temp->start == size)
    {
        temp->name = (char *)malloc(sizeof(char) * SIZE);
        temp->use_flag = 1;
        strcpy(temp->name, name);
        return;
    }
    else if (temp->end - temp->start > size)
    {
        temp->name = (char *)malloc(sizeof(char) * SIZE);
        temp->use_flag = 1;
        strcpy(temp->name, name);

        struct node *newnode = (struct node *)malloc(sizeof(struct node));
        newnode->end = temp->end;
        temp->end = temp->start + size;
        newnode->start = temp->end;
        newnode->use_flag = 0;
        newnode->name = NULL;
        newnode->next = temp->next;

        temp->next = newnode;
        return;
    }
}

void release(char *name)
{
    struct node *slow = head, *fast = head->next, *temp;
    if (head->name && strcmp(head->name, name) == 0)
    {
        //printf("RL %s",name);
        free(head->name);
        head->name = NULL;
        head->use_flag = 0;
        if (head->next && head->next->use_flag == 0)
        {
            temp = head->next;
            head->end = temp->end;
            head->next = temp->next;
            free(temp);
        }
        return;
    }
    else
    {
        while (fast)
        {
            if (fast->name && strcmp(fast->name, name) == 0)
            {
                free(fast->name);
                fast->name = NULL;
                fast->use_flag = 0;
                if (fast->next && fast->next->use_flag == 0)
                {
                    temp = fast->next;
                    fast->end = temp->end;
                    fast->next = temp->next;
                    free(temp);
                }
                if (slow->use_flag == 0)
                {
                    slow->end = fast->end;
                    slow->next = fast->next;
                    free(fast);
                }
                return;
            }
            fast = fast->next;
            slow = slow->next;
        }
    }
    printf("No process found.\n");
}

void compact(){
    if(head->end == max_size){
        return;
    }
    struct node * temp, *fast, *slow;
    while(head->use_flag==0){
        temp = head->next;
        free(head);
        head = temp;
    }
    head->end = head->end-head->start;
    head->start = 0;
    slow = head;fast = head->next;
    while(fast){
        if(fast->use_flag==0){
            temp = fast->next;
            free(fast);
            slow->next = temp;
            fast = temp;
        }
        else{
            fast->end = fast->end-fast->start;
            fast->start = slow->end;
            fast->end += slow->end;
            slow = slow->next;fast = fast->next;
        }
    }
    struct node* newnode = (struct node*)malloc(sizeof(struct node));
    newnode->start = slow->end;
    newnode->end = max_size;
    newnode->name = NULL;
    newnode->next = NULL;
    newnode->use_flag = 0;
    slow->next = newnode;
}

void print()
{
    struct node *temp = head;
    while (temp)
    {
        if (temp->use_flag)
        {
            printf("Addresses [%d:%d] Process %s\n", temp->start, temp->end - 1, temp->name);
        }
        else
        {
            printf("Addresses [%d:%d] Unused\n", temp->start, temp->end - 1);
        }
        temp = temp->next;
    }
}

void clear()
{
    struct node *p = head, *temp;
    while (p)
    {
        if (p->use_flag)
        {
            free(p->name);
        }
        temp = p;
        p = p->next;
        free(temp);
    }
}
