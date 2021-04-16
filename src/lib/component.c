#include "component.h"
#include <string.h>
#define d(x) ((PRIME + x - home[x]) % PRIME)
#define n(x) (devices+devices_t[x])

int hash_code(char *s)
{
    int x = 0;
    while (*s)
    {
        x = (x * 127 + *s++) % PRIME;
    }
    return x;
}

int insert(int i)
{
    int *a = devices_t;
    int x = hash_code(devices[i].name);
    if (a[x] < 0)
    {
        a[x] = i;
        home[x]=i;
        return x;
    }
    int m = x;
    int t = (m + 1) % PRIME;
    for (; t != m; t = (t + 1) % PRIME)
    {
        if (a[t] < 0)
        {
            a[t] = i;
            home[t]=x;
            return t;
        }
        if (d(t) < ((PRIME + t - x) % PRIME))
        {
            int tmp = a[t];
            a[t] = i;
            i=tmp;
            tmp=home[t];
            home[t]=x;
            x=tmp;
        }
    }
    return -1;
}

int search(char *s)
{
    int i = hash_code(s);
    int m = i;
    int *a = devices_t;
    for (; a[i] > 0 && d(i) > ((PRIME + i - m) % PRIME); i = (i + 1) % PRIME)
    {
        if (!strcmp(s, n(i)->name))
            return i;
    }
    return -1;
}