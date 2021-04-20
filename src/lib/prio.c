#include "prio.h"
#include "fcfs.h"

void prio_schedule(request *rqs[], request *success[], request *fail[]) {
    int n_success=0;
    int n_fail=0;
    for (int j = 0; j < 4; j++)
    {
        if (rqs[j]->priority==j)
        {
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
        
    }
    

    
}