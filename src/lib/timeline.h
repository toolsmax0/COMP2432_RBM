#pragma once

#include "request.h"

// use doubly linked list to represent the timeline
typedef struct{
    request *r;
    node *next;
    node *prev;
}node;

// the first and last node on the timeline
node genesis,eternity;