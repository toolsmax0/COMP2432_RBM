#pragma once

#include "request.h"

// use doubly linked list to represent the timeline
typedef struct NODE_{
    request *r;
    struct NODE_ *next;
    struct NODE_ *prev;
}node;

// the first and last node on the timeline
node genesis,eternity;