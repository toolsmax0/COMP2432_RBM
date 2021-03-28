#pragma once

#include <stdio.h>

typedef struct {
    char name[40];
    int capacity;
}room;

// use hashtable to store equipment
typedef struct
{
    char name[40];
    int quantity;
    int occupied;
} equipment;


int insert(equipment *a[], equipment *x);

int search(equipment *a[], equipment *x);