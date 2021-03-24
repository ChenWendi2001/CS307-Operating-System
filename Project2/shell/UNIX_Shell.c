#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE 80 /* 80 chars per line, per command */
#define READ_END 0
#define WRITE_END 1

struct status
{
    int if_wait;
    int if_history;
    int if_exit;
    int in_redirect;
    int out_redirect;
    int pipe;
};

struct status get_input(char *args[], char *args_cache[])
{
    char temp;
    int is_blank = 1;
    int i = -1;
    int j = 0;
    for (; i < MAX_LINE / 2 + 1;)
    {
        temp = getchar();
        if (temp == '\n')
        {
            for (int k = i + 1; k < MAX_LINE / 2 + 1; ++k)
            {
                if (args_cache[k])
                    free(args_cache[k]);
                args_cache[k] = args[k];
                args[k] = NULL;
            }
            break;
        }
        if (temp == '\t' || temp == ' ')
        {
            is_blank = 1;
        }
        else if (is_blank)
        {
            is_blank = 0;
            ++i;
            j = 0;
            if (args_cache[i])
                free(args_cache[i]);
            args_cache[i] = args[i];
            args[i] = malloc(sizeof(char) * 100);
            memset(args[i],0,sizeof(char) * 100);
            args[i][j++] = temp;
        }
        else
        {
            args[i][j++] = temp;
        }
    }
    //debug code starts
    // for(int i = 0;i<MAX_LINE/2+1;++i){
    //     if(args[i]){
    //         printf("%s ",args[i]);
    //     }
    //     else{
    //         printf("\n");
    //         break;
    //     }
    // }
    //debug code ends
    struct status result;
    result.if_history = 0;
    result.if_exit = 0;
    result.if_wait = 1;
    result.out_redirect = -1;
    result.in_redirect = -1;
    result.pipe = -1;
    if (i >= 0 && strcmp(args[0], "!!") == 0)
    {
        for (int k = 0; k < MAX_LINE / 2 + 1; ++k)
        {
            if (args[k])
                free(args[k]);
            args[k] = args_cache[k];
            args_cache[k] = NULL;
        }
        result.if_history = 1;
    }
    else if (i >= 0 && strcmp(args[0], "exit") == 0)
    {
        result.if_exit = 1;
    }
    else if (i >= 0 && strcmp(args[i], "&") == 0)
    {
        free(args[i]);
        args[i] = NULL;
        result.if_wait = 0;
    }
    else
    {
    }

    for (int i = 0; i < MAX_LINE / 2 + 1; ++i)
    {
        if (args[i]&&strcmp(args[i], ">") == 0)
        {
            result.out_redirect = i;
        }
        if (args[i]&&strcmp(args[i], "<") == 0)
        {
            result.in_redirect = i;
        }
        if (args[i]&&strcmp(args[i], "|") == 0)
        {
            result.pipe = i;
        }
    }
    return result;
}

