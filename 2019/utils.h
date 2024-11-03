#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

typedef enum
{
    Up,
    Down,
    Left,
    Right,
    None
} Direction;
void debugDirection(Direction d);

/* Resets a buffer to only '\0' */
void resetBuffer(char *buffer, int size);
/* Returns the size of a list separated by a char. Returns at least 1. (1 + numberOfSeparators)*/
int sizeOfList(char *buffer, char separator);

#endif