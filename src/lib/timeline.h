#pragma once

#include "request.h"

/**
 * @brief request node in a doubly linked list
 * 
 * @param   request   a single request
 * @param   prev      pointer to previous request node
 * @param   next      pointer to next request node
 */
typedef struct NODE_ {
    request *r;
    struct NODE_ *prev;
    struct NODE_ *next;
} node;


// The first and last node are border guards, NEVER insert anything out of them.
node*    init_timeline();

// insert the newnode after the target node
void insert_node(node* newnode,node* target);

// remove the target node
void remove_node(node *target);

// search for a node with the specified request starting from the given node
// if direction>=0, search forwards, otherwise search backwards
// return null if not found
node *search_request(node *begin,request *r, int direction);


// search for a begin time between two nodes, and return the address of the former node
// begin indicates the starting node of search
// if direction>=0, search forwards, otherwise search backwards
// return null if not found
node *search_time(node *begin,time_t t, int direction);

// search for a time slot between two nodes, returning the address of the former node.
// search forwards if direction>=0, vice versa
// return null if not found
node *search_slot(node *begin, time_t start, time_t end, int direction);


// search for a time gap between two nodes, returning the address of the node closer to begin.
// min is the length of the gap in minutes
// search forwards if direction>=0, vice versa
// return null if not found
node *search_gap(node *begin,int min,int direction);