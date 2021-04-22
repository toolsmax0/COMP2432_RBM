#include "opti.h"
#include "timeline.h"
#include "request.h"

void opti_schedule(request *rqs[], request *success[], request *fail[])
{
    return;
    int s_len = 0;
    for (; success[s_len]; s_len++)
    {
        request *r = success[s_len];
        node *n = malloc(sizeof(node));
        insert_node(n, rooms[r->roomno].timeline->prev);
        if (r->device[0][0] == 0)
            continue;
        int index = search(r->device[0]);
        n = malloc(sizeof(node));
        n->r = r;
        device *d = devices + index;
        for (int i = 0; i < devices[index].quantity; i++)
        {
            if (cmp_time(r->start, d->timelines[i]->prev->r->end) < 0)
                continue;
            insert_node(n, d->timelines[i]->prev);
        }
        if (r->device[1][0] == 0)
            continue;
        index = search(r->device[1]);
        n = malloc(sizeof(node));
        n->r = r;
        d = devices + index;
        for (int i = 0; i < devices[index].quantity; i++)
        {
            if (cmp_time(r->start, d->timelines[i]->prev->r->end) < 0)
                continue;
            insert_node(n, d->timelines[i]->prev);
        }
    }
    return;
}