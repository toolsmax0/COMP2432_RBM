#include <stdio.h>
#include <string.h>

#define MAX_CMD_LENGTH 20
#define MAX_PARAM_LENGTH 100

#define RUN_CMD_PARSING_ERROR -2
#define RUN_CMD_INVALID -1
#define RUN_CMD_EXIT 0
#define RUN_CMD_SUCCESS 1
#define RUN_CMD_RUNTIME_ERROR 2

// !!! WARNING !!! NEVER USE these two macro when variable cmd is not defined
#define RETURN_CMD_STR(e) {if (cmd == e) {return #e;}}
#define RETURN_CMD_INT(e) {if (strcmp(cmd, #e) == 0) {return e;}}

#define FOREACH_CMD(FUNCTION)       \
        FUNCTION(INVALID        )   \
        FUNCTION(addMeeting     )   \
        FUNCTION(addPresentation)   \
        FUNCTION(addConference  )   \
        FUNCTION(bookDevice     )   \
        FUNCTION(addBatch       )   \
        FUNCTION(printBookings  )   \
        FUNCTION(endProgram     )   \

#define GEN_ENUM(ENUM) ENUM,

// typedef enum {INVALID, CMD1, CMD2, CMD3, ...} CMD;
typedef enum {FOREACH_CMD(GEN_ENUM)} CMD;

char* cmd_to_string(int cmd);
int cmd_to_int(char* cmd);
void usage(int cmd);

int run_cmd(int cmd, char* param)
{
    // printf("accepted: \"%s\", ", param);

    switch (cmd)
    {
    case addMeeting         :
    case addPresentation    :
    case addConference      :
    case bookDevice         :
    case addBatch           :
    case printBookings      :
        /* code */
        return RUN_CMD_SUCCESS;

    case endProgram         :       return RUN_CMD_EXIT;
    case INVALID            :       return RUN_CMD_INVALID;
    // cmd (string) -> (int) parsing error!
    default                 :       return RUN_CMD_PARSING_ERROR;
    }
}

int main(int argc, char** argv)
{
    // printf("---- DEBUG START ----\n");
    // printf("received %d args in total,", argc);

    // for (int i = 0; i < argc; i++)
    // {
    //     printf(" \"%s\"", argv[i]);
    // }
    // printf("\ncmd in string: %s, cmd in int: %d\n", 
    //     cmd_to_string(addMeeting), cmd_to_int("addMeeting"));
    // printf("---- DEBUG END ----\n");

    int cmd_int, status;
    char cmd[MAX_CMD_LENGTH], param[MAX_PARAM_LENGTH];
    do
    {
        scanf("%s %[^;\f\n\r\t\v]", cmd, param);
        //printf("cmd @\"%s\"/\"%s\", ", cmd, param);

        cmd_int = cmd_to_int(cmd);
        usage(cmd_int);
        status = run_cmd(cmd_int, param);
        //printf("status @\"%d\"\n", status);
    }
    while (status != 0);

    printf("quit loop, exiting main program\n");
}

char* cmd_to_string(int cmd)
{
    FOREACH_CMD(RETURN_CMD_STR);
    return "INVALID";
}

int cmd_to_int(char* cmd)
{
    FOREACH_CMD(RETURN_CMD_INT);
    return INVALID;
}

// syntax and explanations
const char *SYNTAX[] =
{
    "Command                 [Parameters]                            EndSymbol     \n", //0
    "  addMeeting              -t YYYY-MM-DD hh:mm n.n p [d] [d]       ;           \n", //1
    "  addPresentation         -t YYYY-MM-DD hh:mm n.n p d d           ;           \n", //2
    "  addConference           -t YYYY-MM-DD hh:mm n.n p d d           ;           \n", //3
    "  bookDevice              -t YYYY-MM-DD hh:mm n.n d               ;           \n", //4
    "  addBatch                -f                                      ;           \n", //5
    "  printBookings           –a                                      ;           \n", //6
    "  endProgram                                                      ;           \n", //7
    "Parameter Syntax        Information                                           \n", //8
    "  t                       A tenant for booking                                \n", //9
    "                              tanant_A|tanant_B|tanant_C|tanant_D|tanant_E    \n", //0
    "  YYYY-MM-DD hh:mm        Booking start time format in ISO 8601               \n", //1
    "  n.n                     Duration in format hours.minutes                    \n", //2
    "  p                       Number of people                                    \n", //3
    "  d                       A device for booking                                \n", //4
    "                            = webcam_FHD|webcam_UHD|monitor_50|monitor_75     \n", //5
    "                              |projector_2K|projector_4K|screen_100|screen_150\n", //6
    "  d d                     Only in two combinations                            \n", //7
    "                            = webcam_* monitor_*|projector_* screen_*         \n", //8
    "  f                       File with commands                                  \n", //9
    "  a                       Algorithms used                                     \n", //0
    "                            = fcfs|prio|opti|ALL                              \n", //1
};
// syntax match-line
const int MATCH[8][11] =
{
    {21,},
    {8,9,10,11,12,13,14,15,16,17,18},
    {8,9,10,11,12,13,14,15,16,17,18},
    {8,9,10,11,12,13,14,15,16,17,18},
    {8,9,10,11,12,14,15,16,},
    {19,},
    {20,21,},
    {0,},
};

void usage(int cmd)
{
    printf("\nUsage: \n");

    if (cmd)
    {
        printf("%s", SYNTAX[cmd]);
        for (int i = 0; i < sizeof(MATCH[cmd]) / sizeof(int) && MATCH[cmd][i]; i++)
            printf("%s",SYNTAX[MATCH[cmd][i]]);
    }
    else 
    {
        for (int i = 0; i < MATCH[cmd][0]; i++)
            printf("%s", SYNTAX[i]);
    }

    printf("\n");
}