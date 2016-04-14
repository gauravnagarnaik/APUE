#include "parse.h"

int 
parse(char *command, char *cmdArgv[], char **nextArg, int *operation)
{
    int cmdArgc = 0;
    int terminate = 0;
    char *srcPtr = command;
    while(*srcPtr != '\0' && *srcPtr != '\n' && terminate == 0)
    {
        *cmdArgv = srcPtr;
        cmdArgc++;
        while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\0' && *srcPtr != '\n' && terminate == 0)
        {
            switch(*srcPtr)
            {
                case '&':
                    *operation = BACKGROUND;
                     break;
                case '>':
                    *operation = REDIRECT_OUTPUT;
                    *cmdArgv = '\0';
                    srcPtr++;
                    if(*srcPtr == '>')
                    {
                        *operation = APPEND_OUTPUT;
                        srcPtr++;
                    }
                    while(*srcPtr == ' ' || *srcPtr == '\t')
                        srcPtr++;
                    *nextArg = srcPtr;
                    trim(*nextArg);
                    terminate = 1;
                    break;
                case '<':
                    *operation = REDIRECT_INPUT;
                    *cmdArgv = '\0';
                    srcPtr++;
                    while(*srcPtr == ' ' || *srcPtr == '\t')
                        srcPtr++;
                    *nextArg = srcPtr;
                    trim(*nextArg);
                    terminate = 1;
                    break;
                case '|':
                    *operation = PIPELINE;
                    *cmdArgv = '\0';
                    srcPtr++;
                    while(*srcPtr == ' ' || *srcPtr == '\t')
                        srcPtr++;
                    *nextArg = srcPtr;
                    terminate = 1;
                    break;
            }
            srcPtr++;
        }
        while((*srcPtr == ' ' || *srcPtr == '\t' || *srcPtr == '\n') && terminate == 0)
        {
            *srcPtr = '\0';
            srcPtr++;
        }
        cmdArgv++;
    }
    *cmdArgv = '\0';
    return cmdArgc;
}


void trim(char *srcPtr){
    while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\n')
    {
        srcPtr++;
    }
    *srcPtr = '\0';
}