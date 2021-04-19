#pragma once

#include <stdbool.h>

#include "request.h"
#include "component.h"

// schedule the requests using the optimal strategy, and save the successsul requests in success[], failed requests in fail[].
// TODO
void opti_schedule(request *rqs[],
                   request *success[],
                   request *fail[]);

int allocateRoom(request *rqs,
                 request *success[]);

int roomCmp(const void *a,
            const void *b);

int countOccupiedDevice(request *success[],
                        int n_success,
                        char devicename[],
                        time_t start,
                        time_t end);

int re_schedule(request *fail[],
                request *re_success[],
                request *re_fail[]);

int diff_fail(request *fail[],
              request *re_fail[]);

int append_success(request *success[],
                   request *re_success);