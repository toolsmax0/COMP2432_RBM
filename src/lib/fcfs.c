#include "fcfs.h"
#include "timeline.h"
#include "request.h"
#include "component.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern device devices[];
extern room rooms[];

int countOccupiedDevice(request *success[],int n_success, char devicename[], time_t start, time_t end){
    int count=0;
    for (int i = 0; i < n_success; i++)
    {
        if((strcmp(success[i]->device[0],devicename)==0||strcmp(success[i]->device[1],devicename)==0)&&
        ((start>=success[i]->start && start<=success[i]->end)||(end>=success[i]->start && end<=success[i]->end))) count++;
    }
    return count;
}



int roomCmp(const void *a,const void *b){
    return (*(room**)a - *(room**)b);
}

int allocateRoom(request *rqs, request *success[]){

    room *sortedRooms[1000]={};

    int n_rooms=0;
    
    for (; n_rooms < 1000&&rooms[n_rooms].name[0]!=0; n_rooms++);
    

    for (int i = 0; i<n_rooms; i++)
    {
        sortedRooms[i]=&rooms[i];
    }

    qsort(sortedRooms, n_rooms, sizeof(sortedRooms[0]), roomCmp);

    for (int i = 0; i < n_rooms; i++)
    {
        if (sortedRooms[i]->capacity<rqs->people) continue;

        int roomno;
        roomno=(sortedRooms[i] - &rooms[0]);

        int flag=0;
        for (int j = 0; success[j]!=0; j++)
        {
            if (success[j]->roomno==roomno
                &&((rqs->start>=success[j]->start && rqs->start<=success[j]->end)||(rqs->end>=success[j]->start && rqs->end<=success[j]->end))){
                    flag=1;
                    break;
            } 
        }
        if (flag==0)
        {
            rqs->roomno= roomno;
            return 1;
        }
    }
    return 0;
}



void fcfs_schedule(request *rqs[], request *success[], request *fail[]){

    int n_success=0;
    int n_fail=0;

    for (int i = 0; rqs[i]!=0; i++)
    {

        if (rqs[i]->isvalid==0){
            fail[n_fail]=rqs[i];
            n_fail++;
            continue;
        }

        if (allocateRoom(rqs[i],success)==0)
        {
            fail[n_fail]=rqs[i];
            n_fail++;
            continue;
        }

        if (rqs[i]->device[0][0]==0)
        {
            success[n_success]=rqs[i];
            n_success++;

        }
        else{
            if (countOccupiedDevice(success,n_success,rqs[i]->device[0],rqs[i]->start,rqs[i]->end)<devices[search(rqs[i]->device[0])].quantity&&
            countOccupiedDevice(success,n_success,rqs[i]->device[1],rqs[i]->start,rqs[i]->end)<devices[search(rqs[i]->device[1])].quantity)
            {
                success[n_success]=rqs[i];
                n_success++;
            }

            else{
                fail[n_fail]=rqs[i];
                n_fail++;
            } 
        }
    }
}