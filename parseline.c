#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parseline.h"


#define COMMANDS_MAX 512
#define CHANNEL_STD  0
#define CHANNEL_FILE 1
#define CHANNEL_PIPE 2

void insertARG(Stage *stage, char *arg) 
{
    int sizeARG = malloc(sizeof(Arg));
    if (stage->args.argc == 0) 
    {
        stage->args.first = sizeARG;
        stage->args.last = stage->args.first;
    }
    else 
    {
        stage->args.last->next = sizeARG;
        stage->args.last = stage->args.last->next;
    }
    stage->args.last->arg = arg;
    stage->args.last->next = NULL;
    ++stage->args.argc;
}

Stage * stageNEW(CommandLine *chanList, char *arg) 
{
    int sizeStage = malloc(sizeof(Stage));
    int sizeARG = malloc(sizeof(Arg));
    if (chanList->stages == 0) 
    {
        chanList->first = sizeStage;
        chanList->last = chanList->first;
    }
    else 
    {
        chanList->last->next = sizeStage;
        chanList->last = chanList->last->next;
    }
    chanList->last->input.type = CHANNEL_STD;
    chanList->last->output.type = CHANNEL_STD;
    chanList->last->args.argc = 0;
    chanList->last->args.first = NULL;
    chanList->last->args.last = NULL;
    chanList->last->next = NULL;
    ++chanList->stages;
    if (chanList->last->args.argc == 0) 
    {
        chanList->last->args.first = sizeARG;
        chanList->last->args.last = chanList->last->args.first;
    }
    else 
    {
        chanList->last->args.last->next = sizeARG;
       chanList->last->args.last = chanList->last->args.last->next;
    }
    chanList->last->args.last->arg = arg;
    chanList->last->args.last->next = NULL;
    ++chanList->last->args.argc;
    return chanList->last;
}

void specialclean(CommandLine *chanList) 
{
    Stage *CurrStage, *stageNext;
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
}


CommandLine * parseLine(char *line) 
{
    char       *token;
    CommandLine     *chanList = malloc(sizeof(CommandLine));
    Commands        commands;
    Stage      *CurrStage;
    int             stacPOSIT, stage_num;
    commands.length = 0;
    token = strtok(line, " \n");
    if (token) 
    {
        commands.items[commands.length] = token;
        ++commands.length;
    }
    else 
    {
        fprintf(stderr, "empty command line\n");


            Stage *CurrStage, *stageNext;
            Arg  *currArg, *next_arg;
            CurrStage = chanList->first;

            while (CurrStage) 
            {
                currArg = CurrStage->args.first;
                while (currArg) 
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
        
        exit(EXIT_FAILURE);
    }
    while (token) 
    {
        token = strtok(NULL, " \n");
    
        if (token) 
        {
            if (commands.length == COMMANDS_MAX) 
            {
                fprintf(stderr, "Maximum  commands \n");
                specialclean(chanList);
                exit(EXIT_FAILURE);
            }
            commands.items[commands.length] = token;
            ++commands.length;
        }
    }
    stacPOSIT = 0;
    if (commands.items[0][0]) 
    {
        if (commands.items[0][0] == '<'){
            fprintf(stderr, "bad input\n");
            specialclean(chanList);
            return 0;
          
        }
        if (commands.items[0][0] == '>'){
            fprintf(stderr, "bad output\n");
            specialclean(chanList);
            return 0;
      
        }
        if (commands.items[0][0] ==  '|'){
            fprintf(stderr, "invalid command\n");
            specialclean(chanList);
            return 0;
           
        }
        
    }
    chanList->stages = 0;
    chanList->first = NULL;
    chanList->last = NULL;
    stage_num = 0;
    CurrStage = stageNEW(chanList, commands.items[0]);
    ++stacPOSIT;
    while (stacPOSIT < commands.length) 
    {
        if (commands.length > stacPOSIT) 
        {
            switch (commands.items[stacPOSIT][0]) 
            {
                case '<':
                    if (stacPOSIT >= commands.length-1 ||
                        commands.items[stacPOSIT+1][0] == '<' ||
                        CurrStage->input.type == CHANNEL_FILE) 
                    {
                            fprintf(stderr, "%s: bad input redirection\n", CurrStage->args.first->arg);
                            specialclean(chanList);
                            return 0;
                    }
                    if (CurrStage->input.type == CHANNEL_PIPE || commands.items[stacPOSIT+1][0] == '|') 
                    {
                        fprintf(stderr, "%s: ambiguous input\n", CurrStage->args.first->arg);
                        specialclean(chanList);
                        exit(EXIT_FAILURE);
                    }
                    CurrStage->input.type = CHANNEL_FILE;
                    CurrStage->input.val.file = commands.items[++stacPOSIT];
                    break;

                case '|':
                    if (stacPOSIT >= commands.length-1 || commands.items[stacPOSIT+1][0] == '|') 
                    {
                        fprintf(stderr, "invalid null command\n");
                        specialclean(chanList);
                        exit(EXIT_FAILURE);
                    }
                    if (CurrStage->output.type != CHANNEL_STD) 
                    {
                        fprintf(stderr, "%s: ambiguous output\n", CurrStage->args.first->arg);
                        specialclean(chanList);
                        exit(EXIT_FAILURE);
                    }
                    CurrStage->output.type = CHANNEL_PIPE;
                    CurrStage->output.val.stage = stage_num + 1;
                    CurrStage = stageNEW(chanList, commands.items[++stacPOSIT]);
                    CurrStage->input.type = CHANNEL_PIPE;
                    CurrStage->input.val.stage = stage_num;
                    ++stage_num;
                    break;

                case '>':
                    if (stacPOSIT >= commands.length-1 ||
                        commands.items[stacPOSIT+1][0] == '>' ||
                        CurrStage->output.type == CHANNEL_FILE) {
                            fprintf(stderr, "%s: bad output redirection\n", CurrStage->args.first->arg);
                            specialclean(chanList);
                            exit(EXIT_FAILURE);
                    }
                    if (CurrStage->output.type == CHANNEL_PIPE || commands.items[stacPOSIT+1][0] == '|') 
                    {
                        fprintf(stderr, "%s: ambiguous output\n", CurrStage->args.first->arg);
                        specialclean(chanList);
                        return 0;
                    }
                    CurrStage->output.type = CHANNEL_FILE;
                    CurrStage->output.val.file = commands.items[++stacPOSIT];
                    break;

                default:
                    insertARG(CurrStage, commands.items[stacPOSIT]);
               
            }
        }
        ++stacPOSIT;
    }

    return chanList;
}
