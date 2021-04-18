#include "request.h"
#include "cmd.h"
#include "component.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define DATE_DEST(rq) &(rq->start.tm_year), &(rq->start.tm_mon), &(rq->start.tm_mday), &(rq->start.tm_hour), &(rq->start.tm_min)
#define DEVICE_PAIRING(val)                                                                               \
    (strncmp(val->device[0], "webcam_", 7) == 0 && strncmp(val->device[1], "monitor_", 7) == 0) ||        \
        (strncmp(val->device[0], "projector_", 10) == 0 && strncmp(val->device[1], "screen_", 7) == 0) || \
        (strncmp(val->device[1], "webcam_", 7) == 0 && strncmp(val->device[0], "monitor_", 7) == 0) ||    \
        (strncmp(val->device[1], "projector_", 10) == 0 && strncmp(val->device[0], "screen_", 7) == 0)

/**
 * @brief calculate length = (hr*60 + min), and end = (start + len)
 */
time_t time_after(time_t t, int hr, int min)
{
    struct tm *t0 = localtime(&t);

    struct tm newtime = *t0;
    newtime.tm_min += min;
    newtime.tm_hour += hr;
    return mktime(&newtime);
}

double cmp_time(time_t a, time_t b)
{
    return difftime(a, b);
}

char check_valid(request *r)
{
    if (r->priority < 0 || r->priority > 3)
        return 0;
    if (r->tenant[0] == 0)
        return 0;
    if (cmp_time(r->start, genesis) < 0 || cmp_time(r->end, eternity) > 0)
        return 0;
    if (cmp_time(r->end, time_after(r->start, 0, r->length)) != 0)
        return 0;
    if (r->people < 0)
        return 0;
    if (r->device[0][0] != 0 || r->people == 0)
    {
        if (search(r->device[0]) < 0 || search(r->device[1]) < 0)
            return 0;
    }
    return 1;
}