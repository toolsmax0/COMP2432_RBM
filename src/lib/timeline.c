#include "timeline.h"
#include "request.h"
#include <stdio.h>
#include <stdlib.h>

// initiates a timeline by setting the first & last value
node *init_timeline()
{
    request tmp1 = {0, 0, start : genesis, end : genesis};
    request tmp2 = {0, 0, start : eternity, end : eternity};
    request *r1 = malloc(sizeof(request));
    request *r2 = malloc(sizeof(request));
    *r1 = tmp1;
    *r2 = tmp2;
    node *n1 = malloc(sizeof(node));
    node *n2 = malloc(sizeof(node));
    n1->r = r1, n2->r = r2;
    n1->next = n2, n2->next = 0;
    n1->prev = 0, n2->prev = n1;
    return n2;
}

/**
 * @brief insert a new request node into the timeline
 * 
 * @param new_node new node that have be allocated already
 */
void insert_node(node *newnode, node *target)
{
    if (newnode == NULL || target == NULL)
    {
        printf("insert_node(%x, %x): Invalid Arguments.\n", newnode, target);
        return;
    }
    node *next = target->next;
    target->next = newnode;
    newnode->prev = target;
    newnode->next = next;
    next->prev = newnode;
    if(newnode->r->end>next->r->start){
        puts("111");
    }
}

void remove_node(node *t)
{
    if (t == NULL)
        return;
    node *p = t->prev;
    node *n = t->next;
    p->next = n;
    n->prev = p;
    t = NULL;
}

node *search_request(node *begin, request *r, int direction)
{
    if (begin == NULL)
        return NULL;
    do
    {
        if (begin->r == r)
            return begin;
        if (direction >= 0)
            begin = begin->next;
        else
            begin = begin->prev;
    } while (begin != NULL);
    return NULL;
}

node *search_time(node *begin, time_t t, int direction)
{
    if (direction < 0)
        begin = begin->prev;
    node *next = begin->next;
    while (begin != NULL && next != NULL)
    {
        if (cmp_time(begin->r->start, t) < 0 && cmp_time(next->r->start, t) >= 0)
        {
            if (begin->prev == NULL)
                return next;
            return begin;
        }
        if (direction >= 0)
            begin = begin->next, next = next->next;
        else
            begin = begin->prev, next = next->prev;
    }
    return NULL;
}

node *search_slot(node *begin, time_t start, time_t end, int direction)
{
    node *next = begin;
    begin = search_time(begin, start, direction);
    next = search_time(next, end, direction);
    if (begin == next)
        return begin;
    return NULL;
}

node *search_gap(node *begin, int min, int direction)
{
    if (direction < 0)
        begin = begin->prev;
    node *next = begin->next;
    if (direction >= 0)
    {
        while (begin && next && cmp_time(next->r->start, time_after(begin->r->end, 0, min)) < 0)
        {
            begin = begin->next;
            next = next->next;
        }
        return begin;
    }
    else
    {
        while (begin->r->end > time_after(next->r->start, 0, -min))
        {
            begin = begin->prev;
            next = next->prev;
        }
        return next;
    }
    if (begin == NULL || next == NULL)
        return NULL;
    if (direction >= 0)
        return begin;
    return next;
}