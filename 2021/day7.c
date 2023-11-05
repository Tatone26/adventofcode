#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// Okay. That was easy. Just gotta realise there is a very simple formula for the sum of all the ints from 1 to n.

int *read_inputs(FILE *f, fpos_t *start, int *size, fpos_t *end, int *max)
{

    *size = 0;
    int c = 0;
    while ((c != '\n') && (c != EOF))
    {
        if (c == EOF)
            break;
        c = fgetc(f);
        if (c == ',')
            (*size)++;
    }
    (*size)++; // Pour le dernier chiffre
    int *result = malloc(sizeof(int) * (*size));
    assert(result);
    for (int i = 0; i < *size; i++)
        result[i] = 0;

    fsetpos(f, start);
    c = 0;
    *max = 0;
    int i = 0;
    while ((c != '\n') && (c != EOF))
    {
        c = fgetc(f);
        if (c == EOF)
            break;
        if ((c != ',') && (c != '\n'))
        {
            result[i] = result[i] * 10 + atoi((char *)&c);
            if (result[i] > *max)
                *max = result[i];
        }
        else
        {
            i++;
        }
    }
    fgetpos(f, end);
    return result;
}

int fuelCost(int *positions, int size, int posToTest, int minToBeat)
{
    int count = 0;
    for (int i = 0; (i < size) && (count <= minToBeat); i++)
    {
        count += abs(positions[i] - posToTest);
    }
    return count;
}

int fuelBetterCost(int *positions, int size, int posToTest, int minToBeat)
{
    int count = 0;
    for (int i = 0; (i < size) && (count <= minToBeat); i++)
    {
        int distance = abs(positions[i] - posToTest);
        count += (distance * (distance + 1)) / 2;
    }
    return count;
}

int main()
{

    FILE *f = fopen("day7.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    int size;
    fpos_t end;
    int max;
    int *positions = read_inputs(f, &start, &size, &end, &max);
    /* for (int i = 0; i < size; i++) {
        printf("%d ", positions[i]);
    }
    printf("\n"); */

    int minFuel = (int)INFINITY;
    int bestPos = 0;
    for (int k = 0; k < max; k++)
    {
        int cost = fuelCost(positions, size, k, minFuel);
        if (cost < minFuel)
        {
            minFuel = cost;
            bestPos = k;
        }
    }

    int minBetterFuel = (int)INFINITY;
    int bestBetterPos = 0;
    for (int k = 0; k < max; k++)
    {
        int cost = fuelBetterCost(positions, size, k, minBetterFuel);
        if (cost < minBetterFuel)
        {
            minBetterFuel = cost;
            bestBetterPos = k;
        }
    }

    printf("-- DAY 7 --\nThey need %d fuel to get to the best pos, %d.\n", minFuel, bestPos);
    printf("They need %d fuel to go to the best position, %d, with a more expensive engine.\n", minBetterFuel, bestBetterPos);

    free(positions);

    fclose(f);

    return 0;
}