#include "timeline.h"
#include "request.h"
#include <stdio.h>
#include <stdlib.h>

// initiates a timeline by setting the first & last value
node *init_timeline()
{
    request tmp1 = {0, 0, genesis, genesis};
    request tmp2 = {0, 0, eternity, eternity};
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

void remove_node(node *t)
{
    node *p = t->prev;
    node *n = t->next;
    p->next = n;
    n->prev = p;
    t = NULL;
}

node *search_request(node *begin, request *r, int direction)
{
    do
    {
        if (begin->r == r)
            return begin;
        if (direction >= 0)
            begin = begin->next;
        else
            begin = begin->prev;
    } while (begin != NULL);
}