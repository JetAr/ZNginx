#include <string.h>
#include "text.h"

bool startWith(char *a, char *b)
{
    int i = 0;
    while(a[i] != 0)
    {
        if (a[i] != b[i]) return false;
        i++;
    }
    return true;
}

char *newString(char *arg)
{
    int len = strlen(arg);
    char *res = new char[len + 1];
    strcpy(res, arg);
    return res;
}