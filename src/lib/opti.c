#include "opti.h"
#include "prio.h"

extern device devices[];
extern room rooms[];

int roomCmp(const void *a,
            const void *b)
{
    return (*(room**)a - *(room**)b);
}

int countOccupiedDevice(request *success[],
                        int n_success,
                        char devicename[],
                        time_t start,
                        time_t end)
{
    int count=0;
    for (int i = 0; i < n_success; i++)
    {
        if((strcmp(success[i]->device[0], devicename)==0    ||
            strcmp(success[i]->device[1], devicename)==0)   &&
            ((start > success[i]->start && start<success[i]->end) ||
            (end>success[i]->start && end < success[i]->end)))
            count++;
    }
    return count;
}

int allocateRoom(request *rqs,
                 request *success[])
{
    room *sortedRooms[1000];
    int n_rooms;
    
    for (int i = 0; i<n_rooms; i++)
    {
        sortedRooms[i]=&rooms[i];
    }

    qsort(sortedRooms, n_rooms, sizeof(sortedRooms[0])*n_rooms, roomCmp);

    for (int i = 0; i < n_rooms; i++)
    {
        if (sortedRooms[i]->capacity < rqs->people) continue;

        if ((rqs->start > success[i]->start && rqs->start < success[i]->end)  ||
            (rqs->end > success[i]->start   && rqs->end < success[i]->end))
            continue;
        rqs->roomno = (sortedRooms[i] - &rooms[0]) / sizeof(sortedRooms[0]);
        return 1;
    }
    return 0;
}

void opti_schedule(request *rqs[],
                   request *success[],
                   request *fail[])
{
    request* re_success[10];
    request* re_fail[10];
    prio_schedule(*rqs, *success, *fail);
    /*
        TODO: complete the two below.
    */
    re_schedule(*fail, *re_success, *re_fail);
    diff_fail(*fail, *re_fail);//overwrite re_fail to fail
    append_success(*success, *re_success);//append re_success to success
}

int main(){

    request* success[10];
    request* fail[10];
    request rqs[3]={{0, "t1", 0,  60,  1, 15, 1, {"device1", "device2"}, 1},
                    {0, "t1", 30, 90,  1, 15, 1, {"device1", "device2"}, 1},
                    {0, "t1", 60, 120, 1, 15, 1, {"device1", "device2"}, 1}};
    request *rqsp[3];
    for (int i = 0; i < 3; i++)
    {
        rqsp[i] = &rqs[i];
    }
    opti_schedule(rqsp, success, fail);
}