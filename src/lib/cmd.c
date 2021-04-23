#include "cmd.h"
#include <stdio.h>
#include <string.h>

#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_DEFAULT "\x1b[0m"

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
static const char *SYNTAX[] =
{
    "[Command]               [Parameters]                            EndSymbol     \n", //0
    "  addMeeting              -t YYYY-MM-DD hh:mm n.n p [d1 d2]       ;           \n", //1
    "  addPresentation         -t YYYY-MM-DD hh:mm n.n p d1 d2         ;           \n", //2
    "  addConference           -t YYYY-MM-DD hh:mm n.n p d1 d2         ;           \n", //3
    "  bookDevice              -t YYYY-MM-DD hh:mm n.n d1              ;           \n", //4
    "  addBatch                -f                                      ;           \n", //5
    "  printBookings           -a                                      ;           \n", //6
    "  endProgram                                                      ;           \n", //7
    "[Parameter Syntax]      [Information]                                         \n", //8
    "  t                       A tenant for booking                                \n", //9
    "                            = tenant_A|tenant_B|tenant_C|tenant_D|tenant_E    \n", //10
    "  YYYY-MM-DD hh:mm        Booking start time format in ISO 8601               \n", //11
    "  n.n                     Duration in format hours.minutes                    \n", //12
    "  p                       Number of people                                    \n", //13
    "  d1/d2                   A device for booking                                \n", //14
    "                            = webcam_FHD|webcam_UHD|monitor_50|monitor_75     \n", //15
    "                              |projector_2K|projector_4K|screen_100|screen_150\n", //16
    "  d1 d2                   Only in two combinations                            \n", //17
    "                            = webcam_* monitor_*|projector_* screen_*         \n", //18
    "  f                       File with commands                                  \n", //19
    "  a                       Algorithms used                                     \n", //20
    "                            = fcfs|prio|opti|ALL                              \n", //21
};
// matches command types with param explanation
static const int MATCH[8][12] =
{
    {22,0},
    {8,9,10,11,12,13,14,15,16,17,18,0},
    {8,9,10,11,12,13,14,15,16,17,18,0},
    {8,9,10,11,12,13,14,15,16,17,18,0},
    {8,9,10,11,12,14,15,16,0},
    {8,19,0},
    {8,20,21,0},
    {0},
};

/**
 * @brief print out usage for a command type
 * 
 * @param cmd short command type in int, see enum CMD
 */
void    usage(int cmd)
{
    puts(ANSI_MAGENTA "\nUsage: ");

    if (cmd)
    {
        printf("%s%s\n", SYNTAX[0], SYNTAX[cmd]);
        for (int i = 0; i < sizeof(MATCH[cmd]) / sizeof(int) && MATCH[cmd][i]; i++)
            printf("%s",SYNTAX[MATCH[cmd][i]]);
    }
    else 
    {
        for (int i = 0; i < MATCH[cmd][0]; i++)
            printf("%s", SYNTAX[i]);
    }

    puts(ANSI_DEFAULT);
}
