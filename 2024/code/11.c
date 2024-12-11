#include "runner.h"

typedef struct
{
    long value;
    long result;
} Caching;

// Both parts could use different, more optimized sizes of caching, but it would be micro-optimisations.
#define MAX 1024

luint nbStones(long value, int n, Caching cache[][MAX])
{
    if (n == 0) // default case
        return 1;

    Caching *cache_cell = &cache[n][value % MAX];
    if (cache_cell->value == value && cache_cell->result != 0) // cached case
        return cache_cell->result;

    luint t; // all three possible modifications
    if (value == 0)
        t = nbStones(1, n - 1, cache);
    else
    {
        int nbdigits = nbOfDigits(value); // to make sure we don't call this slow function multiple times
        if (nbdigits % 2 == 0)
        {
            // pretty "bad" way of finding the necessary power of ten, but it *should* be enough, right ?
            int pow_ten = 10;
            for (int i = 1; i < nbdigits / 2; i++)
                pow_ten *= 10;
            t = nbStones(value / pow_ten, n - 1, cache) + nbStones(value % pow_ten, n - 1, cache);
        }
        else
            t = nbStones(value * 2024, n - 1, cache);
    }
    // caching result
    cache_cell->result = t;
    cache_cell->value = value;
    return t;
}

luint commonPart(va_list args, int n)
{
    int *input = va_arg(args, int *);
    int size = va_arg(args, int);

    luint res = 0;
    Caching(*cache)[MAX] = (Caching(*)[MAX])calloc(n + 1, sizeof(Caching[MAX]));

    for (int i = 0; i < size; i++)
        res += nbStones(input[i], n, cache);
    free(cache);
    return res;
}

luint part1(va_list args)
{
    return commonPart(args, 25);
}

// -----------------------------------------------------------------

luint part2(va_list args)
{
    return commonPart(args, 75);
}

// ----------------------------------------------------------------

int *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    fgets(buffer, MAX_LINE_LEN, f);
    *size = strlen(buffer);
    int *input = (int *)malloc(sizeof(int) * *size);

    int offset = 0, d = 0;
    int a = 0;
    while (sscanf(buffer + offset, "%d", &a) == 1)
    {
        input[d++] = a;
        offset += 1 + nbOfDigits(a);
    }
    if (d == 0)
    {
        free(input);
        return NULL;
    }
    input = realloc(input, d * sizeof(int));
    *size = d;

    fclose(f);
    return input;
}

int main()
{
    int size = 0;
    int *input = readInput("input/11.txt", &size);
    run(1, part1, part2, 2, input, size);
    free(input);
    return 0;
}