#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define MAX_LEN 128
#define DAYS 80
#define MORE_DAYS 256
#define MAX_STATE_START 5

// 0.0001s. Victory.

int *read_inputs(FILE *f, fpos_t *start, int *size, fpos_t *end)
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
    int i = 0;
    while ((c != '\n') && (c != EOF))
    {
        c = fgetc(f);
        if (c == EOF)
            break;
        if ((c != ',') && (c != '\n'))
        {
            result[i] = atoi((char *)&c);
        }
        else
        {
            i++;
        }
    }
    fgetpos(f, end);
    return result;
}

// Need to do some better caching but you know, just don't care
long int number_of_descendants(int state, int days_left, long int cache[MORE_DAYS])
{
    if (days_left < state)
        return 0;
    long int count = 0;
    days_left -= state + 1;
    while (days_left >= 0)
    {
        count += 1 + cache[days_left];
        days_left -= 7;
    }
    return count;
}

int main()
{

    clock_t begin_time = clock();

    FILE *f = fopen("day6.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    int size;
    fpos_t end;
    int *states = read_inputs(f, &start, &size, &end);
    /* for (int i = 0; i < size; i++) {
        printf("%d ", states[i]);
    }
    printf("\n"); */

    // Here is the magic. Only 256 things to compute, and with a constant time for each because it always uses already computed results...
    long int pre_compute[MORE_DAYS];
    for (int i = 0; i < MORE_DAYS; i++)
    {
        pre_compute[i] = number_of_descendants(8, i, pre_compute);
    }

    int count = size;
    for (int i = 0; i < size; i++)
    {
        count += number_of_descendants(states[i], DAYS, pre_compute);
    }

    printf("-- DAY 6 --\nIn %d days, there will be %d fishes.\n", DAYS, count);

    long int count_more = size;
    long int basic_cache[MAX_STATE_START + 1];
    for (int i = 0; i < MAX_STATE_START + 1; i++)
    {
        basic_cache[i] = 0;
    }
    for (int i = 0; i < size; i++)
    {
        if (basic_cache[states[i]] > 0)
        {
            count_more += basic_cache[states[i]];
            continue;
        }
        basic_cache[states[i]] = number_of_descendants(states[i], MORE_DAYS, pre_compute);
        count_more += basic_cache[states[i]];
    }

    printf("In %d days, there will be %ld fishes.\n\n", MORE_DAYS, count_more);

    free(states);

    fclose(f);
    clock_t end_time = clock();
    double time_spend = (double)(end_time - begin_time) / CLOCKS_PER_SEC;

    printf("It took %.5f seconds (cpu time) to compute.\n\n\n", time_spend);
    return 0;
}