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
// TODO
void remove_node(node *target);
