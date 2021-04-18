#pragma once

#include <stdbool.h>
#include "component.h"
#include "request.h"


// schedule the requests using fcfs, and save the successsul requests in success[], failed requests in fail[].
// TODO
void fcfs_schedule(request *rqs[], request *success[], request *fail[], int n_request);

/**
  @brief    try to allocate a room for a request and return the result: 0 if success, 1 if fail
  @param    rqs         the pointer of the request
  @param    success     requests which have been adapted successfully
  @return   0 if success in allocating a room for the request, 1 otherwise

 */
int allocateRoom(request *rqs, request *success[]);


/**
  @brief    the comparator for qsort
  @param    a           pointer of the first room
  @param    b           pointer of the second room
  @return   capacity of a - capacity of b
 */
int roomCmp(const void *a,const void *b);

/**
  @brief    count the number of certain devices which are occupied during certain time
  @param    success     requests which have been adapted successfully
  @param    n_success   the number of requests which have been adapted successfully
  @param    devicename  name of the device
  @param    start       start time of the booking of the device
  @param    end         start time of the booking of the device
  @return   capacity of a - capacity of b
 */
int countOccupiedDevice(request *success[],int n_success, char devicename[], time_t start, time_t end);