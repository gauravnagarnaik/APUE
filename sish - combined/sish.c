#include<pwd.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<bsd/stdlib.h>
#include<sys/types.h>

#include "execute.h"

int opt_c;

void signalHandler(int);

/*
 * This is a simple shell that takes interprets commands.
 * Supports option -c to execute commands
 * Supports option -x to enable tracing
 * When option -c is not provided, it runs as a command-line interactive tool
 */
int 
main(int argc, char **argv){
    char *command  = (char*)malloc(sizeof(char)*INPUT_SIZE);
    int ch;
    int prevStatus = 0;

    setprogname((char *) argv[0]);

    while ((ch = getopt(argc, argv, "c:x")) != -1) {
        switch (ch) {
            case 'x':
                opt_x = 1;
                break;
            case 'c':
		opt_c = 1;
                command = optarg;
                execute(command, prevStatus);
                break;
            default:
                fprintf(stderr, "Usage : %s [-c command] -x\n", getprogname());
                exit(EXIT_FAILURE);
        }
    }

    signal(SIGINT, signalHandler);

    if(!opt_c){
        while(1){
            printf("\nsish$ ");
            if(fgets(command, INPUT_SIZE, stdin) == NULL){
                fprintf(stderr, "Error reading input\n");
            } else {
		execute(command, prevStatus);
	    }
        }
    }

    exit(EXIT_SUCCESS);
}


/* 
 * Reset handler to catch SIGINT next time. 
 */
void signalHandler(int sig_num)
{
    signal(SIGINT, signalHandler);
}
