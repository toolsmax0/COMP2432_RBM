#pragma once

// a single record of request
typedef struct {
    int priority;
    char tenant[40];
    char time[20];
    int time_i;
    char len[10];
    int len_i;
    int size;
    char equip[40][2];
}request;

// get an integer from a string of time.
int get_time(char *time);