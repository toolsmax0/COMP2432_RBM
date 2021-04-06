#include "cli.h"
#include <stdio.h>
#include <string.h>

// #define _DEBUG
// #define _CLI_DEBUG

/**
 * @brief run a single cmd, returns running status
 * 
 * @param cmd short command type in int, see enum CMD
 * @param param parameters for the command
 * @return run status in int, see RUN_* definitions
 */
STATUS  run_cmd(int  cmd, char* param)
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

/**
 * @brief main CLI, with loop and exit
 * 
 */
void    cli()
{

    int cmd_int, status;
    char input[MAX_INPUT_LENGTH];
    char cmd[MAX_CMD_LENGTH], param[MAX_PARAM_LENGTH];
    do
    {
        scanf("%[^;];%*[^\f\n\r\t\v]", input);
        sscanf(input, "%s %[^;]", cmd, param);
        printf("cmd @\"%s\"/\"%s\", ", cmd, param);

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
        #ifdef _DEBUG
            printf("----DEBUG: cmd @%d, cmd|parm @%s|%s, status @%d\n", cmd_int, cmd, param, status);
        #endif
    }
    while (status != RUN_EXIT);

    printf("quit loop, exiting main program\n");

}

/**
 * @brief parse a int cmd to string
 * 
 * @param cmd short command type in int, see enum CMD
 * @return string of short command type
 */
char*   cmd_to_string(int cmd)
{
    FOREACH_CMD(RETURN_CMD_STR);
    return "INVALID";
}

/**
 * @brief parse a string cmd to string
 * 
 * @param cmd short command type in string, see enum CMD
 * @return int of short command type
 */
CMD     cmd_to_int(char* cmd)
{
    FOREACH_CMD(RETURN_CMD_INT);
    return INVALID;
}

// command syntax and explanations
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
// matches command types with param explanation
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


/**
 * @brief print out usage for a command type
 * 
 * @param cmd short command type in int, see enum CMD
 */
void    usage(int cmd)
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

#ifdef _CLI_DEBUG
    int main()
    {
        cli();
    }
#endif