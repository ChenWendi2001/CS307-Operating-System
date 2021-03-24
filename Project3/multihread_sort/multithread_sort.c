#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000

int unsorted[SIZE];
int sorted[SIZE];

typedef struct{
    int l;
    int r;
}parameters;

void init();
void quici_sort(int s[],int left, int right);
void *sorter(void *param);
void *merger(void *param);

int main(){
    srand((unsigned)time(NULL)); 
    init();
    pthread_t first_tid, second_tid, merge_tid;
    pthread_attr_t first_attr, second_attr, merge_attr;

    parameters *left_data = (parameters*) malloc(sizeof(parameters));
    parameters *right_data = (parameters*) malloc(sizeof(parameters));
    parameters *merge_data = (parameters*) malloc(sizeof(parameters));
    left_data->l = 0;
    left_data->r = (SIZE>>1);
    right_data->l = left_data->r+1;
    right_data->r = SIZE-1;
    merge_data->l = 0;
    merge_data->r = SIZE-1;

    pthread_attr_init(&first_attr);
    pthread_attr_init(&second_attr);
    pthread_attr_init(&merge_attr);
    
    pthread_create(&first_tid,&first_attr,sorter,left_data);
    pthread_create(&second_tid,&second_attr,sorter,right_data);
    pthread_join(first_tid,NULL);
    pthread_join(second_tid,NULL);

    pthread_create(&merge_tid,&merge_attr,merger,merge_data);
    pthread_join(merge_tid,NULL);

    free(left_data);
    free(right_data);
    free(merge_data);

    for(int i =0;i<SIZE;++i){
        printf("%d ", sorted[i]);
        if(i%10==0) printf("\n");
    }

    return 0;
}

void init(){
    for(int i = 0; i < SIZE;++i)
        unsorted[i] = rand()%100+1;
}


void quick_sort(int s[], int left, int right)
{
    if (left < right)
    {
        int i = left, j = right, pivot = s[left];
        while (i < j)
        {
            while(i < j && s[j] >= pivot){
                j--;
            }
            if(i < j)
                s[i++] = s[j];

            while(i < j && s[i] < pivot){
                i++;
            }
            if(i < j)
                s[j--] = s[i];
        }
        s[i] = pivot;
        quick_sort(s, left, i - 1); 
        quick_sort(s, i + 1, right);
    }
}

void *sorter(void * param){
    parameters* p = (parameters *) param;
    quick_sort(unsorted,p->l,p->r);
}

void *merger(void * param){
    parameters* p = (parameters *) param;
    int mid = (p->l+p->r+1)>>1;
    int i = 0,j = mid+1,k = 0;
    while(i<=mid && j<=p->r){
        if(unsorted[i]<unsorted[j]){
            sorted[k++] = unsorted[i++];
        }
        else{
            sorted[k++] = unsorted[j++];
        }
    }
    while(i<=mid){
        sorted[k++] = unsorted[i++];
    }
    while(j<=p->r){
        sorted[k++] = unsorted[j++];
    }
}