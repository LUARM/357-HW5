#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parseline.h"

#define CL_MAX_LENGTH 1024
#define CHANNEL_FILE 1
#define CHANNEL_PIPE 2
int main() 
{
    CommandLine *chanList;
    char line[CL_MAX_LENGTH];
    int ch, bytes_read = 0;
    Stage  *CurrStage;
    Arg    *CurrArg;
    int    StageCount;

    while ((ch = getc(stdin)) != EOF) 
    {
        if (bytes_read > CL_MAX_LENGTH-1) 
        {
            fprintf(stderr, "Maximum  length exceeded\n");
            return 0;
        }
        if (ch == '\n')
            break;
        line[bytes_read] = ch;
        ++bytes_read;
    }
    line[bytes_read] = 0;
    chanList = parseLine(line);
    CurrStage = chanList->first;
    StageCount = 0;
  
    while (CurrStage) {
        fprintf(stdout, "\n--------\n");
        fprintf(stdout, "Stage %d: \"", StageCount);
        if (CurrStage->input.type == CHANNEL_PIPE)
            fprintf(stdout, " ");
        fprintf(stdout, "%s", CurrStage->args.first->arg);
        if (CurrStage->input.type == CHANNEL_FILE) 
        {
            fprintf(stdout, " < %s", CurrStage->input.val.file);
        }
        if (CurrStage->output.type == CHANNEL_FILE) 
        {
            fprintf(stdout, " > %s", CurrStage->output.val.file);
        }
        CurrArg = CurrStage->args.first->next;
        while (CurrArg) {
            fprintf(stdout, " %s", CurrArg->arg);
            CurrArg = CurrArg->next;
        }
        if (CurrStage->output.type == CHANNEL_PIPE)
            fprintf(stdout, " ");
        fprintf(stdout, "\"\n--------\n");
        switch (CurrStage->input.type) 
        {
            case CHANNEL_FILE:
                fprintf(stdout, "     input: %s\n", CurrStage->input.val.file);
                break;
            case CHANNEL_PIPE:
                fprintf(stdout, "     input: pipe from stage %d\n", CurrStage->input.val.stage);
                break;
            default:
                fprintf(stdout, "     input: original stdin\n");
                break;
        }
        switch (CurrStage->output.type) 
        {
            case CHANNEL_FILE:
                fprintf(stdout, "    output: %s\n", CurrStage->output.val.file);
                break;
            case CHANNEL_PIPE:
                fprintf(stdout, "    output: pipe to stage %d\n", CurrStage->output.val.stage);
                break;
            default:

                fprintf(stdout, "    output: original stdout\n");
                break;
        }
        fprintf(stdout, "      argc: %d\n", CurrStage->args.argc);
        fprintf(stdout, "      argv: ");
        CurrArg = CurrStage->args.first;
        while (CurrArg) 
        {
            fprintf(stdout, "\"%s\"", CurrArg->arg);
            if (CurrArg->next)
                fprintf(stdout, ",");
            CurrArg = CurrArg->next;
        }
        fprintf(stdout, "\n");
        
        CurrStage = CurrStage->next;
        ++StageCount;
    }
  
    Stage *stageNext;
    Arg  *currArg, *next_arg;
    CurrStage = chanList->first;

    while (CurrStage != NULL) 
    {
        currArg = CurrStage->args.first;
        while (currArg != NULL) 
        {
            next_arg = currArg->next;
            free(currArg);
            currArg = next_arg;
        }
        stageNext = CurrStage->next;
        free(CurrStage);
        CurrStage = stageNext;
    }
    free(chanList);



    return 0;
}
