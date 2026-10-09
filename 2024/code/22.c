#include "runner.h"

#define STEPS 2000

// that... doesn't seem to make good assembly ?

static inline int step(int n)
{
    int res = n;
    // modulo 16777216 = 2^24 => keeping low 24 bits. No need for modulo operation.
    res = ((res << 6) & 0xFFFFFF) ^ res;
    res = ((res >> 5) ^ res); // this modulo isn't necessary => division is keeping the number smaller.
    res = ((res << 11) & 0xFFFFFF) ^ res;
    return res;
}

luint part1(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    luint total = 0;

    for (int i = 0; i < size; i++)
    {
        int temp = input[i];
        for (int j = 0; j < STEPS; j++)
            temp = step(temp);
        total += temp;
    }

    return total;
}

// -----------------------------------------------------------------

#define MAX_HASH 608851

static inline uint32_t hash(short *sequence)
{
    int x = sequence[0] + 9;
    x = (x << 5) | (sequence[1] + 9);
    x = (x << 5) | (sequence[2] + 9);
    x = (x << 5) | (sequence[3] + 9);
    return x;
}

static inline uint32_t rollhash(uint32_t hash, short next) // avoiding full recomputation of hash.
{
    return ((hash & 32767) << 5) | (next + 9); // keep three previous values, and apply hash.
}

luint part2(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    if (size <= 1)
        return 0;

    int max_total = 0;
    int *totals = (int *)calloc(MAX_HASH, sizeof(int));
    int *done = (int *)calloc(MAX_HASH, sizeof(int));

    for (int i = 0; i < size; i++)
    {
        int h = 0;
        int temp = input[i];
        int t = temp % 10;
        for (int j = 0; j < STEPS + 1; j++)
        {
            // prices
            int prev = t;

            temp = step(temp);
            t = temp % 10;

            if (j >= 4 && done[h] != i + 1)
            {
                totals[h] += prev;
                if (max_total < totals[h])
                    max_total = totals[h];
                done[h] = i + 1;
            }

            h = rollhash(h, t - prev);
        }
    }

    free(totals);
    free(done);
    return (luint)max_total;
}
// ----------------------------------------------------------------

int *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;
    *size = fileSize(f);
    int *input = (int *)malloc(sizeof(int) * *size);

    for (int i = 0; i < *size; i++)
    {
        if (!fgets(buffer, MAX_LINE_LEN, f))
        {
            fclose(f);
            free(input);
            return NULL;
        }
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