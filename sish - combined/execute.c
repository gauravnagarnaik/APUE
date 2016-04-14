#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<wait.h>

#include<sys/types.h>

#include "execute.h"
#include "parse.h"

int 
execute(char *command, int prevStatus)
{
    int operation = STANDARD, operation2 = STANDARD, status1, builtin = 0, status;
    char *cmdArgv[INPUT_SIZE], *nextArg = NULL;
    char *cmdArgv2[INPUT_SIZE], *nextArg2 = NULL;
    pid_t pid, pid2;
    char *filePath = NULL;
    FILE *fp;
    int newpipe[2];
    char *curDir = (char *)malloc(100);

    getcwd(curDir, 100);

    parse(command, cmdArgv, &nextArg, &operation);

    if(nextArg)
    {
        char *slash = "/";
        char* str3 = (char *)malloc(1 + strlen(curDir)+ strlen(nextArg)+ strlen(slash));
        strcpy(str3, curDir);
        strcat(str3, slash);
        strcat(str3, nextArg);
        filePath = str3;  
    }

    if(cmdArgv[0] == NULL){
        return 0;
    }

    if(opt_x){
	fprintf(stderr, "+%s\n", command);
	if(nextArg != NULL){
	   fprintf(stderr, "+%s\n", nextArg);
	}
    }

    if(strncmp(*cmdArgv, "exit", 4) == 0){
        builtin = 1;
        exit(0);
    }
    else if(strcmp(*cmdArgv, "cd") == 0){
        builtin = 1;
        status = change_dir(cmdArgv[1]);
	return status;
    }
    else if(strcmp(*cmdArgv, "echo") == 0){
        builtin = 1;
        status = echo(cmdArgv, prevStatus);
	return status;
    }

    if(operation == PIPELINE)
    {
        if((status = pipe(newpipe)) < 0)
        {
            fprintf(stderr, "Pipe failed!");
        }

        parse(nextArg, cmdArgv2, &nextArg2, &operation2);
    }

    pid = fork();
    if( (status = pid) < 0)
    {
        fprintf(stderr, "Error occured while forking\n");
    }

    else if(pid == 0)
    {
        switch(operation)
        {
            case REDIRECT_OUTPUT:
                if(filePath == NULL) break;
                fp = fopen(filePath, "w+");
                status = dup2(fileno(fp), 1);
                break;
            case APPEND_OUTPUT:
                if(filePath == NULL) break;
                fp = fopen(filePath, "a");
                status = dup2(fileno(fp), 1);
                break;
            case REDIRECT_INPUT:
                if(filePath == NULL) break;
                fp = fopen(filePath, "r");
                status = dup2(fileno(fp), 0);
                break;
            case PIPELINE:
                close(newpipe[0]);
                status = dup2(newpipe[1], fileno(stdout));
                close(newpipe[1]);
                break;
        }
        if(builtin == 0){
            if(execvp(*cmdArgv, cmdArgv) < 0){
                status = -1;
                fprintf(stderr, "%s : Command not found\n", cmdArgv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        if(operation == BACKGROUND);
        else if(operation == PIPELINE)
        {
            waitpid(pid, &status1, 0);
            pid2 = fork();
            if(pid2 < 0)
            {
                fprintf(stderr, "Error occured while forking for pipe\n");
                exit(EXIT_FAILURE);
            }
            else if(pid2 == 0)
            {
                close(newpipe[1]);
                dup2(newpipe[0], fileno(stdin));
                close(newpipe[0]);
                if(execvp(*cmdArgv2, cmdArgv2) < 0){
                    status = -1;
                    fprintf(stderr, "%s : Command not found\n", cmdArgv[0]);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                close(newpipe[0]);
                close(newpipe[1]);
            }
        }  else {
            waitpid(pid, &status1, 0);
        }       
    }
    return status;
}


/**
 * change directory to given argument
 * if no argument specified, change to home directory
 */
int
change_dir(char *path) {
    char *homeDir;

    if (path == NULL) {
        homeDir = getenv("HOME");

        if (chdir(homeDir) < 0) {
            fprintf(stderr, "Cannot cd into '%s'\n", homeDir);
            return -1;
        }   
    } else {
        if (chdir(path) < 0) {
            fprintf(stderr, "Cannot cd into '%s'\n", path);
            return -1;
        }
    }
    return 0;
}


/* echo prints argument to standard output.
 * echo $$ prints the current process ID 
 * echo $? prints the exit status of last command
 */
int
echo(char *cmd[], int status){
    int i = 1;  
    while(1){
        if(cmd[i] == NULL)
        {
            printf("\n");
            fflush(stdout);
            status = 0;
            return 0;
        }
        if (strncmp(cmd[i], "$$", 2) == 0){
            printf("%d ", getpid());
        }else if(strncmp(cmd[i], "$?", 2) == 0){
            printf("%d", status);
        }else{
            printf("%s ", cmd[i]);
        }
        i++;
    }
    return 0;
}

