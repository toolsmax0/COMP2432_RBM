#include "timeline.h"
#include <stdio.h>

rq_node genesis, eternity;

// initiates a timeline by setting the first & last value
void init_timeline()
{
    genesis.next = &eternity;
    eternity.prev = &genesis;.

    genesis.r = NULL;
    genesis.prev = NULL;
    eternity.r = NULL;
    eternity.next = NULL;
}

/**
 * @brief insert a new node into the timeline
 * 
 * @param new_node new node that have be allocated already
 */
void insert_node(rq_node *new_node)
{
    rq_node *last_node = eternity.prev;
    // unlink the last node with eternity
    last_node->next = new_node;
    eternity.prev = new_node;

    // link new node to timeline
    new_node->next = &eternity;
    new_node->prev = last_node;
}