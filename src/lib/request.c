#include "request.h"
#include "cmd.h"
#include "component.h"
#include "iniparser.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define DATE_DEST(rq) &(rq->start.tm_year), &(rq->start.tm_mon), &(rq->start.tm_mday), &(rq->start.tm_hour), &(rq->start.tm_min)

#define DEVICE_PAIRING(val, d1, d2)                                                                  \
    (strncmp(val->device[0], d1, strlen(d1)) == 0 && strncmp(val->device[1], d2, strlen(d2)) == 0)   \
        || (strncmp(val->device[1], d1, strlen(d1)) == 0 && strncmp(val->device[0], d2, strlen(d2)) == 0)

#define DEVICE_PAIRING_1(val) DEVICE_PAIRING(val, "webcam_", "monitor_")
#define DEVICE_PAIRING_2(val) DEVICE_PAIRING(val, "projector_", "screen_")

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
    // priority not in range
    if (r->priority < 0 || r->priority > 3)
        return 0;
    // tenant not specified
    if (r->tenant[0] == 0)
        return 0;
    // time invalid
    if (cmp_time(r->start, genesis) < 0 || cmp_time(r->end, eternity) > 0)
        return 0;
    // internal err when calculating end = start + length
    if (cmp_time(r->end, time_after(r->start, 0, r->length)) != 0)
        return 0;
    // people invalid
    if (r->people < 0)
        return 0;
    // have device with an invalid device name
    if (r->device[0][0] != 0)
    {
        // convert to lowercase to conform ini standards
        strlwc(r->device[0], r->device[0], strlen(r->device[0]));
        strlwc(r->device[1], r->device[1], strlen(r->device[1]));
        // invalid device name
        if (search(r->device[0]) < 0 || (r->priority != 3 && search(r->device[1]) < 0))
            return 0;
    }
    // some priority have certain devices pairing rules
    switch (r->priority)
    {
    case 0:
        // conference need webcam & monitor
        return DEVICE_PAIRING_1(r);
    case 1:
        // presentation need projector & monitor
        return DEVICE_PAIRING_2(r);
    case 2:
        // meeting needs devices in pairs
        // but not necceesary
        return r->device[0][0] == 0 || DEVICE_PAIRING_1(r) || DEVICE_PAIRING_2(r);
    default:
        break;
    }
    return 1;
}