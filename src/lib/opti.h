#pragma once

#include "request.h"
#include "timeline.h"
#include "component.h"

// schedule the requests using the optimal strategy, and save the successsul requests in success[], failed requests in fail[].
// TODO
void opti_schedule(request *rqs[],
                   request *success[],
                   request *fail[]);