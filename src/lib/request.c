#include "request.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

// #define _REQ_DEBUG

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

    // very unlikely to overflow, but still possible
    struct tm *t0 = localtime(&t);

    struct tm newtime = *t0;
    newtime.tm_min += min;
    newtime.tm_hour += hr;
    return mktime(&newtime);
}

#ifdef _REQ_DEBUG
// simulates parameter parsing
int main(int argc, char **argv)
{
    char param[128];
    request *rq = malloc(sizeof(request));
    int n_param;
    int len[2]; // store hr and min (temp)
    char device1[40], device2[40];

    scanf("%[^\n\r\f\v]", param);
    printf("input: %s\n", param);
    n_param = sscanf(param, "-%s %d-%d-%d %d:%d %d.%d %d %s %s",
                     rq->tenant, DATE_DEST(rq), &len[0], &len[1],
                     &(rq->people), rq->device[0], rq->device[1]);
    parse_time(rq, len[0], len[1]);

    // only fits addMeeting/conference now...
    // below is a sample input
    // -tenant_A 2021-4-1 1:11 1.30 10 device1 device2
    printf("scanned: %d\n", n_param);
    printf("struct request: \n"
            "  tanant: %s\n"
            "  time:   %s"
            "  length@%d p@%d devices: %s %s device-match@%d\n",
           rq->tenant,
           asctime(&(rq->start)), // asctime comes with \n automatically
           rq->length, rq->people, rq->device[0], rq->device[1],
           DEVICE_PAIRING(rq));
}
#endif