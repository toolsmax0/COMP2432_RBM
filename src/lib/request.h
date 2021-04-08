#pragma once

#include <time.h>

/**
 * @brief a single record of request
 * 
 * @param   priority  conference > meeting > presentation
 * @param   tenant    tenant name in string
 * @param   start     start time in struct tm
 * @param   end       end time in struct tm
 * @param   length    length of in !!minutes!!
 * @param   people    number of people, if only devices are booked, people=0
 * @param   device    device name in string
 */
typedef struct {
    int priority;
    char tenant[40];
    struct tm start;
    struct tm end;
    int length;
    int people;
    char device[2][40];
} request;

void cal_end(request *t);

// 删了 parser会在main.c写 这里就不需要了
// int get_time(char *time); // get an integer from a string of time.