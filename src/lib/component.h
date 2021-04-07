#pragma once

#include <stdio.h>

typedef struct {
    char name[40];
    int capacity;
} room;

// use hashtable to store device
typedef struct
{
    char name[40];
    int quantity;
    int occupied;
} device;


int insert(device *a[], device *x);
int search(device *a[], device *x);