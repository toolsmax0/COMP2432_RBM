#include "opti.h"
#include "timeline.h"
#include "request.h"
#include <math.h>

int push_slot(node **ns, int len, int begin, int length, int direction)
{
    if (begin >= 0)
    {
        if (direction >= 0)
            ns[begin] = ns[begin]->next;
        else
            ns[begin] = ns[begin]->prev;
        ns[begin] = search_gap(ns[begin], length, direction);
    }
    int res = 0;
    for (int i = 1; i < len; i++)
    {
        if (direction >= 0)
        {
            if (cmp_time(ns[i]->r->start, ns[res]->r->start) < 0)
            {
                res = i;
            }
        }
        else
        {
            if (cmp_time(ns[i]->r->end, ns[res]->r->end) > 0)
            {
                res = i;
            }
        }
    }
    return res;
}

int check_newtime(node **dst, int len, int minutes, int direction, time_t *result)
{
    time_t start, end;
    if (direction >= 0)
    {
        start = dst[0]->r->end;
        end = dst[0]->next->r->start;
    }
    else
    {
        start = dst[0]->prev->r->end;
        end = dst[0]->r->start;
    }
    int tgt = 0;
    if(len>1){
        if(direction>=0&&dst[tgt]->next->next==NULL)tgt++;
        if(direction<=0&&dst[tgt]->prev->prev==NULL)tgt++;
    }
    for (int i = 1; i < len; i++)
    {
        if (direction >= 0)
        {
            if (dst[i]->next->next&&(dst[i]->r->end < dst[tgt]->r->end))
                tgt = i;
            start = start > dst[i]->r->end ? start : dst[i]->r->end;
            end = end < dst[i]->next->r->start ? end : dst[i]->next->r->start;
        }
        else
        {
            if (dst[i]->prev->prev&&(dst[i]->r->start > dst[tgt]->r->start))
                tgt = i;
            start = start > dst[i]->prev->r->end ? start : dst[i]->prev->r->end;
            end = end < dst[i]->r->start ? end : dst[i]->r->start;
        }
    }
    double time = difftime(end, start);
    if (time >= minutes * 60)
    {
        *result = start;
        return -1;
    }
    return tgt;
}

time_t find_newtime(node ***nodes, int lens[], int len, node **dst, int min, int direction)
{
    node *ns[5][1000];
    int dsti[5] = {};
    memcpy(ns, nodes, sizeof(node *) * 3 * 1000);
    for (int i = 0; i < len; i++)
    {
        dsti[i] = push_slot(ns[i], lens[i], -1, 0, direction);
        dst[i] = ns[i][dsti[i]];
    }
    time_t result = time(0);
    int flag = check_newtime(dst, len, min, direction, &result);
    while (flag != -1)
    {
        dsti[flag] = push_slot(ns[flag], lens[flag], dsti[flag], min, direction);
        dst[flag] = ns[flag][dsti[flag]];
        flag = check_newtime(dst, len, min, direction, &result);
    }
    return result;
}

int cmp4(const void *x, const void *y)
{
    request *a = *(request **)x;
    request *b = *(request **)y;
    if (a == NULL)
        return 1;
    if (b == NULL)
        return -1;
    return 0;
}

void opti_schedule(request *rqs[], request *success[], request *fail[])
{

    int s_len = 0;
    for (; success[s_len]; s_len++)
    {
        request *r = success[s_len];
        node *n = malloc(sizeof(node));
        n->r = r;
        insert_node(n, rooms[r->roomno].timeline->prev);
        if (n->next->r->start < n->r->end)
            printf("%d", n->r->length);
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
            break;
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
            break;
        }
    }
    int f_len = 0;
    for (; fail[f_len]; f_len++)
    {
        request *r = fail[f_len];
        if (r->isvalid == 0)
            continue;
        node *ns[3][1000] = {};
        int lens[5] = {};
        int len = 0;
        for (int i = 0; rooms[i].name[0]; i++)
        {
            if (rooms[i].capacity >= r->people)
            {
                ns[len][lens[len]] = search_time(rooms[i].timeline, r->start, -1);
                lens[len]++;
            }
        }
        if (lens[len])
            len++;
        if (r->device[0][0] != 0)
        {
            device *d = devices + search(r->device[0]);
            for (int i = 0; i < d->quantity; i++)
            {
                ns[len][lens[len]] = search_time(d->timelines[i], r->start, -1);
                lens[len]++;
            }
            if (lens[len])
                len++;
            if (r->device[1][0] != 0)
            {
                d = devices + search(r->device[1]);
                for (int i = 1; i < d->quantity; i++)
                {
                    ns[len][lens[len]] = search_time(d->timelines[i], r->start, -1);
                }
                if (lens[len])
                    len++;
            }
        }
        node *desn[5] = {};
        node *desp[5] = {};
        time_t newtimens = find_newtime((node ***)ns, lens, len, desn, r->length, -1);
        time_t newtimeps = find_newtime((node ***)ns, lens, len, desp, r->length, 1);
        time_t newtimene = time_after(newtimens, 0, r->length);
        time_t newtimepe = time_after(newtimeps, 0, r->length);
        if (difftime(r->start, newtimens) <= difftime(newtimeps, r->start))
        {
            r->start = newtimens;
            r->end = newtimene;
            node *tmp = desn[0];
            while (tmp->next)
                tmp = tmp->next;
            for (int i = 0; rooms[i].name[0]; i++)
            {
                if (rooms[i].timeline == tmp)
                {
                    r->roomno = i;
                    break;
                }
            }
            for (int i = 0; i < len; i++)
            {
                node *n = malloc(sizeof(node));
                n->r = r;
                insert_node(n, desn[i]->prev);
                if (n->next->r->start < n->r->end)
                    printf("%d", n->r->length);
            }
        }
        else
        {
            r->start = newtimeps;
            r->end = newtimepe;
            node *tmp = desp[0];
            while (tmp->next)
                tmp = tmp->next;
            for (int i = 0; rooms[i].name[0]; i++)
            {
                if (rooms[i].timeline == tmp)
                {
                    r->roomno = i;
                    break;
                }
            }
            for (int i = 0; i < len; i++)
            {
                node *n = malloc(sizeof(node));
                n->r = r;
                insert_node(n, desp[i]);
                if (n->next->r->start < n->r->end)
                    printf("%d", n->r->length);
            }
        }
        success[s_len++] = r;
        fail[f_len] = 0;
    }
    qsort(fail, f_len, sizeof(request *), cmp4);
    // node *a = rooms[0].timeline->prev;
    // time_t t1 = a->r->end;
    // int length = 0;
    // while (a->prev)
    // {
    //     length += a->r->length;
    //     a = a->prev;
    // }
    // time_t t2 = a->next->r->start;
    // printf("%.5f ", length * 60 / difftime(t1, t2));
    // t1 = eternity;
    // t2 = genesis;
    // length = 0;
    // for (int i = 0; i < s_len; i++)
    // {
    //     request *r = success[i];
    //     if (r->roomno == 0)
    //     {
    //         length += r->length;
    //         t1 = t1 < r->start ? t1 : r->start;
    //         t2 = t1 > r->end ? t2 : r->end;
    //     }
    // }
    // printf("%.5f\n", length * 60 / difftime(t2, t1));
    // printf("%d\n",s_len);
    // for (int i = 0; rooms[i].name[0]; i++)
    // {
    //     node *r=rooms[i].timeline->prev;
    //     while(r){
    //         printf("%d,%d,%d\n",r->r->start/60,r->r->length,r->r->end/60);
    //         r=r->prev;
    //     }
    // }

    return;
}