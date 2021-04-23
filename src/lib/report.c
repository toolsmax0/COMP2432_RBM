#include "report.h"
#include "request.h"
#include "component.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _REPORT_DEBUG

extern int n_components[];
extern room rooms[];
extern device devices[];

// type of booking in hierarchy (linux style)
static const char *TYPE[] = {
    "Conference", // 0
    "Presentation", // 1
    "Meeting", // 2
};

// max number of requests for a tenant
#define MAX_RECORD_PER_TENANT 100
#define MAX_RECORD_PER_ROOM 1000
#define MAX_RECORD_PER_DEVICE 1000
// max number of tenants
#define MAX_TENANT 100

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_DEFAULT "\x1b[0m"

#define RQ_ISLAST(arr, i) arr[i] != 0
#define RQ_ISVALID(arr, i) arr[i]->isvalid
#define ADD_TO_QUEUE(src, queue, counter, src_i, queue_i, max) if (counter[queue_i] < max) queue[queue_i][ counter[queue_i]++ ] = src[src_i];

#define N_ROOMS n_components[1]
#define N_DEVICES n_components[2]

void print_booking(request *success[], request *fail[], char *algo)
{

// several constant values
#define SCSS_HEAD "Date         Start   End     Type          Room     Device\n"
#define FAIL_HEAD "Date         Start   End     Type          Device\n"
#define SEP "===========================================================================\n"
#define END "================================= END =====================================\n"

// several constant functions
#define PRINT_DEVICE(cur, offset)                                              \
    if (cur->device[0][0] == 0) printf("*\n");                                 \
    else if (cur->device[1][0] == 0) printf("%s\n", cur->device[0]);           \
    else printf("%s\n%" #offset "s %s\n", cur->device[0], "", cur->device[1]);

#define SEPERATE(src, dest, counter)                                                  \
    for (int i = 0; RQ_ISLAST(src, i); i++) {                                         \
        if (!RQ_ISVALID(src, i)) continue;                                            \
        for (int ti = 0; ti < MAX_TENANT; ti++) {                                     \
            if (dest[ti][0] == 0 || strcmp(dest[ti][0]->tenant, src[i]->tenant) == 0) \
            { ADD_TO_QUEUE(src, dest, counter, i, ti, MAX_RECORD_PER_TENANT) break; } \
        }                                                                             \
    }

#define PRINT_TIME(cur, sd, st, et)                           \
    strftime(sd, sizeof(sd), "%F", localtime(&(cur->start))); \
    strftime(st, sizeof(st), "%R", localtime(&(cur->start))); \
    strftime(et, sizeof(et), "%R", localtime(&(cur->end)));   \
    printf("%-12s %-7s %-7s ", sd, st, et);

    // function body begins
    // seperation queue, seperated into tenants
    request*queue[MAX_RECORD_PER_TENANT][MAX_TENANT];
    // counter to store items in the queue
    int     counter          [MAX_TENANT];
    char s_date[15], s_time[6], e_time[6];

    memset(queue, 0, sizeof(queue[0][0]) * MAX_RECORD_PER_TENANT * MAX_TENANT);
    memset(counter, 0, sizeof(counter[0]) * MAX_TENANT);
    printf(ANSI_GREEN "*** Room Booking - ACCEPTED / %s ***\n", algo );
    SEPERATE(success, queue, counter)
    for (int ti = 0; ti < MAX_TENANT && counter[ti] > 0; ti++)
    {
        printf("\n%s has the following bookings:\n\n" SCSS_HEAD SEP, queue[ti][0]->tenant);
        for (int i = 0; i < counter[ti]; i++)
        {
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
    puts("\n" END);

    memset(queue, 0, sizeof(queue[0][0]) * MAX_RECORD_PER_TENANT * MAX_TENANT);
    memset(counter, 0, sizeof(counter[0]) * MAX_TENANT);
    SEPERATE(fail, queue, counter)
    printf(ANSI_RED "*** Room Booking - REJECTED / %s ***\n", algo );
    for (int ti = 0; ti < MAX_TENANT && counter[ti] > 0; ti++)
    {
        printf("\n%s has the following bookings:\n\n" FAIL_HEAD SEP, queue[ti][0]->tenant);
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
    puts("\n" END ANSI_DEFAULT);

    #ifdef _REPORT_DEBUG
    printf(ANSI_YELLOW);
    // print out the invalid ones
#define PRINT_INVALID_RQ(arr)                                                                                                                   \
    for (int i = 0; RQ_ISLAST(arr, i); i++)                                                                                                     \
    {                                                                                                                                           \
        if (RQ_ISVALID(arr, i))                                                                                                                 \
            continue;                                                                                                                           \
        printf("INVALID in" #arr ": %s %d %d %s %s\n", arr[i]->tenant, arr[i]->priority, arr[i]->people, arr[i]->device[0], arr[i]->device[1]); \
    }

    PRINT_INVALID_RQ(success)
    PRINT_INVALID_RQ(fail)
    #endif
}

void print_perform(request *success[], request *fail[], char *algo)
{
    int n_scss = 0, n_fail = 0, si = 0, fi = 0;
    for (; RQ_ISLAST(success, si); si++) if (RQ_ISVALID(success, si)) n_scss++;
    for (; RQ_ISLAST(fail, fi); fi++) if (RQ_ISVALID(fail, fi)) n_fail++;
    int n_total = si + fi;

    printf( ANSI_BLUE
        "For %s:\n"
        "\tTotal Number of Bookings Received: %-3d (%3.1f%%)\n"
        "\t      Number of Bookings Assigned: %-3d (%3.1f%%)\n"
        "\t      Number of Bookings Rejected: %-3d (%3.1f%%)\n",
        algo,
        n_total, (float) 100,  
        n_scss, (float) n_scss/n_total*100,  
        n_fail, (float) n_fail/n_total*100
    );

    puts("\n\tUtilizatioion of Time Slot:");

    request * queue_r[MAX_RECORD_PER_ROOM][N_ROOMS];
    int     counter_r                     [N_ROOMS];
    memset(queue_r, 0, sizeof(queue_r[0][0]) * MAX_RECORD_PER_ROOM * N_ROOMS);
    memset(counter_r, 0, sizeof(counter_r[0]) * N_ROOMS);
    for (int i = 0; RQ_ISLAST(success, i); i++)
    {
        if (!RQ_ISVALID(success, i)) continue;
        if (0 <= success[i]->roomno < N_ROOMS)
            ADD_TO_QUEUE(success, queue_r, counter_r, i, success[i]->roomno, MAX_RECORD_PER_ROOM)
        else
            puts(ANSI_RED "\tERROR when printing" ANSI_BLUE);
    }

    request * queue_d[MAX_RECORD_PER_DEVICE][N_DEVICES];
    int     counter_d                       [N_DEVICES];
    memset(queue_d, 0, sizeof(queue_d[0][0]) * MAX_RECORD_PER_DEVICE * N_DEVICES);
    memset(counter_d, 0, sizeof(counter_d[0]) * N_DEVICES);
    for (int i = 0; RQ_ISLAST(success, i); i++)
    {
        if (!RQ_ISVALID(success, i)) continue;
        for (int j = 0; j < N_DEVICES; j++)
            if (strcmp(devices[j].name, success[i]->device[0]) == 0
                    || strcmp(devices[j].name, success[i]->device[1]) == 0)
            { ADD_TO_QUEUE(success, queue_d, counter_d, i, j, MAX_RECORD_PER_DEVICE) }
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
            res[di].name, book_len *6000/difftime(e,s) );  \
    }

    PRINT_UTILIZATION(queue_r, counter_r, N_ROOMS, rooms)
    PRINT_UTILIZATION(queue_d, counter_d, N_DEVICES, devices)

    printf("\n\tInvalid requests(s) made: %d\n" ANSI_DEFAULT, n_total-n_scss-n_fail);
}