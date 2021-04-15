// uncomment this macro to enable report debugging
// compile and run with 
// $ gcc report.c -o report.out; ./report.out
// #define _REPORT_DEBUG_ALONE

// NOTE: all functions are already moved to main.c
// this file along with .h file are kept for future debugging needs
#ifdef _REPORT_DEBUG_ALONE

#include "report.h"
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define _REP_DEBUG

// static consts that must be defined in main.c
static const int MAX_TENANT = 5;
static const int MAX_ROOM = 3;
static const int MAX_DEVICE = 5;
static const char *ROOM[] = {
    "room_A", // 0
    "room_B", // 1
    "room_C", // 2
};
static const char *TENANTS[] = {
    "tenant_A", 
    "tenant_B", 
    "tenant_C", 
    "tenant_D", 
    "tenant_E"
};
static const char *DEVICE[] = {
    "device_A", 
    "device_B", 
    "device_C", 
    "device_D", 
    "device_E"
};

// type of booking in hierarchy (linux style)
const char *TYPE[] = {
    "Conference", // 0
    "Presentation", // 1
    "Meeting", // 2
};

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_DEFAULT "\x1b[0m"

#define ISLAST(arr, i) arr[i].tenant[0] != 0
#define ISVALID(arr, i) arr[i].isvalid
#define ADD_TO_QUEUE(src, queue, counter, src_i, queue_i) queue[queue_i][ counter[queue_i]++ ] = &src[src_i];

void print_booking(request success[], request fail[])
{

// several constant values
#define SCSS_HEAD "Date         Start   End     Type          Room     Device\n"
#define FAIL_HEAD "Date         Start   End     Type          Device\n"
#define SEP "===========================================================================\n"
#define END "==================================END======================================\n"

// several constant functions
#define FOREACH_TENANT(i, exp) for (int i = 0; i < MAX_TENANT; i++) { exp }
#define PRINT_DEVICE(cur, offset)                                   \
    if (cur->device[0][0] == 0) printf("*\n");                      \
    else if (cur->device[1][0] == 0) printf("%s\n", cur->device[0]);\
    else printf("%s\n%" #offset "s %s\n", cur->device[0], "", cur->device[1]);

#define SEPERATE(src, dest, counter)                          \
    for (int i = 0; ISLAST(src, i); i++) {                    \
        if (!ISVALID(src, i)) continue;                       \
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
                    printf("%-13s %-8s ", TYPE[ cur->priority ], ROOM[ cur->roomno ]);
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

    for (si = 0; ISLAST(success, si); si++) if (ISVALID(success, si)) n_scss++;
    for (fi = 0; ISLAST(fail, fi); fi++) if (ISVALID(fail, fi)) n_fail++;

    int n_total = si + fi;

    printf( ANSI_BLUE
        "\tTotal Number of Bookings Received: %-3d (%3.1f%%)\n"
        "\t      Number of Bookings Assigned: %-3d (%3.1f%%)\n"
        "\t      Number of Bookings Rejected: %-3d (%3.1f%%)\n",
        n_total, (float) 100,  
        n_scss, (float) n_scss/n_total*100,  
        n_fail, (float) n_fail/n_total*100
    );

    puts("\n\tUtilizatioion of Time Slot\n");

    request*queue_r[1000/MAX_ROOM][MAX_ROOM];
    int     counter_r             [MAX_ROOM];
    memset(counter_r, 0, MAX_TENANT * sizeof(int));
    for (int i = 0; ISLAST(success, i); i++)
    {
        if (!ISVALID(success, i)) continue;
        if (0 <= success[i].roomno < MAX_ROOM)
            ADD_TO_QUEUE(success, queue_r, counter_r, i, success[i].roomno)
        else
            puts(ANSI_RED "ERROR when printing\n" ANSI_BLUE);
    }

    request*queue_d[1000/MAX_DEVICE][MAX_DEVICE];
    int     counter_d               [MAX_DEVICE];
    memset(counter_d, 0, MAX_TENANT * sizeof(int));
    for (int i = 0; ISLAST(success, i); i++)
    {
        if (!ISVALID(success, i)) continue;
        for (int j = 0; j < MAX_DEVICE; j++)
        {
            if (strcmp(DEVICE[j], success[i].device[0]) == 0)
                ADD_TO_QUEUE(success, queue_d, counter_d, i, j)
            if (strcmp(DEVICE[j], success[i].device[1]) == 0)
                ADD_TO_QUEUE(success, queue_d, counter_d, i, j)
        }
    }

#define PRINT_UTILIZATION(arr, ptr, len, conststr)                  \
    for (int di = 0; di < len; di++) {                              \
        time_t s = -1, e = -1;                                      \
        int book_len = 0;                                           \
        for (int j = 0; j < ptr[di]; j++) {                         \
            request *cur = arr[di][j];                              \
            s = (s == -1 || s > cur->start) ? cur->start : s;       \
            e = (e == -1 || e < cur->end) ? cur->end : e;           \
            book_len += cur->length;        }                       \
        printf("\t      %-20s - %3.1f%%\n",                           \
            conststr[di], book_len / ((float)(e - s)) * 60 * 100);  \
    }

    PRINT_UTILIZATION(queue_r, counter_r, MAX_ROOM, ROOM)
    PRINT_UTILIZATION(queue_d, counter_d, MAX_DEVICE, DEVICE)

    printf("\n\tInvalid requests(s) made: %d\n" ANSI_DEFAULT, n_total-n_scss-n_fail);
}

#ifdef _REP_DEBUG
int main()
{
    request s[7] = {
        {0, "tenant_E", time(NULL), time(NULL)+3600, 60,10,0,{"device_B","device_E"}, 1},
        {0, "tenant_C", time(NULL), time(NULL)+3600, 60,50,1,{"",""}, 1},
        {1, "tenant_A", time(NULL), time(NULL)+3600, 60,0,2,{"device_C",""}, 1},
        {2, "tenant_B", time(NULL)-7200, time(NULL)-3600, 60,20,0,{"device_B",""}, 1},
        {2, "tenant_E", time(NULL)-3600, time(NULL), 60,15,1,{"device_E",""}, 1},
    };
    request f[10] = {
        {0, "tenant_E", time(NULL), time(NULL)+3600, 60,10,0,{"device_B","device_E"}, 1},
        {0, "tenant_C", time(NULL), time(NULL)+3600, 60,50,0,{"",""}, 0}, //invalid
        {1, "tenant_A", time(NULL), time(NULL)+3600, 60,0,0,{"device_C",""}, 1},
        {2, "tenant_B", time(NULL), time(NULL)+3600, 60,20,0,{"device_B",""}, 0}, //invalid
        {2, "tenant_E", time(NULL), time(NULL)+3600, 60,15,0,{"device_E",""}, 1},
        {2, "tenant_E", time(NULL), time(NULL)+3600, 60,15,0,{"device_E",""}, 1},
    };
    print_booking(s, f);
    print_perform(s, f);
    return 0;
}
#endif

#endif