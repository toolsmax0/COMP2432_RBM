#include "master.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

room rooms[1000];

equipment eq[1000];

equipment *eq_t[1000];

int main()
{

    FILE *fp = fopen("RBM.ini","r");
    {
        // read in the config
    }

    // points to the pipe
    int readp, writep;
    int pid = getpid(), cid;
    // pipes
    int p[4] = {};
    int flag = 0;

    //return if any one of the pipe creations fails
    flag |= pipe(p) < 0;
    flag |= pipe(p + 2) < 0;
    if (flag)
        return -1;

    
    cid = fork();
    if (cid < 0)
        return -2;
    
    // parent process
    else if (cid > 0)
    {
        // close unneccessary pipes and assign the rest to variables for best readability.
        close(p[0]);
        writep = p[1];
        readp = p[2];
        close(p[3]);
    }

    // child process
    else
    {
        // close unneccessary pipes and assign the rest to variables for best readability.
        readp = p[0];
        close(p[1]);
        close(p[2]);
        writep = p[3];
    }
    return 0;
}