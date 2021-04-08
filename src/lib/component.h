#pragma once

#include <stdio.h>
#include "timeline.h"

typedef struct {
    char name[40];
    int capacity;
    node *timeline;
} room;

// use hashtable to store device
typedef struct
{
    char name[40];
    int quantity;
    node **timelines;
} device;


// TODO
int insert(device *a[], int x);

// search for a device in the hashtable, returning its index in the device array. Return -1 if not found.
int search(device *a[], device *x);