#include "master.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

room rooms[1000];

equipment eq[1000];

equipment *eq_t[1000];

/**
 * @brief run a single cmd, returns running status
 * 
 * @param cmd short command type in int, see enum CMD
 * @param param parameters for the command
 * @return run status in int, see RUN_* definitions
 */
STATUS run_cmd(int cmd, char *param)
{
    // printf("accepted: \"%s\", ", param);

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

    FILE *fp = fopen("RBM.ini", "r");
    {
        // read in the config
    }

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
        } while (status != RUN_EXIT);

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