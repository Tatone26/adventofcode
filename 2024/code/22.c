#include "runner.h"

#define STEPS 2000

// that... doesn't seem to make good assembly ?

static inline long step(long n)
{
    long res = n;
    res = ((res * 64) ^ res) % 16777216;
    res = ((res / 32) ^ res) % 16777216;
    res = ((res * 2048) ^ res) % 16777216;
    return res;
}

luint part1(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    luint total = 0;

    for (int i = 0; i < size; i++)
    {
        long temp = input[i];
        for (int j = 0; j < STEPS; j++)
            temp = step(temp);
        total += temp;
    }

    return total;
}

// -----------------------------------------------------------------

// most time spent on preparation (reading the input :( )

#define MAX_HASH 608851

int hash(short *sequence)
{
    int x = sequence[0] + 9;
    x = (x << 5) | (sequence[1] + 9);
    x = (x << 5) | (sequence[2] + 9);
    x = (x << 5) | (sequence[3] + 9);
    return x;
}

// all preparation steps are SLOW, the rest is instant

luint part2(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    if (size <= 1)
        return 0;

    // preparation

    short **buyers = (short **)malloc(sizeof(short *) * size);
    short **changes = (short **)malloc(sizeof(short *) * size);

    for (int i = 0; i < size; i++)
    {
        buyers[i] = (short *)malloc(sizeof(short) * (STEPS + 1));
        changes[i] = (short *)malloc(sizeof(short) * (STEPS + 1));
        {
            long temp = input[i];
            int t = temp % 10;
            for (int j = 0; j < STEPS + 1; j++)
            {
                // prices
                buyers[i][j] = t;
                temp = step(temp);
                t = temp % 10;
                // changes
                changes[i][j] = t - buyers[i][j];
            }
        }
    }

    int *totals = (int *)calloc(sizeof(int), MAX_HASH);
    int done[MAX_HASH] = {0};
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < STEPS - 3; j++)
        {
            int h = hash(changes[i] + j);
            if (done[h] != i + 1)
            {
                totals[h] += buyers[i][j + 4];
                done[h] = i + 1;
            }
        }
    }

    // calculation

    int res = 0;
    bool *cache = (bool *)calloc(sizeof(bool), MAX_HASH);
    for (int x = 0; x < size; x++)
    {
        for (int d = 0; d < STEPS - 3; d++)
        {
            int test = 0;
            int h = hash(changes[x] + d);
            if (cache[h])
                continue;

            test = totals[h];

            if (test > res)
                res = test;
            cache[h] = true;
        }
    }

    // end

    free(cache);
    for (int i = 0; i < size; i++)
        free(changes[i]);
    free(changes);

    for (int i = 0; i < size; i++)
        free(buyers[i]);
    free(buyers);

    free(totals);
    return (luint)res;
}

// ----------------------------------------------------------------

int *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *size = fileSize(f);
    int *input = (int *)malloc(sizeof(int) * *size);

    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        input[i] = atoi(buffer);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    int *input = readInput(argv[1], &size);
    run(22, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}