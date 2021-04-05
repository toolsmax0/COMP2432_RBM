#include <stdio.h>
#include <string.h>

#define MAX_CMD_LENGTH 20
#define MAX_PARAM_LENGTH 100

// run status flags
// <0 means error occured
// =0 means exit system
// >0 means successfule
#define RUN_ERROR_PARAM -4
#define RUN_ERROR_RUNTIME -3
#define RUN_ERROR_PARSING -2
#define RUN_ERROR_INVALID_CMD -1
#define RUN_EXIT 0
#define RUN_SUCCESS 1

// !!! WARNING !!! NEVER USE these two macro when variable cmd is not defined
#define RETURN_CMD_STR(e) {if (cmd == e) {return #e;}}
#define RETURN_CMD_INT(e) {if (strcmp(cmd, #e) == 0) {return e;}}

// for-each macro, defines all command heads
#define FOREACH_CMD(f)       \
        f(INVALID        )   \
        f(addMeeting     )   \
        f(addPresentation)   \
        f(addConference  )   \
        f(bookDevice     )   \
        f(addBatch       )   \
        f(printBookings  )   \
        f(endProgram     )   \

// typedef enum {INVALID, CMD1, CMD2, CMD3, ...} CMD;
#define GEN_ENUM(e) e,
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
        // if param not correct,
        return RUN_ERROR_PARAM;
        // return RUN_SUCCESS;

    case endProgram         :       return RUN_EXIT;
    case INVALID            :       return RUN_ERROR_INVALID_CMD;
    // cmd (string) -> (int) parsing error!
    default                 :       return RUN_ERROR_PARSING;
    }
}

int main(int argc, char** argv)
{

    int cmd_int, status;
    char cmd[MAX_CMD_LENGTH], param[MAX_PARAM_LENGTH];
    do
    {
        scanf("%s %[^;]", cmd, param);
        // \f\n\r\t\v for normal endings, may be needed when handling a bat file
        // printf("cmd @\"%s\"/\"%s\", ", cmd, param);

        cmd_int = cmd_to_int(cmd);
        // < 0 then error occured
        if ((status = run_cmd(cmd_int, param)) < RUN_EXIT)
        {
            switch (status)
            {
            case RUN_ERROR_PARAM:
            case RUN_ERROR_INVALID_CMD:
                // intended, two cases with same handling
                usage(cmd_int);
                break;
            case RUN_ERROR_RUNTIME:
            case RUN_ERROR_PARSING:
                puts("this is a bug");
                break;

            default:
                puts("Error detected.");
                break;
            }
        }
        // printf("status @\"%d\"\n", status);
    }
    while (status != RUN_EXIT);

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
    "                            = tanant_A|tanant_B|tanant_C|tanant_D|tanant_E    \n", //10
    "  YYYY-MM-DD hh:mm        Booking start time format in ISO 8601               \n", //11
    "  n.n                     Duration in format hours.minutes                    \n", //12
    "  p                       Number of people                                    \n", //13
    "  d                       A device for booking                                \n", //14
    "                            = webcam_FHD|webcam_UHD|monitor_50|monitor_75     \n", //15
    "                              |projector_2K|projector_4K|screen_100|screen_150\n", //16
    "  d d                     Only in two combinations                            \n", //17
    "                            = webcam_* monitor_*|projector_* screen_*         \n", //18
    "  f                       File with commands                                  \n", //19
    "  a                       Algorithms used                                     \n", //20
    "                            = fcfs|prio|opti|ALL                              \n", //21
};
// syntax match-line
// matches command heads with param body
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


// generate help msg for each cmd
// if cmd not valid then print all available msg
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