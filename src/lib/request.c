#include "request.h"
#include "cmd.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define _REQ_DEBUG

#define DATE_DEST(rq) &(rq->start.tm_year), &(rq->start.tm_mon), &(rq->start.tm_mday), &(rq->start.tm_hour), &(rq->start.tm_min)
#define DEVICE_PAIRING(val)                                 \
    (strncmp(val->device[0],"webcam_",7)==0 && strncmp(val->device[1],"monitor_",7)==0) || \
    (strncmp(val->device[0],"projector_",10)==0 && strncmp(val->device[1],"screen_",7)==0) || \
    (strncmp(val->device[1],"webcam_",7)==0 && strncmp(val->device[0],"monitor_",7)==0) || \
    (strncmp(val->device[1],"projector_",10)==0 && strncmp(val->device[0],"screen_",7)==0)

/**
 * @brief calculate length = (hr*60 + min), and end = (start + len), stored in ptr
 */
time_t time_after(time_t t, int hr, int min)
{
    struct tm *t0 = localtime(&t);

    struct tm newtime = *t0;
    newtime.tm_min += min;
    newtime.tm_hour += hr;
    return mktime(&newtime);
}

// move this into main.c later
// merge with run_cmd() before any execution
void parse_request(CMD cmd, char *param, request *rq)
{
    struct tm s;
    int n_param;
    int len[2];

    if (addMeeting <= cmd <= addConference)
    {
        n_param = sscanf(
            param, "-%s %d-%d-%d %d:%d %d.%d %d %s %s",
            rq->tenant, &(s.tm_year), &(s.tm_mon), &(s.tm_mday), &(s.tm_hour), &(s.tm_min),
            &len[0], &len[1], &rq->people, rq->device[0], rq->device[1]
        );
        rq->isvalid = (n_param == 11);
    }
    switch (cmd)
    {
    case addMeeting:
        // device is optional
        rq->isvalid |= (n_param == 9);
        rq->priority = 2;
        break;
    case addPresentation:
        rq->priority = 1;
        break;
    case addConference:
        rq->priority = 0;
        break;
    case bookDevice:
        n_param = sscanf(
            param, "-%s %d-%d-%d %d:%d %d.%d %s",
            rq->tenant, &s.tm_year, &s.tm_mon, &s.tm_mday, &s.tm_hour, &s.tm_min,
            &len[0], &len[1], rq->device[0]
        );
        rq->device[1][0] = 0;
        rq->isvalid = (n_param == 9);
        rq->priority = 4;
        rq->people = 0;
        break;

    case addBatch:
        break;
    case printBookings:
        break;
    // case endProgram
    // no parsing is needed, so ignored.
    default:
        // exit directly
        // if cmd is others
        return;
    }
    if (addMeeting <= cmd <= bookDevice)
    {
        s.tm_year -= 1900;
        s.tm_mon -= 1;
        s.tm_sec = 0;
        rq->start = mktime(&s);
        rq->end = time_after(rq->start, len[0], len[1]);
        rq->roomno = -1;
        rq->length = 60 * len[0] + len[1];
    }
    

    // printf("scanned: %-2d ", n_param);
    // printf(
    //         "tanant: %s length@%-3d p@%-2d devices: %-10s %-10s device-match@%d isvalid:%d time: %s",
    //        rq->tenant, rq->length, rq->people, rq->device[0], rq->device[1],
    //        DEVICE_PAIRING(rq), rq->isvalid,
    //        asctime(localtime(&rq->start)) // asctime comes with \n automatically
    // );
}

#ifdef _REQ_DEBUG
// simulates parameter parsing
int main()
{
    char param[128];
    request *rq = malloc(sizeof(request));

    parse_request(addMeeting, "-tenant_A 2021-4-1 1:11 1.30 10", rq);
    parse_request(addPresentation, "-tenant_B 2021-4-1 1:11 1.30 10 device1 device2", rq);
    parse_request(addConference, "-tenant_C 2021-4-1 1:11 1.30 10 device1 device2", rq);
    parse_request(bookDevice, "-tenant_D 2021-4-1 1:11 1.30 device1", rq);
    parse_request(addBatch, "-xxxx", rq);
    parse_request(printBookings, "-fcfs", rq);
    
}
#endif