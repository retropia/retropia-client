#include "stringutil.h"
#include <string.h>
#include <ctype.h>

void r_stringutil_rtrim(char *str)
{
    char *p;

    p = str + strlen(str) - 1;

    while (p > str && isspace(*p))
    {
        *(p--) = '\0';
    }
}

void r_stringutil_normalize_case(char *str)
{
    char *p;

    p = str;
    while(*p != '\0')
    {
        if (p == str || *(p - 1) == '-' || *(p - 1) == '_' || isspace(*(p - 1)))
        {
            *p = toupper(*p);
        }
        else
        {
            *p = tolower(*p);
        }

        ++p;
    }
}
