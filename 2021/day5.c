#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define MIN(x, y) (x > y ? y : x)
#define MAX(x, y) (x > y ? x : y)

// A way to speed up : there is no lines that are neither straight or diagonals in the files.

int main()
{

    FILE *f = fopen("inputs/day5.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    int maxX = 0, maxY = 0;
    int x1, y1, x2, y2;
    while (!feof(f))
    {
        if (fscanf(f, "%d,%d -> %d,%d\n", &x1, &y1, &x2, &y2) == 4)
        {
            maxX = x1 > maxX ? x1 : maxX;
            maxY = y1 > maxY ? y1 : maxY;
            maxX = x2 > maxX ? x2 : maxX;
            maxY = y2 > maxY ? y2 : maxY;
        }
    }

    maxX++;
    maxY++;

    int **world = malloc(sizeof(int *) * (maxX));
    if (!world)
    {
        printf("problème malloc\n");
    }
    for (int i = 0; i < maxX; i++)
    {
        world[i] = malloc(sizeof(int) * (maxY));
        for (int j = 0; j < maxY; j++)
        {
            world[i][j] = 0;
        }
    }

    int newX, newY;
    fsetpos(f, &start);
    while (!feof(f))
    {
        if (fscanf(f, "%d,%d -> %d,%d\n", &x1, &y1, &x2, &y2) == 4)
        {
            if (x1 == x2 || y1 == y2)
            {
                for (int i = 0; i <= MAX(abs(x1 - x2), abs(y1 - y2)); i++)
                {
                    if (x1 == x2)
                    {
                        newX = x1;
                        newY = y1 > y2 ? y1 - i : y1 + i;
                    }
                    else
                    {
                        newX = x1 > x2 ? x1 - i : x1 + i;
                        newY = y1;
                    }
                    world[newX][newY]++;
                }
            }
        }
    }

    int count = 0;
    for (int i = 0; i < maxX; i++)
    {
        for (int j = 0; j < maxY; j++)
        {
            if (world[i][j] >= 2)
                count++;
        }
    }
    printf("-- Day 5 -- \nOverlapping points : %d\n", count);

    fsetpos(f, &start);
    while (!feof(f))
    {
        if (fscanf(f, "%d,%d -> %d,%d\n", &x1, &y1, &x2, &y2) == 4)
        {
            if (x1 != x2 && y1 != y2)
            {
                for (int i = 0; i <= MAX(abs(x1 - x2), abs(y1 - y2)); i++)
                {
                    newX = x1 > x2 ? x1 - i : x1 + i;
                    newY = y1 > y2 ? y1 - i : y1 + i;
                    world[newX][newY]++;
                }
            }
        }
    }
    count = 0;
    for (int i = 0; i < maxX; i++)
    {
        for (int j = 0; j < maxY; j++)
        {
            if (world[i][j] >= 2)
                count++;
        }
    }
    printf("With diagonal lines : %d\n", count);

    for (int i = 0; i < maxX; i++)
    {
        free(world[i]);
    }
    free(world);
    fclose(f);

    return 0;
}