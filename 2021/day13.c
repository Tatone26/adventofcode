#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define MAX_LEN 256

// Nice one. Don't know why I spent so much time on reading the input. Ouch.

int **readInput(FILE *f, fpos_t *start, int *sizeX, int *sizeY, fpos_t *start_actions)
{
    fsetpos(f, start);
    int maxX = 0, maxY = 0;
    char buffer[MAX_LEN];
    int foundAction = 0;
    while (!feof(f))
    {
        fpos_t temp;
        fgetpos(f, &temp);
        fgets(buffer, MAX_LEN, f);
        int x, y;
        if (sscanf(buffer, "%d,%d", &x, &y) == 2)
        {
            if (x > maxX)
                maxX = x;
            if (y > maxY)
                maxY = y;
        }
        else if (sscanf(buffer, "fold along x=%d", &x) == 1 && !foundAction)
        {
            *start_actions = temp;
            foundAction = 1;
        }
        else if (sscanf(buffer, "fold along y=%d", &y) == 1 && !foundAction)
        {
            *start_actions = temp;
            foundAction = 1;
        }
    }
    maxX++;
    maxY++;
    *sizeX = maxX;
    *sizeY = maxY;
    int **result = malloc(sizeof(int *) * maxY);
    if (result == NULL)
    {
        fprintf(stderr, "Error malloc 1");
    }
    for (int i = 0; i < maxY; i++)
    {
        result[i] = malloc(sizeof(int) * maxX);
        if (result[i] == NULL)
        {
            fprintf(stderr, "Error malloc 2");
        }
        for (int j = 0; j < maxX; j++)
        {
            result[i][j] = 0;
        }
    }
    fsetpos(f, start);
    while (!feof(f))
    {
        fgets(buffer, MAX_LEN, f);
        int x, y;
        if (sscanf(buffer, "%d,%d", &x, &y) == 2)
        {
            result[y][x] = 1;
        }
    }
    return result;
}

int **fold(int **input, int xAxis, int *sizeX, int *sizeY)
{
    // if !xAxis -> yAxis

    int **newWorld;
    int temp = *sizeY;
    if (xAxis)
    {
        int newSizeX = (*sizeX - 1) / 2;
        newWorld = malloc(sizeof(int *) * (*sizeY));
        for (int i = 0; i < *sizeY; i++)
        {
            newWorld[i] = malloc(sizeof(int) * newSizeX);
            for (int j = 0; j < newSizeX; j++)
            {
                newWorld[i][j] = input[i][j] || input[i][*sizeX - j - 1];
            }
        }
        *sizeX = newSizeX;
    }
    else
    {
        int newSizeY = (*sizeY - 1) / 2;
        newWorld = malloc(sizeof(int *) * (newSizeY));
        for (int i = 0; i < newSizeY; i++)
        {
            newWorld[i] = malloc(sizeof(int) * (*sizeX));
            for (int j = 0; j < *sizeX; j++)
            {
                newWorld[i][j] = input[i][j] || input[*sizeY - i - 1][j];
            }
        }
        *sizeY = newSizeY;
    }
    for (int i = 0; i < temp; i++)
    {
        free(input[i]);
    }
    free(input);
    return newWorld;
}

int main()
{

    FILE *f = fopen("inputs/day13.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    int sizeX, sizeY;
    fpos_t startActions;
    int **input = readInput(f, &start, &sizeX, &sizeY, &startActions);
    // input[sizeY][sizeX]
    /* for (int i = 0; i < sizeY; i++)
    {
        for (int j = 0; j < sizeX; j++)
        {
            printf("%c", input[i][j] ? '#' : '.');
        }
        printf("\n");
    } */

    char buffer[MAX_LEN];
    fsetpos(f, &startActions);
    int i = 0;
    while (!feof(f))
    {
        i++;
        int x, h;
        fgets(buffer, MAX_LEN, f);
        if (feof(f))
            break;
        if (sscanf(buffer, "fold along x=%d", &h) == 1)
            x = 1;
        else if (sscanf(buffer, "fold along y=%d", &h) == 1)
            x = 0;
        else
            break;
        input = fold(input, x, &sizeX, &sizeY);

        if (i == 1)
        {
            int compte = 0;
            for (int i = 0; i < sizeY; i++)
            {
                for (int j = 0; j < sizeX; j++)
                {
                    if (input[i][j])
                        compte++;
                }
            }
            printf("-- Day 13 --\nAfter one instruction, there are %d visible points.\n", compte);
        }
    }
    printf("After following the instructions, here is the code : \n");
    for (int i = 0; i < sizeY; i++)
    {
        for (int j = 0; j < sizeX; j++)
        {
            printf("%c", input[i][j] ? '#' : ' ');
        }
        printf("\n");
        free(input[i]);
    }
    free(input);

    fclose(f);
    return 0;
}