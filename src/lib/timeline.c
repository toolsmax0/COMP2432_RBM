#include "timeline.h"
#include <stdio.h>
#include <stdlib.h>

// initiates a timeline by setting the first & last value
node *init_timeline()
{
    struct tm genesis = {0, 0, 0, 1, 0, 0};
    struct tm eternity = {0, 0, 0, 1, 0, 130};
    time_t t1 = mktime(&genesis);
    time_t t2 = mktime(&eternity);
    request tmp1 = {0, 0, t1, t1};
    request tmp2 = {0, 0, t2, t2};
    request *r1 = malloc(sizeof(request));
    request *r2 = malloc(sizeof(request));
    *r1 = tmp1;
    *r2 = tmp2;
    node *n1 = malloc(sizeof(node));
    node *n2 = malloc(sizeof(node));
    n1->r = r1, n2->r = r2;
    n1->next = n2, n2->next = 0;
    n1->prev = 0, n2->prev = n1;
    return n1;
}

/**
 * @brief insert a new request node into the timeline
 * 
 * @param new_node new node that have be allocated already
 */
void insert_node(node *newnode, node *target)
{
    node *next = target->next;
    target->next = newnode;
    newnode->prev = target;
    newnode->next = next;
    next->prev = newnode;
}