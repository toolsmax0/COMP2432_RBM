#include "master.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#define _DEBUG
#define _INI_DEBUG
#define _REP_DEBUG

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_DEFAULT "\x1b[0m"

// global configs
room rooms[1000];
device devices[1000];
device *devices_t[1000];
int n_devices, n_rooms;
dictionary *d;

/**
 * @brief initiate all available devices from RBM.ini
 */
void init_from_ini()
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

    d = iniparser_load("RBM.ini");

    INIT(devices, quantity, devices);
    INIT(rooms, capacity, rooms);
    n_devices = num_devices;
    n_rooms = num_rooms;

#ifdef _INI_DEBUG
#define INIT_DEBUG(val, f, s)                        \
    printf("No. of " #s " available: %d\n", n_##s); \
    for (int i = 0; i < n_##s; i++)                 \
        printf("  %d: %s @%d\n", i, val[i].name, val[i].f);

    INIT_DEBUG(devices, quantity, devices);
    INIT_DEBUG(rooms, capacity, rooms);
#endif

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

// static consts that must be defined in main.c
static const int MAX_TENANT = 5;
static const char *TENANTS[] = {
    "tenant_A", 
    "tenant_B", 
    "tenant_C", 
    "tenant_D", 
    "tenant_E"
};

// type of booking in hierarchy (linux style)
const char *TYPE[] = {
    "Conference", // 0
    "Presentation", // 1
    "Meeting", // 2
};

#define RQ_ISLAST(arr, i) arr[i].tenant[0] != 0
#define RQ_ISVALID(arr, i) arr[i].isvalid
#define ADD_TO_QUEUE(src, queue, counter, src_i, queue_i) queue[queue_i][ counter[queue_i]++ ] = &src[src_i];

void print_booking(request success[], request fail[])
{

// several constant values
#define SCSS_HEAD "Date         Start   End     Type          Room     Device\n"
#define FAIL_HEAD "Date         Start   End     Type          Device\n"
#define SEP "===========================================================================\n"
#define END "================================= END =====================================\n"

// several constant functions
#define FOREACH_TENANT(i, exp) for (int i = 0; i < MAX_TENANT; i++) { exp }
#define PRINT_DEVICE(cur, offset)                                   \
    if (cur->device[0][0] == 0) printf("*\n");                      \
    else if (cur->device[1][0] == 0) printf("%s\n", cur->device[0]);\
    else printf("%s\n%" #offset "s %s\n", cur->device[0], "", cur->device[1]);

#define SEPERATE(src, dest, counter)                          \
    for (int i = 0; RQ_ISLAST(src, i); i++) {                 \
        if (!RQ_ISVALID(src, i)) continue;                    \
        FOREACH_TENANT(                                       \
            ti,                                               \
            if (strcmp(TENANTS[ti], src[i].tenant) == 0)      \
            { ADD_TO_QUEUE(src, dest, counter, i, ti) break; }\
    )}

#define PRINT_TIME(cur, sd, st, et)                           \
    strftime(sd, sizeof(sd), "%F", localtime(&(cur->start))); \
    strftime(st, sizeof(st), "%R", localtime(&(cur->start))); \
    strftime(et, sizeof(et), "%R", localtime(&(cur->end)));   \
    printf("%-12s %-7s %-7s ", sd, st, et);

    // function body begins
    // seperation queue, seperated into tenants
    request*queue[1000/MAX_TENANT][MAX_TENANT];
    // counter to store items in the queue
    int     counter               [MAX_TENANT];
    char s_date[15], s_time[6], e_time[6];

    puts(ANSI_GREEN "*** Room Booking - ACCEPTED ***");
    memset(counter, 0, MAX_TENANT * sizeof(int));
    SEPERATE(success, queue, counter)
    FOREACH_TENANT(
        ti,
        if (counter[ti] > 0)
        {
            printf("\n%s has the following bookings:\n\n" SCSS_HEAD SEP, TENANTS[ti]);
            for (int i = 0; i < counter[ti]; i++) {
                request *cur = queue[ti][i];
                PRINT_TIME(cur, s_date, s_time, e_time)

                if (cur->people != 0)
                {
                    printf("%-13s %-8s ", TYPE[ cur->priority ], rooms[ cur->roomno ].name);
                    PRINT_DEVICE(cur, 51)
                }
                else printf("%-13s %-8s %s\n", "*", "*", cur->device[0]);
            }
        }
        else printf("\n%s has no bookings.\n", TENANTS[ti]);
    )
    puts("\n" END);

    memset(counter, 0, MAX_TENANT * sizeof(int));
    SEPERATE(fail, queue, counter)
    puts(ANSI_RED "*** Room Booking - REJECTED ***");
    FOREACH_TENANT(
        ti,
        if (counter[ti] > 0)
        {
            printf("\n%s has the following bookings:\n\n" FAIL_HEAD SEP, TENANTS[ti]);
            for (int i = 0; i < counter[ti]; i++)
            {
                request *cur = queue[ti][i];
                PRINT_TIME(cur, s_date, s_time, e_time)

                if (cur->people != 0)
                {
                    printf("%-13s ", TYPE[ cur->priority ]);
                    PRINT_DEVICE(cur, 42)
                }
                else printf("%-13s %s\n", "*", cur->device[0]);
            }
        }
        else printf("\n%s has no bookings.\n", TENANTS[ti]);
    )
    puts("\n" END ANSI_DEFAULT);
}

void print_perform(request success[], request fail[])
{

    int n_scss = 0, n_fail = 0, si, fi;
    for (si = 0; RQ_ISLAST(success, si); si++) if (RQ_ISVALID(success, si)) n_scss++;
    for (fi = 0; RQ_ISLAST(fail, fi); fi++) if (RQ_ISVALID(fail, fi)) n_fail++;
    int n_total = si + fi;

    printf( ANSI_BLUE
        "\tTotal Number of Bookings Received: %-3d (%3.1f%%)\n"
        "\t      Number of Bookings Assigned: %-3d (%3.1f%%)\n"
        "\t      Number of Bookings Rejected: %-3d (%3.1f%%)\n",
        n_total, (float) 100,  
        n_scss, (float) n_scss/n_total*100,  
        n_fail, (float) n_fail/n_total*100
    );

    puts("\n\tUtilizatioion of Time Slot:");

    request*queue_r[1000/n_rooms][n_rooms];
    int     counter_r             [n_rooms];
    memset(counter_r, 0, MAX_TENANT * sizeof(int));
    for (int i = 0; RQ_ISLAST(success, i); i++)
    {
        if (!RQ_ISVALID(success, i)) continue;
        if (0 <= success[i].roomno < n_rooms)
            ADD_TO_QUEUE(success, queue_r, counter_r, i, success[i].roomno)
        else
            puts(ANSI_RED "\tERROR when printing" ANSI_BLUE);
    }

    request*queue_d[1000/n_devices][n_devices];
    int     counter_d               [n_devices];
    memset(counter_d, 0, MAX_TENANT * sizeof(int));
    for (int i = 0; RQ_ISLAST(success, i); i++)
    {
        if (!RQ_ISVALID(success, i)) continue;
        for (int j = 0; j < n_devices; j++)
        {
            if (strcmp(devices[j].name, success[i].device[0]) == 0)
                ADD_TO_QUEUE(success, queue_d, counter_d, i, j)
            if (strcmp(devices[j].name, success[i].device[1]) == 0)
                ADD_TO_QUEUE(success, queue_d, counter_d, i, j)
        }
    }

#define PRINT_UTILIZATION(arr, ptr, len, res)                       \
    for (int di = 0; di < len; di++) {                              \
        time_t s = -1, e = -1;                                      \
        int book_len = 0;                                           \
        for (int j = 0; j < ptr[di]; j++) {                         \
            request *cur = arr[di][j];                              \
            s = (s == -1 || s > cur->start) ? cur->start : s;       \
            e = (e == -1 || e < cur->end) ? cur->end : e;           \
            book_len += cur->length;                                \
        } printf("\t      %-20s - %3.1f%%\n",                       \
            res[di].name, book_len / ((float)(e - s)) * 60 * 100);  \
    }

    PRINT_UTILIZATION(queue_r, counter_r, n_rooms, rooms)
    PRINT_UTILIZATION(queue_d, counter_d, n_devices, devices)

    printf("\n\tInvalid requests(s) made: %d\n" ANSI_DEFAULT, n_total-n_scss-n_fail);
}

#ifdef _REP_DEBUG
int main()
{
    init_from_ini();
    request s[7] = {
        {0, "tenant_E", time(NULL), time(NULL) + 3600, 60, 10, 0, {"monitor_50", "monitor_75"}, 1},
        {0, "tenant_C", time(NULL), time(NULL) + 3600, 60, 50, 1, {"", ""}, 1},
        {1, "tenant_A", time(NULL), time(NULL) + 3600, 60, 0, 2, {"projector_2k", ""}, 1},
        {2, "tenant_B", time(NULL) - 7200, time(NULL) - 3600, 60, 20, 0, {"monitor_50", ""}, 1},
        {2, "tenant_E", time(NULL) - 3600, time(NULL), 60, 15, 1, {"monitor_75", ""}, 1},
    };
    request f[10] = {
        {0, "tenant_E", time(NULL), time(NULL)+3600, 60,10,0,{"monitor_50","monitor_75"}, 1},
        {0, "tenant_C", time(NULL), time(NULL)+3600, 60,50,0,{"",""}, 0}, //invalid
        {1, "tenant_A", time(NULL), time(NULL)+3600, 60,0,0,{"projector_2k",""}, 1},
        {2, "tenant_B", time(NULL), time(NULL)+3600, 60,20,0,{"monitor_50",""}, 0}, //invalid
        {2, "tenant_E", time(NULL), time(NULL)+3600, 60,15,0,{"monitor_75",""}, 1},
        {2, "tenant_E", time(NULL), time(NULL)+3600, 60,15,0,{"monitor_75",""}, 1},
    };
    print_booking(s, f);
    print_perform(s, f);
    return 0;
}
#endif

#ifndef _REP_DEBUG
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
#endif