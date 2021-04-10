#include "report.h"
#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define _REP_DEBUG

#define MAX_TENANT 5

// type of booking in hierarchy (linux style)
static const char *TYPE[] = {
    "Conference", // 0
    "Presentation", // 1
    "Meeting", // 2
};
static const char *ROOM[] = {
    "room_A", // 0
    "room_B", // 1
    "room_C", // 2
};

void print_booking(request success[], request fail[])
{

// several constant values

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_WHITE "\x1b[0m"
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

#define SEPERATE(src, dest, ptr)                               \
    for (int i = 0; src[i].tenant[0] != 0; i++) {              \
        if (!src[i].isvalid) continue;                         \
        FOREACH_TENANT(                                        \
            ti,                                                \
            if (strcmp(TENANTS[ti], src[i].tenant) == 0) {     \
                dest[ti][ ptr[ti]++ ] = &src[i]; break;        \
            }         )                         }

// parsing and printing time
// applies to all kinds of bookings
#define PRINT_TIME(cur, sd, st, et)                           \
    strftime(sd, sizeof(sd), "%F", localtime(&(cur->start))); \
    strftime(st, sizeof(st), "%R", localtime(&(cur->start))); \
    strftime(et, sizeof(et), "%R", localtime(&(cur->end)));   \
    printf("%-12s %-7s %-7s ", sd, st, et);

    // function body begins

    const char *TENANTS[MAX_TENANT]= {"tenant_A", "tenant_B", "tenant_C", "tenant_D", "tenant_E"};
    
    request*arr[1000/MAX_TENANT][MAX_TENANT];
    int     ptr                 [MAX_TENANT];
    char s_date[15], s_time[6], e_time[6];

    puts(ANSI_GREEN "*** Room Booking - ACCEPTED ***");
    memset(ptr, 0, MAX_TENANT * sizeof(int));
    SEPERATE(success, arr, ptr)
    FOREACH_TENANT(
        ti,
        if (ptr[ti] > 0) // tenant has bookings
        {
            printf("\n%s has the following bookings:\n\n" SCSS_HEAD SEP, TENANTS[ti]);
            for (int i = 0; i < ptr[ti]; i++)
            {
                request *cur = arr[ti][i];
                PRINT_TIME(cur, s_date, s_time, e_time)

                // getting booking type, room
                // applies to other bookings except bookDevice
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

    memset(ptr, 0, MAX_TENANT * sizeof(int));
    SEPERATE(fail, arr, ptr)
    puts(ANSI_RED "*** Room Booking - REJECTED ***");
    FOREACH_TENANT(
        ti,
        if (ptr[ti] > 0) // tenant has bookings
        {
            printf("\n%s has the following bookings:\n\n" FAIL_HEAD SEP, TENANTS[ti]);
            for (int i = 0; i < ptr[ti]; i++)
            {
                request *cur = arr[ti][i];
                PRINT_TIME(arr[ti][i], s_date, s_time, e_time)

                // getting booking type, room
                // applies to other bookings except bookDevice
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
    puts("\n" END ANSI_WHITE);
}

void print_perform(request success[], request fail[]) {}

#ifdef _REP_DEBUG
int main()
{
    request s[6] = {
        {0, "tenant_E", time(NULL), time(NULL), 0,10,0,{"device_E","device_E"}, 1},
        {0, "tenant_C", time(NULL), time(NULL), 0,50,0,{"",""}, 1},
        {1, "tenant_A", time(NULL), time(NULL), 0,0,0,{"only_device",""}, 1},
        {2, "tenant_B", time(NULL), time(NULL), 0,20,0,{"device_B",""}, 1},
        {2, "tenant_E", time(NULL), time(NULL), 0,15,0,{"device_E",""}, 1},
    };
    request f[6] = {
        {0, "tenant_E", time(NULL), time(NULL), 0,10,0,{"device_E","device_E"}, 1},
        {0, "tenant_C", time(NULL), time(NULL), 0,50,0,{"",""}, 1},
        {1, "tenant_A", time(NULL), time(NULL), 0,0,0,{"only_device",""}, 1},
        {2, "tenant_B", time(NULL), time(NULL), 0,20,0,{"device_B",""}, 1},
        {2, "tenant_E", time(NULL), time(NULL), 0,15,0,{"device_E",""}, 1},
    };
    print_booking(s, f);
    return 0;
}
#endif