#include "runner.h"

typedef struct
{
    luint a;
    luint b;
} Range;

luint part1(void *input_v, void **args)
{
    luint *input = (luint *)input_v;
    int nb_items = *(int *)((__ptr_t *)args)[0];
    int nb_ranges = *(int *)((__ptr_t *)args)[1];
    Range *ranges = *(Range **)((__ptr_t *)args)[2];

    /* for (int i = 0; i < nb_items; i++)
        printf("%llu\n", input[i]);
    printf("ranges :\n");
    for (int i = 0; i < nb_ranges; i++)
        printf("%llu - %llu\n", ranges[i].a, ranges[i].b); */

    luint count = 0;
    for (int i = 0; i < nb_items; i++)
    {
        for (int x = 0; x < nb_ranges; x++)
        {
            if (ranges[x].a <= input[i] && ranges[x].b >= input[i])
            {
                count++;
                break;
            }
        }
    }
    return count;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    int nb_ranges = *(int *)((__ptr_t *)args)[1];
    Range *ranges = *(Range **)((__ptr_t *)args)[2];

    Range *new_ranges = (Range *)malloc(sizeof(Range) * nb_ranges);
    int nb_new_ranges = nb_ranges;
    memcpy(new_ranges, ranges, sizeof(Range) * nb_ranges);

    int found_anything = 1;
    while (found_anything)
    {
        found_anything = 0;
        Range *temp = (Range *)malloc(sizeof(Range) * nb_new_ranges);
        int nb_temp_ranges = 0;
        for (int x = 0; x < nb_new_ranges; x++)
        {
            int found_overlapping = 0;
            for (int i = 0; i < nb_temp_ranges; i++)
            {
                if (new_ranges[x].a >= temp[i].a && new_ranges[x].a <= temp[i].b && new_ranges[x].b >= temp[i].a && new_ranges[x].b <= temp[i].b)
                {
                    found_overlapping = 1; // new_ranges[x] is totally in temp[i]
                }
                else if (new_ranges[x].a < temp[i].a && new_ranges[x].b > temp[i].b)
                {
                    // temp[i] is totally in new_ranges[x]
                    temp[i].a = new_ranges[x].a;
                    temp[i].b = new_ranges[x].b;
                    found_overlapping = 1;
                }
                else if (new_ranges[x].a >= temp[i].a && new_ranges[x].a <= temp[i].b && new_ranges[x].b > temp[i].b)
                {
                    // x is overlapping up
                    temp[i].b = new_ranges[x].b;
                    found_overlapping = 1;
                }
                else if (new_ranges[x].a < temp[i].a && new_ranges[x].b >= temp[i].a && new_ranges[x].b <= temp[i].b)
                {
                    // x is overlapping down
                    temp[i].a = new_ranges[x].a;
                    found_overlapping = 1;
                }

                if (found_overlapping)
                {
                    found_anything = 1;
                    break;
                }
            }

            if (!found_overlapping)
            {
                temp[nb_temp_ranges].a = new_ranges[x].a;
                temp[nb_temp_ranges++].b = new_ranges[x].b;
            }
        }
        free(new_ranges);
        new_ranges = temp;
        nb_new_ranges = nb_temp_ranges;
    }

    luint count = 0;

    for (int i = 0; i < nb_new_ranges; i++)
        count += (new_ranges[i].b - new_ranges[i].a + 1);

    free(new_ranges);
    return count;
}

// 368596031847584 too high
// 368596031847587
// 359772124067630 too high
// 343089439502475 ?

// ----------------------------------------------------------------

luint *readInput(char *filename, int *nb_items, Range **ranges, int *nb_ranges)
{
    char buffer[MAX_LINE_LEN];
    buffer[0] = '\0';

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    fpos_t start;
    fgetpos(f, &start);

    *nb_ranges = 0;
    fgets(buffer, MAX_LINE_LEN, f);
    while (strlen(buffer) > 1)
    {
        (*nb_ranges)++;
        buffer[0] = '\0';
        fgets(buffer, MAX_LINE_LEN, f);
    }
    *nb_items = 0;
    fgets(buffer, MAX_LINE_LEN, f);
    while (strlen(buffer) > 1)
    {
        (*nb_items)++;
        buffer[0] = '\0';
        fgets(buffer, MAX_LINE_LEN, f);
    }

    if (*nb_ranges <= 0 || *nb_items == 0)
        return NULL;

    fsetpos(f, &start);
    *ranges = (Range *)malloc(sizeof(Range) * (*nb_ranges));
    for (int i = 0; i < *nb_ranges; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "%llu-%llu", &((*ranges)[i].a), &((*ranges)[i].b));
    }
    fgets(buffer, MAX_LINE_LEN, f);
    luint *input = (luint *)malloc(sizeof(luint) * (*nb_items));
    for (int i = 0; i < *nb_items; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "%llu", &input[i]);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int nb_items = 0, nb_ranges = 0;
    Range *ranges;
    luint *input = readInput(argv[1], &nb_items, &ranges, &nb_ranges);
    __ptr_t args[] = {&nb_items, &nb_ranges, &ranges};
    run(5, part1, part2, input, (void **)args);
    free(input);
    free(ranges);
    return 0;
}