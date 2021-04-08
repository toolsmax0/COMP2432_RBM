#include "scheduler.h"
#include "request.h"
#include "timeline.h"
#include <stdio.h>

#define _SCH_DEBUG

// timeline for a single component
// for request in requests:
//     request -> find the target component
//     slot = find_slot
//     insert into timeline by slot found

typedef struct CP_NODE_ {
    struct CP_NODE_ *prev;
    struct CP_NODE_ *next;
} cp_node;
// use linklist to get a timeline of a single component

typedef enum
{
    FCFS,
    PRIO,
    OPTI
} ALG;



int find_slot(ALG algorithm)
{
    int n = 0;

    switch (algorithm)
    {
    case FCFS:  break;
    case PRIO:  break;
    case OPTI:  break;
    
    default:
        puts("err");
        break;
    }

    return n;
}

void insert_node(cp_node *new_node, int n)
{
    // insert a node at n
}

#ifdef _SCH_DEBUG
int main()
{
    rq_node genesis, eternity;
    rq_node *rqptr;
    init_timeline(&genesis, &eternity);
    rqptr = &genesis;
}
#endif