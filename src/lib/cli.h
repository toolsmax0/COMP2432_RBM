#pragma once

#define MAX_CMD_LENGTH 20
#define MAX_PARAM_LENGTH 100
#define MAX_INPUT_LENGTH MAX_CMD_LENGTH+MAX_PARAM_LENGTH

/**
 * @brief run status flags after running a command
 * 
 * < 0 indicates error
 * = 0 indicates intended exit system
 * > 0 indicates success
 */ 
typedef enum
{
    RUN_ERROR_PARAM         = -4,
    RUN_ERROR_RUNTIME       ,
    RUN_ERROR_PARSING       ,
    RUN_ERROR_INVALID_CMD   ,
    RUN_EXIT                = 0,
    RUN_SUCCESS
} STATUS;

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

char*   cmd_to_string   (int   cmd);
CMD     cmd_to_int      (char* cmd);
void    usage           (int   cmd);
