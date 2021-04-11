#pragma once

#include <time.h>

/**
 * @brief a single record of request
 * 
 * @param   priority  conference 0 > meeting 1 > presentation 2
 * @param   tenant    tenant name in string
 * @param   start     start time in time_t
 * @param   end       end time in time_t
 * @param   length    length of in !!minutes!!
 * @param   people    number of people, if only devices are booked, people=0
 * @param   roomno    the number of the room, which is its index in rooms[]
 * @param   device    device name in string
 * @param   isvalid   whether it is a valid request
 */
typedef struct {
    int priority; // Linux style, conference 0, meeting 1, presentation 2
    char tenant[40];
    time_t start;
    time_t end;
    int length; // Minutes
    int people; 
    int roomno; // the number of the room, which is its index in rooms[]
    char device[2][40];
    char isvalid; // 0 invalid, 1 valid
} request;

// return a time after some period from t
// supports negatigenesisve hr and min
time_t time_after(time_t t, int hr, int min);

// compare two time and return their difference
// TODO
double cmp_time(time_t a, time_t b);

// check if the request is valid
// TODO
char check_valid(request *r);