#include "master.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#define _DEBUG

room rooms[1000];
device devices[1000];
// hashtable for devices
device *devices_t[1000];
int n_devices, n_rooms;

/**
 * @brief initiate all available devices from RBM.ini
 */
int init_from_ini()
{
// initialize a target, val:variablename
// val: variablename, f: field, s: ini section
#define INIT(val, f, s)                                 \
    int num_##s = iniparser_getsecnkeys(d, #s);         \
    const char *name_##s[num_##s];                      \
    iniparser_getseckeys(d, #s, name_##s);              \
    for (int i = 0; i < num_##s; i++) {                 \
        sscanf(name_##s[i], #s ":%s", val[i].name);     \
        val[i].f = iniparser_getint(d, name_##s[i], 0); \
    }
#define _INIT_DEBUG(val, f, s)                        \
    printf("No. of " #s " available: %d\n", num_##s); \
    for (int i = 0; i < num_##s; i++)                 \
        printf("  %d: %s @%d\n", i, val[i].name, val[i].f);

    dictionary *d = iniparser_load("RBM.ini");

    INIT(devices, quantity, devices);
    INIT(rooms, capacity, rooms);
    n_devices = num_devices;
    n_rooms = num_rooms;


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
    for (int i = 0; devices[i].name[0] != 0; i++)
    {
        insert(devices_t, i);
        node *timelines[devices[i].quantity];
        for (int j = 0; j < devices[i].quantity; j++)
            timelines[j] = init_timeline();
        devices[i].timelines = timelines;
    };
    for (int i = 0; rooms[i].name[0] != 0; i++)
    {
        rooms[i].timeline = init_timeline();
    }

    struct tm tmp = {tm_year : 2021, tm_mon : 4, tm_mday : 1};
    time_t t1 = mktime(&tmp);
    time_t t2 = time_after(t1, 2, 0);
    request tmp0 = {1, "test tenant", t1, t2, 120, 5};
    request tmp1 = {0, "test tenant2", t1, t2, 120, 15, 0, "webcam_FHD", "screen_100"};
    request tmp2 = {3, "device", t1, t2, 120, 0, 0, "webcam_FHD", "screen_100"};
    request test[] = {tmp0, tmp1, tmp2};
    request *success[1000];
    request *fail[1000];
    fcfs_schedule(test, success, fail);
    prio_schedule(test, success, fail);
    opti_schedule(test, success, fail);
    return 0;

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
            printf("----DEBUG: cmd @%d, cmd|parm @%s|%s, execution @%d\n", cmd_int, cmd, param, execution);
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