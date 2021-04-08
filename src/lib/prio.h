#pragma once

#include "request.h"

// schedule the requests using priority, and save the successsul requests in success[], failed requests in fail[].
// TODO
void prio_schedule(request rqs[], request *success[], request *fail[]);