#include "utils.h"

void resetBuffer(char *buffer, int size)
{
    for (int i = 0; i < size; i++)
    {
        buffer[i] = '\0';
    }
}
