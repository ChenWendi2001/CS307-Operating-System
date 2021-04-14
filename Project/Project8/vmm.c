#include <stdio.h>

#define SIZE_OF_TABLE 256
#define SIZE_OF_TLB 16
#define NUM_OF_FRAME 128
#define SIZE_OF_FRAME 256

FILE * input=NULL;
FILE * output=NULL;
FILE * disk=NULL;

typedef struct{
    int page;
    int frame;
}tlb;

int visit_count, tlb_hit, page_fault;

int table[SIZE_OF_TABLE];
int flag[SIZE_OF_TABLE];
char mem[NUM_OF_FRAME][SIZE_OF_FRAME];

tlb TLB[SIZE_OF_TLB];
int visit_tlb(int page);
int insert_tlb(int page,int frame);
int tlb_count;

tlb list_of_frame[NUM_OF_FRAME];
int request_frame(int page);
void visit_frame(int page);
int frame_count;


int init(int argc,char* argv[]);

void visit(unsigned int virtual);

void destruct();

int main(int argc,char* argv[]){
    if(init(argc,argv)<0){
        return -1;
    }

    while(!feof(input)){
        unsigned int virtual;
        fscanf(input,"%u",&virtual);
        visit(virtual);
    }
    destruct();
    printf("Page-fault rate:%.3f\t TLB hit rate:%.3f\n",(float)page_fault/visit_count,(float)tlb_hit/visit_count);
    return 0;
}

int init(int argc,char* argv[]){
    if(argc!=3){
        printf("Invalid parameters.\n");
        destruct();
        return -1;
    }
    input = fopen(argv[2],"r");
    output = fopen(argv[1],"w");
    disk = fopen("BACKING_STORE.bin","rb");
    if(input==NULL||output==NULL||disk==NULL){
        printf("File not found.\n");
        return -1;
    }

    //init frame flag
    for(int i =0;i<SIZE_OF_TABLE;i++){
        flag[i] = 0;
    }

    //init frame list
    frame_count = 0;

    //init tlb count
    tlb_count = 0;

    return 0;
}

int visit_tlb(int page){
    for(int i = 0;i<tlb_count;i++){
        if(page == TLB[i].page){
            tlb temp = TLB[i];
            for(int j = i-1;j>=0;j--){
                TLB[j+1] = TLB[j];
            }
            TLB[0] = temp;
            tlb_hit++;
            return TLB[0].frame;
        }
    }
    return -1;
}
int insert_tlb(int page,int frame){
    if(tlb_count==SIZE_OF_TLB){
        for(int i = SIZE_OF_TLB-2;i>=0;i--){
            TLB[i+1] = TLB[i];
        }
        TLB[0].page = page;TLB[0].frame = frame;
    }
    else{
        for(int i = tlb_count-1;i>=0;i--){
            TLB[i+1] = TLB[i];
        }
        TLB[0].page = page;TLB[0].frame = frame;
        tlb_count++;
    }
}

int request_frame(int page){
    if(frame_count<NUM_OF_FRAME){
        for(int i = frame_count-1;i>=0;i--){
            list_of_frame[i+1] = list_of_frame[i]; 
        }
        flag[page] = 1;
        list_of_frame[0].page = page;
        list_of_frame[0].frame = frame_count++;
    }
    else{
        tlb temp = list_of_frame[NUM_OF_FRAME-1];
        for(int i = NUM_OF_FRAME-2;i>=0;i--){
            list_of_frame[i+1] = list_of_frame[i];
        }
        flag[temp.page] = 0;
        flag[page] = 1;
        list_of_frame[0] = temp;
        list_of_frame[0].page = page;
    }
    return list_of_frame[0].frame;
}

void visit_frame(int page){
    for(int i = 0;i<frame_count;i++){
        if(list_of_frame[i].page == page){
            tlb temp = list_of_frame[i];
            for(int j = i-1;j>=0;j--){
                list_of_frame[j+1] = list_of_frame[j];
            }
            list_of_frame[0] = temp;
            return;
        }
    }
}

void visit(unsigned int virtual){
    visit_count++;
    unsigned page = (virtual & 0xFF00)>>8;
    unsigned offset = (virtual & 0x00FF);
    
    int tlb_flag = visit_tlb(page),frame;
    if(tlb_flag>=0){
        frame = tlb_flag;
    }
    else{
        if(flag[page]){
            visit_frame(page);
        }
        else{
            page_fault++;
            fseek(disk, virtual&0xFF00, SEEK_SET );
            table[page] = request_frame(page);
            fread(mem[table[page]],sizeof(char),SIZE_OF_FRAME,disk);
        }
        insert_tlb(page,table[page]);
        frame = table[page];
    }
    
    unsigned int physical = (frame<<8)+offset;
    fprintf(output,"Virtual address: %u Physical address: %u Value: %d\n",
    virtual & 0xFFFF,physical,mem[frame][offset]);
}

void destruct(){
    if(input) fclose(input);
    if(output) fclose(output);
    if(disk) fclose(disk);
}