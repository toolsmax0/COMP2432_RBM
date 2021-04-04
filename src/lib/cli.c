#include <stdio.h>
#include <stdlib.h>

#define RETURN_CMD_STR(val, e) {if (val == e) {return #e;}}
#define RETURN_CMD_INT(val, e) {if (strcmp(val, #e) == 0) {return e;}}

// typedef enum {INVALID, CMD1, CMD2, CMD3, ...} CMD;
typedef enum {INVALID = 0, addMeeting, addPresentation, addConference, bookDevice, addBatch, printBookings, endProgram} CMD;

char *cmd_to_string(int cmd);
int cmd_to_int(char *cmd);

int run_cmd(int cmd)
{
    switch (cmd)
    {
    case addMeeting        :
    case addPresentation   :
    case addConference     :
    case bookDevice        :
    case addBatch          :
    case printBookings     :
        /* code */
        return 1;
    case endProgram        :
        return 0;
    case INVALID           :
        // cmd parsing is wrong
        return -2;

    default:
        // cmd (string) -> (int) parsing error!
        return -1;
    }
}

int main(int argc, char **argv)
{
    printf("---- DEBUG START ----\n");
    printf("received %d args in total,", argc);

    for (int i = 0; i < argc; i++)
    {
        printf(" \"%s\"", argv[i]);
    }
    printf("\n");
    printf("cmd in string: %s, cmd in int: %d\n", cmd_to_string(addMeeting), cmd_to_int("addMeeting"));
    printf("---- DEBUG END ----\n");

    int loop = 1;
    while (loop != 0)
    {
        scanf("%d", &loop);
        printf("input @%d, ", loop);

        loop = run_cmd(loop);
        printf("execute result @%d\n", loop);
    }
    printf("quit loop, exiting main program\n");
}

char* cmd_to_string(int cmd)
{
    RETURN_CMD_STR( cmd     , addMeeting        );
    RETURN_CMD_STR( cmd     , addPresentation   );
    RETURN_CMD_STR( cmd     , addConference     );
    RETURN_CMD_STR( cmd     , bookDevice        );
    RETURN_CMD_STR( cmd     , addBatch          );
    RETURN_CMD_STR( cmd     , printBookings     );
    RETURN_CMD_STR( cmd     , endProgram        );
    RETURN_CMD_STR( INVALID , INVALID           );
}

int cmd_to_int(char *cmd)
{
    RETURN_CMD_INT( cmd     , addMeeting        );
    RETURN_CMD_INT( cmd     , addPresentation   );
    RETURN_CMD_INT( cmd     , addConference     );
    RETURN_CMD_INT( cmd     , bookDevice        );
    RETURN_CMD_INT( cmd     , addBatch          );
    RETURN_CMD_INT( cmd     , printBookings     );
    RETURN_CMD_INT( cmd     , endProgram        );
    RETURN_CMD_INT( INVALID , INVALID           );
}