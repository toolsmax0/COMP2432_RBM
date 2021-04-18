#pragma once

#include "request.h"

// schedule the requests using fcfs, and save the successsul requests in success[], failed requests in fail[].
// TODO
void fcfs_schedule(request *rqs[], request *success[], request *fail[]);