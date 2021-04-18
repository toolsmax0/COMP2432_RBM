#include "report.h"
#include "request.h"
#include "component.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define _REP_DEBUG

extern int n_components[];
extern room rooms[];
extern device devices[];
extern tenant tenants[];

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

#define RQ_ISLAST(arr, i) arr[i].tenant[0] != 0
#define RQ_ISVALID(arr, i) arr[i].isvalid
#define ADD_TO_QUEUE(src, queue, counter, src_i, queue_i) queue[queue_i][ counter[queue_i]++ ] = &src[src_i];

#define N_TENANTS n_components[0]
#define N_ROOMS n_components[1]
#define N_DEVICES n_components[2]

void print_booking(request success[], request fail[])
{

// several constant values
#define SCSS_HEAD "Date         Start   End     Type          Room     Device\n"
#define FAIL_HEAD "Date         Start   End     Type          Device\n"
#define SEP "===========================================================================\n"
#define END "================================= END =====================================\n"

// several constant functions
#define FOREACH_TENANT(i, exp) for (int i = 0; i < N_TENANTS; i++) { exp }
#define PRINT_DEVICE(cur, offset)                                   \
    if (cur->device[0][0] == 0) printf("*\n");                      \
    else if (cur->device[1][0] == 0) printf("%s\n", cur->device[0]);\
    else printf("%s\n%" #offset "s %s\n", cur->device[0], "", cur->device[1]);

#define SEPERATE(src, dest, counter)                          \
    for (int i = 0; RQ_ISLAST(src, i); i++) {                 \
        if (!RQ_ISVALID(src, i)) continue;                    \
        FOREACH_TENANT(                                       \
            ti,                                               \
            if (strcmp(tenants[ti].name, src[i].tenant) == 0)      \
            { ADD_TO_QUEUE(src, dest, counter, i, ti) break; }\
    )}

#define PRINT_TIME(cur, sd, st, et)                           \
    strftime(sd, sizeof(sd), "%F", localtime(&(cur->start))); \
    strftime(st, sizeof(st), "%R", localtime(&(cur->start))); \
    strftime(et, sizeof(et), "%R", localtime(&(cur->end)));   \
    printf("%-12s %-7s %-7s ", sd, st, et);

    // function body begins
    // seperation queue, seperated into tenants
    request*queue[1000][1000];
    // counter to store items in the queue
    int     counter               [1000];
    char s_date[15], s_time[6], e_time[6];

    puts(ANSI_GREEN "*** Room Booking - ACCEPTED ***");
    memset(counter, 0, N_TENANTS * sizeof(int));
    SEPERATE(success, queue, counter)
    printf("%d %d %d %d %d\n", counter[0], counter[1], counter[2], counter[3], counter[4]);
    FOREACH_TENANT(
        ti,
        if (counter[ti] > 0) {
            printf("\n%s has the following bookings:\n\n" SCSS_HEAD SEP, tenants[ti].name);
            for (int i = 0; i < counter[ti]; i++)
            {
                request *cur = queue[ti][i];
                PRINT_TIME(cur, s_date, s_time, e_time)

                if (cur->people != 0)
                {
                    printf("%-13s %-8s ", TYPE[ cur->priority ], rooms[ cur->roomno ].name);
                    PRINT_DEVICE(cur, 51)
                }
                else
                    printf("%-13s %-8s %s\n", "*", "*", cur->device[0]);
            }
        } else printf("\n%s has no bookings.\n", tenants[ti].name);)
    puts("\n" END);

    memset(counter, 0, N_TENANTS * sizeof(int));
    SEPERATE(fail, queue, counter)
    puts(ANSI_RED "*** Room Booking - REJECTED ***");
    FOREACH_TENANT(
        ti,
        if (counter[ti] > 0)
        {
            printf("\n%s has the following bookings:\n\n" FAIL_HEAD SEP, tenants[ti].name);
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
        else printf("\n%s has no bookings.\n", tenants[ti].name);
    )
    puts("\n" END ANSI_DEFAULT);
}

void print_perform(request success[], request fail[])
{
    int n_scss = 0, n_fail = 0, si = 0, fi = 0;
    for (; RQ_ISLAST(success, si); si++) if (RQ_ISVALID(success, si)) n_scss++;
    for (; RQ_ISLAST(fail, fi); fi++) if (RQ_ISVALID(fail, fi)) n_fail++;
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

    request * queue_r[1000][N_ROOMS];
    int     counter_r      [N_ROOMS];
    memset(counter_r, 0, N_TENANTS * sizeof(int));
    for (int i = 0; RQ_ISLAST(success, i); i++)
    {
        if (!RQ_ISVALID(success, i)) continue;
        if (0 <= success[i].roomno < N_ROOMS)
            queue_r[success[i].roomno][ counter_r[success[i].roomno]++ ] = &success[i];
        else
            puts(ANSI_RED "\tERROR when printing" ANSI_BLUE);
    }

    request * queue_d[1000][N_DEVICES];
    int     counter_d      [N_DEVICES];
    memset(counter_d, 0, N_TENANTS * sizeof(int));
    for (int i = 0; RQ_ISLAST(success, i); i++)
    {
        if (!RQ_ISVALID(success, i)) continue;
        for (int j = 0; j < N_DEVICES; j++)
        {
            if (strcmp(devices[j].name, success[i].device[0]) == 0)
                queue_d[j][ counter_d[j]++ ] = &success[i];
            if (strcmp(devices[j].name, success[i].device[1]) == 0)
                queue_d[j][ counter_d[j]++ ] = &success[i];
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

    PRINT_UTILIZATION(queue_r, counter_r, N_ROOMS, rooms)
    PRINT_UTILIZATION(queue_d, counter_d, N_DEVICES, devices)

    printf("\n\tInvalid requests(s) made: %d\n" ANSI_DEFAULT, n_total-n_scss-n_fail);
}