#pragma once

#include "request.h"

/**
 * @brief request node in a doubly linked list
 * 
 * @param   request   a single request
 * @param   prev      pointer to previous request node
 * @param   next      pointer to next request node
 */
typedef struct RQ_NODE_ {
    request *r;
    struct RQ_NODE_ *prev;
    struct RQ_NODE_ *next;
} rq_node;

// 移到.c了 不然compile不了
// // the first and last node on the timeline
// node    genesis;
// node    eternity;

void    init_timeline();
void    add_request(rq_node *new_node);