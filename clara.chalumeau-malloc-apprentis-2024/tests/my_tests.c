#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    char *a = malloc(100);
    // char *b;
    for (size_t i = 0; i < 100000; i++)
    {
        char *b = calloc(1202, 1);
    }

    return 0;
}
