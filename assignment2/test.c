#include <stdio.h>
#include <string.h>

int main()
{
    char *x = "abcd";

    if(strstr("abcd", x) != NULL)
    {
        printf("PASS!\n");
    }
    
    
    return 0;
}


