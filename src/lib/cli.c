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

int run_cmd(int cmd, char* param)
{
    printf("accepted: \"%s\", ", param);

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
    printf("---- DEBUG START ----\n");
    printf("received %d args in total,", argc);

    for (int i = 0; i < argc; i++)
    {
        printf(" \"%s\"", argv[i]);
    }
    printf("\ncmd in string: %s, cmd in int: %d\n", cmd_to_string(addMeeting), cmd_to_int("addMeeting"));
    printf("---- DEBUG END ----\n");

    int status;
    char cmd[MAX_CMD_LENGTH], param[MAX_PARAM_LENGTH];
    do
    {
        scanf("%s %[^\f\n\r\t\v]", cmd, param);
        printf("cmd @\"%s\"/\"%s\", ", cmd, param);

        int cmd_int = cmd_to_int(cmd);

        status = run_cmd(cmd_int, param);
        printf("status @\"%d\"\n", status);
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