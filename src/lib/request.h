#pragma once

#include <time.h>

/**
 * @brief a single record of request
 * 
 * @param   priority  conference > meeting > presentation > device only
 * @param   tenant    tenant name in string
 * @param   start     start time in struct tm
 * @param   end       end time in struct tm
 * @param   length    length of in !!minutes!!
 * @param   people    number of people, if only devices are booked, people=0
 * @param   device    device name in string
 * @param   isvalid   whether it is a valid request
 */
typedef struct {
    int priority; // Linux style, 0 for conference and 3 for device booking
    char tenant[40];
    time_t start;
    time_t end;
    int length; // Minutes
    int people; // 0 means device booking
    int roomno; // the number of the room, which is its index in rooms[]
    char device[2][40];
    char isvalid; // 0 if invalid, 1 otherwise
} request;

// return a time after some period from t
// supports negatigenesisve hr and min
// hr and min does not need to fit in the scale, for example, hr=25,min=70 is valid
time_t time_after(time_t t, int hr, int min);

// compare two time and return their difference (a-b)
// TODO
double cmp_time(time_t a, time_t b);

// check if the request is valid
// TODO
char check_valid(request *r);