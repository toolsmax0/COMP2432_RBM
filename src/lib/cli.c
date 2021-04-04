#include <stdio.h>
#include <string.h>

#define RETURN_CMD_STR(val, e) {if (val == e) {return #e;}}
#define RETURN_CMD_INT(val, e) {if (strcmp(val, #e) == 0) {return e;}}

// typedef enum {INVALID, CMD1, CMD2, CMD3, ...} CMD;
typedef enum {INVALID = 0, addMeeting, addPresentation, addConference, bookDevice, addBatch, printBookings, endProgram} CMD;

char *cmd_to_string(int value);
int cmd_to_int(char *cmd);

int main(int argc, char **argv)
{
    printf("received %d args in total,", argc);

    for (int i = 0; i < argc; i++)
    {
        printf(" \"%s\"", argv[i]);
    }
    printf("\n");
    printf("%s", cmd_to_string(addMeeting));
    printf("\n");
    printf("%d", cmd_to_int("addMeeting"));
    printf("\n");

}

char* cmd_to_string(int cmd) {
    RETURN_CMD_STR( cmd     , addMeeting        );
    RETURN_CMD_STR( cmd     , addPresentation   );
    RETURN_CMD_STR( cmd     , addConference     );
    RETURN_CMD_STR( cmd     , bookDevice        );
    RETURN_CMD_STR( cmd     , addBatch          );
    RETURN_CMD_STR( cmd     , printBookings     );
    RETURN_CMD_STR( cmd     , endProgram        );
    RETURN_CMD_STR( INVALID , INVALID           );
}

int cmd_to_int(char *cmd) {
    RETURN_CMD_INT( cmd     , addMeeting        );
    RETURN_CMD_INT( cmd     , addPresentation   );
    RETURN_CMD_INT( cmd     , addConference     );
    RETURN_CMD_INT( cmd     , bookDevice        );
    RETURN_CMD_INT( cmd     , addBatch          );
    RETURN_CMD_INT( cmd     , printBookings     );
    RETURN_CMD_INT( cmd     , endProgram        );
    RETURN_CMD_INT( INVALID , INVALID           );
}