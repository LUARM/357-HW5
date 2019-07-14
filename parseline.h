
typedef struct Arg Arg;
typedef struct Args Args;
typedef struct Channel Channel;
typedef struct Stage Stage;
typedef struct CommandLine CommandLine;
typedef struct Commands Commands;

#define COMMANDS_MAX        512

struct Args 
{
    int         argc;
    Arg         *first;
    Arg         *last;
};
struct Channel 
{
    int         type;
    union {
        int     stage;
        char    *file;
    } val;
};
struct Stage {
    Channel     input;
    Channel     output;
    Args        args;
    Stage       *next;
};

struct CommandLine {
    int         stages;
    Stage       *first;
    Stage       *last;
};

struct Commands 
{
    int         length;
    char        *items[COMMANDS_MAX];
};
struct Arg 
{
    char        *arg;
    Arg         *next;

};
CommandLine * parseLine(char *line);
void specialclean(CommandLine *cl);

