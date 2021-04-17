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



extern room rooms[];
extern device devices[];
// hashtable for devices
extern int devices_t[];
extern int home[];
// the prime number used in the hashing function
extern const int PRIME;

int hash_code(char *s);

int insert(int x);

// search for a device in the hashtable, returning its index in the device array. Return -1 if not found.
int search(char *x);