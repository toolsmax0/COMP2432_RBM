#include "opti.h"
#include "prio.h"
#include "fcfs.h"

void opti_schedule(request *rqs[],
				   request *success[],
				   request *fail[])
{
	node* init = init_timeline();
    
    size_t n_success = 0;
    size_t n_fail = 0;

    for (size_t i = 0; success[i] != 0; i++)
    {
        n_success++;
    }//count accepted requests
    
    for (size_t i = 0; fail[i] != 0; i++)
    {
        n_fail++;
    }//count rejected requests
    
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

    size_t n_resch_date = 0;

    request *resch_date = (request*)calloc(n_fail, sizeof(request));
    memset(resch_date, 0x00, n_fail * sizeof(request));

    for (size_t i = 0; i < n_fail; i++)
    {
        if (fail[i]->isvalid == 1)
        {
            resch_date[n_resch_date] = *fail[i];
            n_resch_date++;
        }
    }

    node *node_resch_date[n_resch_date];

    for (size_t i = 0; i < n_resch_date; i++)
    {
        node_resch_date[i]->r = &resch_date[i];
    }
    
    node *tmp_init;

    size_t n_resch_time = 0;

    request * resch_time = (request*)calloc(n_resch_date, sizeof(request));
    memset(resch_time, 0x00, n_resch_date);

    for (size_t i = 0; i < n_resch_date; i++)
    {
        tmp_init = search_slot(node_success, resch_date[i].start, resch_date[i].end, 1);
        if (tmp_init != NULL)
        {
            insert_node(node_resch_date[i], tmp_init);
            tmp_init = NULL;
        }
        else
        {
            resch_time[n_resch_time] = resch_date[i];
            n_resch_time++;
        }
    }
    
    //necessary?
    node *node_resch_time[n_resch_time];

    for (size_t i = 0; i < n_resch_time; i++)
    {
        node_resch_time[i]->r = &resch_time[i];
    }
    for (size_t i = 0; i < n_resch_time; i++)
    {
        tmp_init = search_time(node_success, resch_time[i].length, 1);
        if (tmp_init != NULL)
        {
            insert_node(node_resch_time[i], tmp_init);
            tmp_init = NULL;
        }
    }
}
