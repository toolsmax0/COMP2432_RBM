#include "fcfs.h"
#include "timeline.h"
#include "request.h"
#include "component.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

<<<<<<< Updated upstream
void fcfs_schedule(request *rqs[], request *success[], request *fail[]){}
=======
device devices[3]={{"device1",2,NULL},{"device2",2,NULL},{"device3",2,NULL}};

int countOccupiedDevice(request *success[],int n_success, char devicename[], time_t start){
    int count=0;
    for (int i = 0; i < n_success; i++)
    {
        if((strcmp(success[i]->device[0],devicename)==0||strcmp(success[i]->device[1],devicename)==0)) count++;
    }
    return count;
}

int isOccupied(request rqs,request *success[],int n_success){

    int n_device0=devices[search(rqs.device[0])].quantity;
    int n_devide1=devices[search(rqs.device[1])].quantity;

    for (int i = 0; i < n_success; i++)
    {
        if (rqs.roomno==success[i]->roomno && rqs.start<success[i]->start && rqs.end>success[i]->end) return 1;
    }
    if (countOccupiedDevice(success, n_success, rqs.device[0], rqs.start)>=n_device0) return 1;
    if (countOccupiedDevice(success, n_success, rqs.device[1], rqs.start)>=n_device0) return 1;

    return 0;
}



void fcfs_schedule(request rqs[], request *success[], request *fail[], int n_request){

    int n_success=0;
    int n_fail=0;

    for (int i = 1; i < n_request; i++)
    {
        if (isOccupied(rqs[i],success,n_success))
        {
            success[n_success]=&rqs[i];
            n_success++;
        }
        else{
            fail[n_fail]=&rqs[i];
            n_fail++;
        }
    }
}

int main(){

    request* success[10];
    request* fail[10];

    request rqs[3]={{0,"t1",0,60,1,15,1,{"device1","device2"},1},{0,"t1",30,90,1,15,1,{"device1","device2"},1},{0,"t1",60,120,1,15,1,{"device1","device2"},1}};

    fcfs_schedule(rqs,success,fail,3);

}
>>>>>>> Stashed changes
