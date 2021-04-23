#include "master.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// #define _DEBUG

// collection of num_tenants, num_rooms, num_devices
int n_components[3];
tenant tenants[1000];

room rooms[1000];
device devices[1000];
int devices_t[1000];
int home[1000];
const int PRIME = 997;
request requests[10000];
FILE *IStreams[100];
int isi = 0;
time_t genesis;
time_t eternity;
int requestno;

void schedule(int);
int openBatch(char *s);
/**
 * @brief initiate all available devices from RBM.ini
 */
int init_from_ini()
{
#define INIT(val, f, s)                                 \
    int n_##s = iniparser_getsecnkeys(d, #s);           \
    const char *name_##s[n_##s];                        \
    iniparser_getseckeys(d, #s, name_##s);              \
    for (int i = 0; i < n_##s; i++)                     \
    {                                                   \
        sscanf(name_##s[i], #s ":%s", val[i].name);     \
        val[i].f = iniparser_getint(d, name_##s[i], 0); \
    }
#define _INIT_DEBUG(val, f, s)                      \
    printf("No. of " #s " available: %d\n", n_##s); \
    for (int i = 0; i < n_##s; i++)                 \
        printf("  %d: %s @%d\n", i, val[i].name, val[i].f);

    dictionary *d = iniparser_load("RBM.ini");
    INIT(devices, quantity, devices);
    INIT(rooms, capacity, rooms);
    INIT(tenants, enabled, tenants);

    n_components[0] = n_tenants,
    n_components[1] = n_rooms,
    n_components[2] = n_devices;

#ifdef _DEBUG
    _INIT_DEBUG(devices, quantity, devices);
    _INIT_DEBUG(rooms, capacity, rooms);
    _INIT_DEBUG(tenants, enabled, tenants);
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
EXE run_cmd(int cmd, char *param, request *rq, int *newreq)
{
    // printf("accepted: \"%s\", ", param);
    int n_param, duration[2];
    struct tm s; // start time

#define HANDLE_PARAM_ERR \
    if (!rq->isvalid)    \
        return RUN_ERROR_PARAM;
#define SCAN_PARAM_FOR_ADD_FUNCTIONS(rq, s, len)                                        \
    n_param = sscanf(                                                                   \
        param, "-%s %d-%d-%d %d:%d %d.%d %d %s %s",                                     \
        rq->tenant, &(s.tm_year), &(s.tm_mon), &(s.tm_mday), &(s.tm_hour), &(s.tm_min), \
        &len[0], &len[1], &rq->people, rq->device[0], rq->device[1]);

#define SCAN_PARAM_POSTPROCESS(rq, s, len)           \
    s.tm_year -= 1900;                               \
    s.tm_mon -= 1;                                   \
    s.tm_sec = 0;                                    \
    rq->start = mktime(&s);                          \
    rq->end = time_after(rq->start, len[0], len[1]); \
    rq->roomno = -1;                                 \
    rq->length = 60 * len[0] + len[1];

    switch (cmd)
    {
    case addMeeting:
        *newreq = 1;
        SCAN_PARAM_FOR_ADD_FUNCTIONS(rq, s, duration)
        rq->priority = 2;
        rq->isvalid = (n_param == 11) || (n_param == 9);
        SCAN_PARAM_POSTPROCESS(rq, s, duration)
        rq->isvalid &= check_valid(rq);

        HANDLE_PARAM_ERR
        // addMeeting executions
        puts("executing addMeeting");
        break;

    case addPresentation:
        *newreq = 1;
        SCAN_PARAM_FOR_ADD_FUNCTIONS(rq, s, duration)
        rq->priority = 1;
        rq->isvalid = (n_param == 11);
        SCAN_PARAM_POSTPROCESS(rq, s, duration)
        rq->isvalid &= check_valid(rq);

        HANDLE_PARAM_ERR
        // addPresentation executions
        puts("executing addPresentation");
        break;

    case addConference:
        *newreq = 1;
        SCAN_PARAM_FOR_ADD_FUNCTIONS(rq, s, duration)
        rq->priority = 0;
        rq->isvalid = (n_param == 11);
        SCAN_PARAM_POSTPROCESS(rq, s, duration)
        rq->isvalid &= check_valid(rq);

        HANDLE_PARAM_ERR
        // addConference executions
        puts("executing addConference");
        break;

    case bookDevice:
        *newreq = 1;
        n_param = sscanf(
            param, "-%s %d-%d-%d %d:%d %d.%d %s",
            rq->tenant, &s.tm_year, &s.tm_mon, &s.tm_mday, &s.tm_hour, &s.tm_min,
            &duration[0], &duration[1], rq->device[0]);
        rq->device[1][0] = 0;
        rq->isvalid = (n_param == 9);
        rq->priority = 3;
        rq->people = 0;
        SCAN_PARAM_POSTPROCESS(rq, s, duration)
        rq->isvalid &= check_valid(rq);

        HANDLE_PARAM_ERR
        // bookDevice executions
        puts("executing bookDevice");
        break;

    case addBatch:;

        char filename[40];
        n_param = sscanf(param, "-%s", filename);
        if (n_param != 1)
            return RUN_ERROR_PARAM;
        // the above also affects the following return status RUN_ERROR_PARAM
        puts("executing addBatch");
        return openBatch(filename);
        break;

    case printBookings:;
        char algo[20];
        sscanf(param, "-%s", algo);
        int type = 0;
        switch (algo[0])
        {
        case 'f':
            if (strcmp(algo, "fcfs"))
                return RUN_ERROR_PARAM;
            type = 1;
            break;
        case 'p':
            if (strcmp(algo, "prio"))
                return RUN_ERROR_PARAM;
            type = 2;
            break;
        case 'o':
            if (strcmp(algo, "opti"))
                return RUN_ERROR_PARAM;
            type = 3;
            break;
        case 'A':
            if (strcmp(algo, "ALL\0"))
                return RUN_ERROR_PARAM;
            type = 4;
            break;
        default:
            return RUN_ERROR_PARAM;
        }
        schedule(type);
        break;

    case endProgram:
        return RUN_EXIT;
    case INVALID:
        return RUN_ERROR_INVALID_CMD;
    // cmd (string) -> (int) parsing error!
    default:
        return RUN_ERROR_PARSING;
    }
    return RUN_SUCCESS;
}

void init()
{
    init_from_ini();
    struct tm genesis_s = {0, 0, 0, 1, 0, 0};
    struct tm eternity_s = {0, 0, 0, 1, 0, 130};
    genesis = mktime(&genesis_s);
    eternity = mktime(&eternity_s);
    IStreams[0] = stdin;
    memset(devices_t, -1, sizeof(devices_t));
    for (int i = 0; devices[i].name[0] != 0; i++)
    {
        insert(i);
        node **timelines=calloc(devices[i].quantity,sizeof(node*));
        for (int j = 0; j < devices[i].quantity; j++)
            timelines[j] = init_timeline();
        devices[i].timelines = timelines;
    };
    for (int i = 0; rooms[i].name[0] != 0; i++)
    {
        rooms[i].timeline = init_timeline();
    }
}

int main()
{
    init();
    // struct tm tmp = {tm_year : 2021-1900, tm_mon : 4-1, tm_mday : 1};
    // time_t t1 = mktime(&tmp);
    // time_t t2 = time_after(t1, 2, 0);
    // request tmp0 = {1, "tenant_a", t1, t2, 120, 5,isvalid:1};
    // request tmp1 = {0, "test tenant2", t1, t2, 120, 15, 0, "webcam_fhd", "screen_150",isvalid:1};
    // request tmp2 = {3, "device", t1, t2, 120, 0, 0, "webcam_fhd", "screen_150",isvalid:1};
    // request *test[] = {&tmp0, &tmp1, &tmp2,0};
    // request *success[1000]={};
    // request *fail[1000]={};
    // fcfs_schedule(test, success, fail);
    // print_booking(success,fail,"FCFS");
    // print_perform(success,fail,"FCFS");
    // opti_schedule(test, success, fail);
    // schedule(4);
    // return 0;

    int cmd_int, execution;
    char input[MAX_INPUT_LENGTH];
    char cmd[MAX_CMD_LENGTH], param[MAX_PARAM_LENGTH];
    do
    {
        printf("RBM# ");
        char st[1000] = {};
        char check[100] = {};
        fgets(st, 200, stdin);
        if (sscanf(st, "%[^;]%s", input, check) == EOF)
        {
            if (!isi)
            {
                puts("ERROR: No more commands to be read, exiting.");
                return -1;
            }
            fclose(IStreams[isi--]);
            stdin = IStreams[isi];
            continue;
        }
        if (*check != ';')
        {
            puts("Syntax Error: Missing Semi-Column, Skipping.");
        }
        sscanf(input, "%s %[^;]", cmd, param);
        cmd_int = cmd_to_int(cmd);
        int newreq = 0;
        // < 0 then error occured
        if ((execution = run_cmd(cmd_int, param, requests + requestno, &newreq)) < RUN_EXIT)
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
        if (newreq)
            requestno++;
#ifdef _DEBUG
        printf("----DEBUG: cmd @%d, cmd|parm @%s|%s, execution @%d\n", cmd_int, cmd, param, execution);
#endif
    } while (execution != RUN_EXIT);

    printf("quit loop, exiting main program\n");
}

int cmp(const void *x, const void *y)
{
    request *a = *(request **)x;
    request *b = *(request **)y;
    return a->priority - b->priority;
}

int cmp2(const void *x, const void *y)
{
    request *a = *(request **)x;
    request *b = *(request **)y;
    int first = strcmp(a->tenant, b->tenant);
    if (first)
        return first;
    return cmp_time(a->start, b->start);
}
int cmp3(const void *x, const void *y)
{
    request *a = *(request **)x;
    request *b = *(request **)y;
    return cmp_time(a->start, b->start);
}
/**
 * Inter-Process signal:
 *  Parent to Child:
 *      1:fcfs scheduling;
 *      2:prio scheduling;
 *      3:opti scheduling;
 *      4:opti scheduling with preprocessed data;
 *      5:print scheduling result;
 *      6:print scheduling analysis;
 *      7:fetch preprocessed data
 *      8:exit;
 *  Child to Parent:
 *      1: current job finished;
 **/
void schedule(int algo)
{
    int cid = 0, child = 1;
    int pipes[10][2][2] = {};
    int readc[10] = {}, writec[10] = {};
    char ibuf[200] = {}, obuf[200] = {};
    request *req_p[10000] = {};
    int req_len;
    for (req_len = 0; requests[req_len].tenant[0]; req_len++)
    {
        req_p[req_len] = requests + req_len;
    }
    int readp = 0, writep = 0;
    if (algo == 4)
        child = 3;
    for (int i = 0; i < child; i++)
    {
        int flag = 0;
        flag |= pipe(pipes[i][0]) < 0;
        flag |= pipe(pipes[i][1]) < 0;
        cid = fork();
        if (cid < 0 || flag < 0)
        {
            puts("Fatal: fork/pipe failed.");
            for (int j = 0; j < i; j++)
            {
                write(writec[j], "\10", 1);
                wait(0);
            }
            return;
        }
        else if (cid)
        {
            printf("Child %d, PID %d.\n", i, cid);
            close(pipes[i][0][0]);
            writec[i] = pipes[i][0][1];
            readc[i] = pipes[i][1][0];
            close(pipes[i][1][1]);
        }
        else
        {
            readp = pipes[i][0][0];
            close(pipes[i][0][1]);
            close(pipes[i][1][0]);
            writep = pipes[i][1][1];
            break;
        }
    }
    if (cid)
    {
        switch (algo)
        {
        case 1:
        case 2:
        case 3:
            obuf[0] = (char)algo;
            obuf[1] = '\5';
            obuf[2] = '\10';
            write(writec[0], obuf, 3);
            wait(0);
            close(writec[0]);
            close(readc[0]);
            break;
        case 4:
            write(writec[0], "\1", 1);
            write(writec[1], "\2", 1);
            write(writec[2], "\4", 1);
            read(readc[0], ibuf, 1);
            write(writec[0], "\5", 1);
            read(readc[0], ibuf, 1);
            read(readc[1], ibuf, 1);
            write(writec[1], "\5", 1);
            read(readc[1], ibuf, 1);
            write(writec[1], "\7", 1);
            for (int j = 0; j < 2; j++)
            {
                read(readc[1], ibuf, sizeof(int32_t));
                int num = *(int32_t *)ibuf;
                write(writec[2], ibuf, sizeof(int32_t));
                if (num == -1)
                    num = 0;
                for (int i = 0; i < num; i++)
                {
                    read(readc[1], ibuf, sizeof(request *));
                    write(writec[2], ibuf, sizeof(request *));
                    read(readc[1], ibuf, sizeof(int));
                    write(writec[2], ibuf, sizeof(int));
                }
            }
            read(readc[1], ibuf, 1);
            read(readc[2], ibuf, 1);
            write(writec[2], "\5", 1);
            read(readc[2], ibuf, 1);
            puts("\n\n\x1b[34m*** Room Booking Manager – Summary Report ***");
            puts("Performance:");
            write(writec[0], "\6", 1);
            read(readc[0], ibuf, 1);
            write(writec[1], "\6", 1);
            read(readc[1], ibuf, 1);
            write(writec[2], "\6", 1);
            read(readc[2], ibuf, 1);
            for (int i = 0; i < 3; i++)
            {
                write(writec[i], "\10", 1);
                wait(0);
                close(writec[0]);
                close(readc[0]);
            }
            break;
        }
    }
    else
    {
        request *success[10000] = {}, *fail[10000] = {};
        char *dict[] = {"", "FCFS", "PRIO", "OPTI"};
        char *type;
        int len;
        while (read(readp, ibuf, 1))
        {
            char c = ibuf[0];
            switch (c)
            {
            case 1:
                type = dict[1];
                fcfs_schedule(req_p, success, fail);
                write(writep, "\1", 1);
                break;
            case 2:
                type = dict[2];
                qsort(req_p, req_len, sizeof(request *), cmp);
                fcfs_schedule(req_p, success, fail);
                write(writep, "\1", 1);
                break;
            case 3:
                type = dict[3];
                qsort(req_p, req_len, sizeof(request *), cmp);
                fcfs_schedule(req_p, success, fail);
                for(len=0;success[len];len++);
                qsort(success,len,sizeof(request*),cmp3);
                for(len=0;fail[len];len++);
                qsort(fail,len,sizeof(request*),cmp);
                opti_schedule(req_p, success, fail);
                write(writep, "\1", 1);
                break;
            case 4:;
                type = dict[3];
                int32_t num;
                read(readp, ibuf, sizeof(int32_t));
                num = *(int32_t *)ibuf;
                for (int i = 0; i < num; i++)
                {
                    read(readp, ibuf, sizeof(request *));
                    success[i] = *(request **)ibuf;
                    read(readp, ibuf, sizeof(int));
                    success[i]->roomno = *(int *)ibuf;
                }
                qsort(success,num,sizeof(request*),cmp3);
                read(readp, ibuf, sizeof(int32_t));
                num = *(int32_t *)ibuf;
                for (int i = 0; i < num; i++)
                {
                    read(readp, ibuf, sizeof(request *));
                    fail[i] = *(request **)ibuf;
                    read(readp, ibuf, sizeof(int));
                    fail[i]->roomno = *(int *)ibuf;
                }
                qsort(fail,num,sizeof(request*),cmp);
                opti_schedule(req_p, success, fail);
                write(writep, "\1", 1);
                break;
            case 5:;
                for (len = 0; success[len]; len++)
                    ;
                qsort(success, len, sizeof(request *), cmp2);
                for (len = 0; fail[len]; len++)
                    ;
                qsort(fail, len, sizeof(request *), cmp2);

                print_booking(success, fail, type);
                write(writep, "\1", 1);
                break;
            case 6:
                print_perform(success, fail, type);
                write(writep, "\1", 1);
                break;
            case 7:;
                for (num = 0; success[num]; num++)
                    ;
                if (!num)
                    num = -1;
                *(int32_t *)obuf = num;
                write(writep, obuf, sizeof(int32_t));
                for (int i = 0; i < num; i++)
                {
                    *(request **)obuf = success[i];
                    write(writep, obuf, sizeof(request *));
                    *(int *)obuf = success[i]->roomno;
                    write(writep, obuf, sizeof(int));
                }
                for (num = 0; fail[num]; num++)
                    ;
                if (!num)
                    num = -1;
                *(int32_t *)obuf = num;
                write(writep, obuf, sizeof(int32_t));
                for (int i = 0; i < num; i++)
                {
                    *(request **)obuf = fail[i];
                    write(writep, obuf, sizeof(request *));
                    *(int *)obuf = fail[i]->roomno;
                    write(writep, obuf, sizeof(int));
                }
                write(writep, "\1", 1);
                break;
            case 8:
                close(writep);
                close(readp);
                exit(0);
            }
        }
    }
}

int openBatch(char *s)
{
    char names[100][100] = {};
    FILE *files[100] = {};
    int fi = 0;
    int p[2];
    if (pipe(p) < 0)
    {
        puts("Fatal : pipe failed.");
        return RUN_ERROR_PARAM;
    }
    if (fork())
    {
        char ss[10000];
        int n = 0;
        int i = 0;
        do
        {
            n = read(p[0], ss + i, 1000);
            i += n;
        } while (ss[i-1]!=-1);
        ss[i-1]=0;
        int ptr=0;
        for (int i = 0; sscanf(ss+ptr, "%s%n", names[i],&n) != EOF; i++)
        {
            ptr+=n;
            FILE *f = fopen(names[i], "r");
            if (!f)
            {
                printf("Failed to open %s.\n", names[i]);
                return RUN_ERROR_PARAM;
            }
            files[fi++] = f;
        }
        for (fi--; fi >= 0; fi--)
        {
            if (isi >= 99)
            {
                puts("ERROR:You have opened too many batch files, exiting. Is their a recursive reference?");
                return -1;
            }
            IStreams[++isi] = files[fi];
        }
        stdin=IStreams[isi];
        close(p[0]);
        close(p[1]);
        wait(0);
        return RUN_SUCCESS;
    }
    else
    {
        printf("%d\n",getpid());
        // sleep(10);
        dup2(p[1],1);
        char ss[1000];
        sprintf(ss,"ls %s",s);
        system(ss);
        // write(p[1],ss,strlen(ss));
        write(p[1],"\xff",1);
        close(p[0]);
        close(p[1]);
        exit(0);
    }
}