int main(void)
{
    char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
    char *args_cache[MAX_LINE / 2 + 1];
    for (int i = 0; i < MAX_LINE / 2 + 1; ++i)
    { /*initializing*/
        args[i] = NULL;
        args_cache[i] = NULL;
    }

    int should_run = 1;

    char buffer[100];

    while (should_run)
    {
        printf("osh>");
        fflush(stdout);

        /**
         * After reading user input, the steps are:
         * (1) fork a child process
         * (2) the child process will invoke execvp()
         * (3) if command includes &, parent and child will run concurrently
         */
        struct status result = get_input(args, args_cache);
        if (result.if_exit)
        {

            should_run = 0;
            continue;
        }
        else if (args[0] == NULL)
        {
            if (result.if_history)
                printf("No commands in history.\n");
        }
        else
        {
            if (result.in_redirect >= 0 )
            {
                strcpy(buffer,args[result.in_redirect+1]);  
            }
            if (result.out_redirect >= 0 )
            {
                strcpy(buffer,args[result.out_redirect+1]);  
            }

            pid_t pid;
            pid = fork();

            if (pid < 0)
            {
                fprintf(stderr, "Fork Failed.\n");
                return 1;
            }
            else if (pid == 0)
            {
                if (result.in_redirect >= 0)
                {
                    int oldfd, fd;
                    if ((oldfd = open(buffer, O_RDWR | O_CREAT, 0644)) == -1)
                    {
                        printf("open error\n");
                        exit(-1);
                    }
                    fd = dup2(oldfd, fileno(stdin));
                    if (fd == -1)
                    {
                        printf("dup2 error\n");
                        exit(-1);
                    }
                }

                if (result.out_redirect >= 0)
                {
                    int oldfd, fd;
                    if ((oldfd = open(buffer, O_RDWR | O_CREAT, 0644)) == -1)
                    {
                        printf("open error\n");
                        exit(-1);
                    }
                    fd = dup2(oldfd, fileno(stdout));
                    if (fd == -1)
                    {
                        printf("dup2 error\n");
                        exit(-1);
                    }
                }

                char *temp_args[MAX_LINE / 2 + 1];
                char *temp_child_args[MAX_LINE / 2 + 1];
                for (int i = 0;i<MAX_LINE / 2 + 1;++i){
                    temp_args[i] = NULL;
                    temp_child_args[i] = NULL;
                }
                if (result.in_redirect >= 0 )
                {
                    for(int i = 0;i<result.in_redirect;++i){
                        temp_args[i] = malloc(sizeof(char)*100);
                        memset(temp_args[i],0,sizeof(char) * 100);
                        strcpy(temp_args[i],args[i]);
                    }
                }
                else if (result.out_redirect >= 0 )
                {
                    for(int i = 0;i<result.out_redirect;++i){
                        temp_args[i] = malloc(sizeof(char)*100);
                        memset(temp_args[i],0,sizeof(char) * 100);
                        strcpy(temp_args[i],args[i]);
                    }
                }
                else if (result.pipe>=0)
                {
                    for(int i = 0;i<result.pipe&& args[i];++i){
                        temp_args[i] = malloc(sizeof(char)*100);
                        memset(temp_args[i],0,sizeof(char) * 100);
                        strcpy(temp_args[i],args[i]);
                    }
                    for(int i = result.pipe+1;i<MAX_LINE / 2 + 1 && args[i];++i){
                        temp_child_args[i-result.pipe-1] = malloc(sizeof(char)*100);
                        memset(temp_child_args[i-result.pipe-1],0,sizeof(char) * 100);
                        strcpy(temp_child_args[i-result.pipe-1],args[i]);
                    }
                }
                int a;
                if (result.in_redirect >= 0 || result.out_redirect >= 0)
                    a = execvp(temp_args[0], temp_args);
                else if (result.pipe>=0){
                    int fd[2];
                    pid_t sub_pid;
                    if(pipe(fd) == -1){
                        fprintf(stderr,"Pipe failed.\n");
                        return 1;
                    }
                    sub_pid = fork();
                    
                    
                    if(sub_pid < 0){
                        fprintf(stderr,"Fork failed.\n");
                        return 1;
                    }
                    if(sub_pid > 0){
                        //wait(NULL);
                        close(fileno(stdout));
                        dup2(fd[WRITE_END], fileno(stdout));
                        close(fd[READ_END]);
                        a = execvp(temp_args[0],temp_args);
                        wait(NULL);
                    }
                    else{
                        close(fileno(stdin));
                        dup2(fd[READ_END], fileno(stdin));
                        close(fd[WRITE_END]);
                        execvp(temp_child_args[0],temp_child_args);
                    }
                }
                else 
                    a = execvp(args[0], args);


                if (a < 0)
                    fprintf(stderr, "No command found.\n");
                for(int i = 0;i<MAX_LINE / 2 + 1;++i){
                    if(temp_args[i]) {
                        free(temp_args[i]);
                        temp_args[i] = NULL;
                    }
                    if(temp_child_args[i]) {
                        free(temp_child_args[i]);
                        temp_child_args[i] = NULL;
                    }
                }
            }
            else
            {
                if (result.if_wait)
                {
                    wait(NULL);
                    printf("Child Complete!\n");
                }
                else
                    printf("Concurrent Running!\n");
            }
        }
    }
    for (int i = 0; i < MAX_LINE / 2 + 1; ++i)
    {
        if (args[i])
        {
            free(args[i]);
            args[i] = NULL;
        }
        if (args_cache[i])
        {
            free(args_cache[i]);
            args_cache[i] = NULL;
        }
    }
    return 0;
}