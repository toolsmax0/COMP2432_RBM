#include "fcfs.h"
#include "timeline.h"
#include "request.h"
#include "component.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

device devices[3]={{"device1",2,NULL},{"device2",2,NULL},{"device3",2,NULL}};
room rooms[3];
int n_rooms;

int countOccupiedDevice(request *success[],int n_success, char devicename[], time_t start, time_t end){
    int count=0;
    for (int i = 0; i < n_success; i++)
    {
        if((strcmp(success[i]->device[0],devicename)==0||strcmp(success[i]->device[1],devicename)==0)&&
        ((start>success[i]->start && start<success[i]->end)||(end>success[i]->start && end<success[i]->end))) count++;
    }
    return count;
}



int roomCmp(const void *a,const void *b){
    return (*(room**)a - *(room**)b);
}

int allocateRoom(request *rqs, request *success[]){

    room *sortedRooms[n_rooms];

    for (int i = 0; i < n_rooms; i++)
    {
        sortedRooms[i]=&rooms[i];
    }

    qsort(sortedRooms, n_rooms, sizeof(room)*n_rooms, roomCmp);

    for (int i = 0; i < n_rooms; i++)
    {
        if (sortedRooms[i]->capacity<rqs->people) continue;

        if ((rqs->start>success[i]->start && rqs->start<success[i]->end)||(rqs->end>success[i]->start && rqs->end<success[i]->end)) continue;

        rqs->roomno= (sortedRooms[i] - &rooms[0])/sizeof(room);
        return 1;

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
            if (countOccupiedDevice(success,n_success,rqs[i]->device[0],rqs[i]->start,rqs[i]->end)>=devices[search(rqs[i]->device[0])].quantity||
            countOccupiedDevice(success,n_success,rqs[i]->device[1],rqs[i]->start,rqs[i]->end)>=devices[search(rqs[i]->device[1])].quantity)
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

int main(){

    request* success[10];
    request* fail[10];

    request rqs[3]={{0,"t1",0,60,1,15,1,{"device1","device2"},1},{0,"t1",30,90,1,15,1,{"device1","device2"},1},{0,"t1",60,120,1,15,1,{"device1","device2"},1}};
    request *rqsp[3];
    for (int i = 0; i < 3; i++)
    {
        rqsp[i] = &rqs[i];
    }
    

    fcfs_schedule(rqsp,success,fail);

}
