#include "master.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#define _DEBUG

/**
 * @brief initialize a target
 * 
 * @param val allocated varaiable name
 * @param f varaible fieldname storing its capacity/quantity
 * @param s section name of the keys in the ini file
 */
#define INIT(val, f, s)                                     \
    int n_##s = iniparser_getsecnkeys(d, #s);               \
    const char *name_##s[n_##s];                            \
    iniparser_getseckeys(d, #s, name_##s);                  \
    for (int i = 0; i < n_##s; i++)                         \
    {                                                       \
        sscanf(name_##s[i], #s":%s", val[i].name);          \
        val[i].f = iniparser_getint(d, name_##s[i], 0);     \
    }
#define _INIT_DEBUG(val, f, s)                              \
    printf("No. of " #s " available: %d\n", n_##s);         \
    for (int i = 0; i < n_##s; i++)                         \
        printf("  %d: %s @%d\n", i, val[i].name, val[i].f);

room rooms[1000];
device devices[1000];
device *devices_t[1000];

/**
 * @brief initiate all available devices from RBM.ini
 * 
 * @param 
 */ 
int init_from_ini()
{
    dictionary *d = iniparser_load("RBM.ini");
    INIT(devices, quantity, devices);
    INIT(rooms, capacity, rooms);

#ifdef _DEBUG
    _INIT_DEBUG(devices, quantity, devices);
    _INIT_DEBUG(rooms, capacity, rooms);
#endif

    return 0;
}

/**
 * @brief run a single cmd, returns execution status
 * 
 * @param   cmd     short command type in int, see enum CMD
 * @param   param   parameters for the command
 * @return  execution status in int, see enum EXE in lib/cmd.h
 */
EXE run_cmd(int cmd, char *param)
{
    // printf("accepted: \"%s\", ", param);
    int n_param;

    switch (cmd)
    {
    case addMeeting:
    case addPresentation:
    case addConference:
    case bookDevice:
    case addBatch:
    case printBookings:
        /* code */
        // if param not correct,
        return RUN_ERROR_PARAM;
        // return RUN_SUCCESS;

    case endProgram:
        return RUN_EXIT;
    case INVALID:
        return RUN_ERROR_INVALID_CMD;
    // cmd (string) -> (int) parsing error!
    default:
        return RUN_ERROR_PARSING;
    }
}

int main()
{
    init_from_ini();

    // points to the pipe
    int readp, writep;
    int pid = getpid(), cid;
    // pipes
    int p[4] = {};
    int flag = 0;

    //return if any one of the pipe creations fails
    flag |= pipe(p) < 0;
    flag |= pipe(p + 2) < 0;
    if (flag)
        return -1;

    cid = fork();
    if (cid < 0)
        return -2;

    // parent process
    else if (cid > 0)
    {
        // close unneccessary pipes and assign the rest to variables for best readability.
        close(p[0]);
        writep = p[1];
        readp = p[2];
        close(p[3]);

        int cmd_int, execution;
        char input[MAX_INPUT_LENGTH];
        char cmd[MAX_CMD_LENGTH], param[MAX_PARAM_LENGTH];
        do
        {
            scanf("%[^;];%*[^\f\n\r\t\v]", input);
            sscanf(input, "%s %[^;]", cmd, param);

            cmd_int = cmd_to_int(cmd);
            // < 0 then error occured
            if ((execution = run_cmd(cmd_int, param)) < RUN_EXIT)
            {
                switch (execution)
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
            printf("----DEBUG: cmd @%d, cmd|parm @%s|%s, execution @%d\n"
                , cmd_int, cmd, param, execution);
#endif
        } while (execution != RUN_EXIT);

        printf("quit loop, exiting main program\n");
    }

    // child process
    else
    {
        // close unneccessary pipes and assign the rest to variables for best readability.
        readp = p[0];
        close(p[1]);
        close(p[2]);
        writep = p[3];
    }
    return 0;
}