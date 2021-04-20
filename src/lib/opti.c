#include "opti.h"
#include "prio.h"
#include "fcfs.h"

void opti_schedule(request *rqs[],
				   request *success[],
				   request *fail[])
{
	size_t n_success = 0;
	size_t n_fail = 0;
    for (size_t j = 0; j < 4; j++)
    {
        if (rqs[j]->priority == j)
        {
            for (size_t i = 0; rqs[i] != 0; i++)
            {
                if (rqs[i]->isvalid == 0)
				{
                    fail[n_fail] = rqs[i];
                    n_fail++;
                    continue;
                }

                if (allocateRoom(rqs[i], success)==0)
                {
                    fail[n_fail] = rqs[i];
                    n_fail++;
                    continue;
                }

                if (rqs[i]->device[0][0] == 0)
                {
                    success[n_success] = rqs[i];
                    n_success++;
                }
                else
				{
                    if (countOccupiedDevice(success,
											n_success,
											rqs[i]->device[0],
											rqs[i]->start,
											rqs[i]->end)	>=
							devices[search(rqs[i]->device[0])].quantity		||
                    	countOccupiedDevice(success,
											n_success,
											rqs[i]->device[1],
											rqs[i]->start,
											rqs[i]->end)	>=
							devices[search(rqs[i]->device[1])].quantity)
                    {
                        success[n_success] = rqs[i];
                        n_success++;
                    }
                    else
					{
                        fail[n_fail] = rqs[i];
                        n_fail++;
                    } 
                }
            }
        }
    }
	node* init = init_timeline();

	node node_success[n_success];
	
	for (size_t i = 0; i < n_success; i++)
	{
		node_success[i].r = success[i];
	}

	insert_node(&node_success[1], init);

	for (size_t i = 0; i < n_success - 1; i++)
	{
		insert_node(&node_success[i + 1], &node_success[i]);
	}
	
}
