#include "opti.h"
#include "prio.h"
#include "fcfs.h"

void opti_schedule(request *rqs[],
				   request *success[],
				   request *fail[])
{
    size_t n_success = 0;
    size_t n_fail = 0;
    size_t n_device = 0;

    //-1 for not found, otherwise for index
    int room_find = -1;
    int dev1_find = -1;
    //-2 for no dev2
    int dev2_find = -1;

    for (size_t i = 0; success[i] != 0; i++)
    {
        n_success++;
    }//count accepted requests
    
    for (size_t i = 0; fail[i] != 0; i++)
    {
        n_fail++;
    }//count rejected requests
    
    for (size_t i = 0; devices[i].name[0] != 0; i++)
    {
        n_device++;
    }
    //delete resch_room related if best-fit not available
    node *node_room_success = (node*)calloc(n_success, sizeof(node));
    node *node_dev1_success = (node*)calloc(n_success, sizeof(node));
    node *node_dev2_success = (node*)calloc(n_success, sizeof(node));
    memset(node_room_success, 0x00, sizeof(node) * n_success);
    memset(node_dev1_success, 0x00, sizeof(node) * n_success);
    memset(node_dev2_success, 0x00, sizeof(node) * n_success);//zero for only one device
    
	for (size_t i = 0; i < n_success; i++)
	{
		node_room_success[i].r = success[i];
        node_dev1_success[i].r = success[i];
        node_dev2_success[i].r = success[i];
	}

    for (size_t i = 0; success[i] != 0; i++)
    {
        insert_node(&node_room_success[i], rooms[node_room_success[i].r->roomno].timeline->prev);

        for (size_t j = 0; j < devices[search(node_dev1_success[i].r->device[0])].quantity; j++)
        {
            if (search_slot(devices[search(node_dev1_success[i].r->device[0])].timelines[j],
                            node_dev1_success[i].r->start,
                            node_dev1_success[i].r->end,
                            -1)     == NULL)
            {
                insert_node(&node_dev1_success[i], devices[search(success[i]->device[0])].timelines[j]->prev);
                break;
            }
        }
        if (node_dev2_success[i].r->device[2][0] != 0)
        {
            for (size_t j = 0; j < devices[search(node_dev1_success[i].r->device[1])].quantity; j++)
            {
                if (search_slot(devices[search(success[i]->device[1])].timelines[j],
                                node_dev1_success[i].r->start,
                                node_dev1_success[i].r->end,
                                -1)     == NULL)
                {
                    insert_node(&node_dev1_success[i], devices[search(success[i]->device[1])].timelines[j]->prev);
                    break;
                }
            }
        }
    }
	
    size_t n_resch_room = 0;

    request *resch_room = (request*)calloc(n_fail, sizeof(request));
    memset(resch_room, 0x00, n_fail * sizeof(request));

    for (size_t i = 0; i < n_fail; i++)
    {
        resch_room[n_resch_room] = *fail[i];
        n_resch_room++;
    }

    node *node_room_resch_room = (node*)calloc(n_resch_room, sizeof(node));
    node *node_dev1_resch_room = (node*)calloc(n_resch_room, sizeof(node));
    node *node_dev2_resch_room = (node*)calloc(n_resch_room, sizeof(node));
    memset(node_room_resch_room, 0x00, sizeof(node) * n_resch_room);
    memset(node_dev1_resch_room, 0x00, sizeof(node) * n_resch_room);
    memset(node_dev2_resch_room, 0x00, sizeof(node) * n_resch_room);

    for (size_t i = 0; i < n_resch_room; i++)
    {
        node_room_resch_room[i].r = &resch_room[i];
        node_dev1_resch_room[i].r = &resch_room[i];
        node_dev2_resch_room[i].r = &resch_room[i];
    }
    
    size_t n_resch_time = 0;

    request *resch_time = (request*)calloc(n_resch_room, sizeof(request));
    memset(resch_time, 0x00, n_resch_room * sizeof(request));

    for (size_t i = 0; i < n_resch_room; i++)
    {
        for (size_t j = 0; rooms[j].capacity != 0; j++)
        {
            if (rooms[j].capacity >= resch_room[i].people   &&
                search_slot(rooms[j].timeline,
                            node_room_resch_room[i].r->start,
                            node_room_resch_room[i].r->end,
                            -1) == NULL)
            {
                room_find = j;
                break;
            }
        }
        for (size_t j = 0; j < devices[search(node_dev1_resch_room[i].r->device[0])].quantity; j++)
        {
            if (search_slot(devices[search(node_dev1_resch_room[i].r->device[0])].timelines[j],
                            node_dev1_resch_room[i].r->start,
                            node_dev1_resch_room[i].r->end,
                            -1)     == NULL)
            {
                dev1_find = j;
                break;
            }
        }
        if (node_dev2_resch_room[i].r->device[2][0] != 0)
        {
            for (size_t j = 0; j < devices[search(node_dev2_resch_room[i].r->device[1])].quantity; j++)
            {
                if (search_slot(devices[search(node_dev2_resch_room[i].r->device[1])].timelines[j],
                                node_dev2_resch_room[i].r->start,
                                node_dev2_resch_room[i].r->end,
                                -1)     == NULL)
                {
                    dev2_find = j;
                    break;
                }
            }
        }
        else
        {
            dev2_find = -2;
        }
        
        if (room_find != -1 && dev1_find != -1 && dev2_find >= 0)
        {
            insert_node(&node_room_resch_room[i], rooms[room_find].timeline->prev);
            insert_node(&node_dev1_resch_room[i], devices[search(node_dev1_resch_room[i].r->device[0])].timelines[dev1_find]->prev);
            insert_node(&node_dev2_resch_room[i], devices[search(node_dev2_resch_room[i].r->device[1])].timelines[dev2_find]->prev);
        }
        else if (room_find != -1 && dev1_find != -1 && dev2_find == -2)
        {
            insert_node(&node_room_resch_room[i], rooms[room_find].timeline->prev);
            insert_node(&node_dev1_resch_room[i], devices[search(node_dev1_resch_room[i].r->device[0])].timelines[dev1_find]->prev);
        }
        else
        {
            resch_time[n_resch_time] = resch_room[i];
            n_resch_time++;
        }
        int room_find = -1;
        int dev1_find = -1;
        int dev2_find = -1;
    }
    
    node *node_room_resch_time = (node*)calloc(n_resch_time, sizeof(node));
    node *node_dev1_resch_time = (node*)calloc(n_resch_time, sizeof(node));
    node *node_dev2_resch_time = (node*)calloc(n_resch_time, sizeof(node));
    memset(node_room_resch_time, 0x00, sizeof(node) * n_resch_time);
    memset(node_dev1_resch_time, 0x00, sizeof(node) * n_resch_time);
    memset(node_dev2_resch_time, 0x00, sizeof(node) * n_resch_time);

    for (size_t i = 0; i < n_resch_time; i++)
    {
        node_room_resch_time[i].r = &resch_time[i];
        node_dev1_resch_time[i].r = &resch_time[i];
        node_dev2_resch_time[i].r = &resch_time[i];
    }
    
    size_t n_real_fail = 0;

    request *real_fail = (request*)calloc(n_resch_time, sizeof(request));
    memset(real_fail, 0x00, n_resch_time * sizeof(request));

    for (size_t i = 0; i < n_resch_time; i++)
    {
        for (time_t j = genesis; j < eternity; )
        {
            for (size_t k = 0; rooms[k].capacity != 0; k++)
            {
                if (rooms[j].capacity >= resch_time[i].people   &&
                    search_slot(rooms[j].timeline,
                                j,
                                j + node_room_resch_time[i].r->length,
                                -1) == NULL)
                {
                    room_find = k;
                    break;
                }
            }
            for (size_t k = 0; j < devices[search(node_dev1_resch_time[i].r->device[0])].quantity; k++)
            {
                if (search_slot(devices[search(node_dev1_resch_time[i].r->device[0])].timelines[j],
                                j,
                                j + node_room_resch_time[i].r->length,
                                -1) == NULL)
                {
                    dev1_find = k;
                    break;
                }
            }
            if (node_dev2_resch_time[i].r->device[2][0] != 0)
            {
                for (size_t k = 0; k < devices[search(node_dev2_resch_time[i].r->device[1])].quantity; k++)
                {
                    if (search_slot(devices[search(node_dev2_resch_time[i].r->device[1])].timelines[k],
                                    j,
                                    j + node_room_resch_time[i].r->length,
                                    -1) == NULL)
                    {
                        dev2_find = k;
                        break;
                    }
                }
            }
            else
            {
                dev2_find = -2;
            }
            if (node_dev2_resch_time[i].r->device[2][0] != 0 && room_find != -1 && dev1_find != -1 && dev2_find >= 0)
            {
                break;
            }
            else if(node_dev2_resch_time[i].r->device[2][0] == 0 && room_find != -1 && dev1_find != -1 && dev2_find == -2)
            {
                break;
            }
            j += 3600;
        }
        if (room_find != -1 && dev1_find != -1 && dev2_find >= 0)
        {
            insert_node(&node_room_resch_time[i], rooms[room_find].timeline->prev);
            insert_node(&node_dev1_resch_time[i], devices[search(node_dev1_resch_time[i].r->device[0])].timelines[dev1_find]->prev);
            insert_node(&node_dev2_resch_time[i], devices[search(node_dev2_resch_time[i].r->device[1])].timelines[dev2_find]->prev);
        }
        else if (room_find != -1 && dev1_find != -1 && dev2_find == -2)
        {
            insert_node(&node_room_resch_time[i], rooms[room_find].timeline);
            insert_node(&node_dev1_resch_time[i], devices[search(node_dev1_resch_time[i].r->device[0])].timelines[dev1_find]->prev);
        }
        else
        {
            real_fail[n_real_fail] = resch_time[i];
            n_resch_time++;
        }
        int room_find = -1;
        int dev1_find = -1;
        int dev2_find = -1;
    }
}
