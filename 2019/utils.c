#include "utils.h"

void resetBuffer(char *buffer, int size)
{
    for (int i = 0; i < size; i++)
    {
        buffer[i] = '\0';
    }
}

int sizeOfList(char *buffer, char separator)
{
    int size = 1;
    int pos = 0;
    while (buffer[pos] != '\0')
    {
        if (buffer[pos] == separator)
            size++;
        pos++;
    }
    return size;
}

void debugDirection(Direction d)
{
    switch (d)
    {
    case Left:
        printf("L");
        break;
    case Right:
        printf("R");
        break;
    case Up:
        printf("U");
        break;
    case Down:
        printf("D");
        break;
    case None:
        printf("None");
        break;
    }
}