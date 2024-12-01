#include "utils.h"

// --------------------------------------------------------------------------------------------

int comp(const void *elem1, const void *elem2)
{
    int f = *((int *)elem1);
    int s = *((int *)elem2);
    return f - s;
}

int part1(int count, va_list args)
{
    if (count != 3)
        printf("ERROR WITH ARGUMENTS\n");
    int size = va_arg(args, int);

    int *list1 = va_arg(args, int *);
    int *list2 = va_arg(args, int *);

    // sorting the arrays NOT in place (because I don't want to modify the input)
    int *new_list1 = (int *)malloc(sizeof(int) * size);
    int *new_list2 = (int *)malloc(sizeof(int) * size);
    memcpy(new_list1, list1, size * sizeof(int));
    memcpy(new_list2, list2, size * sizeof(int));

    qsort(new_list1, size, sizeof(int), comp);
    qsort(new_list2, size, sizeof(int), comp);

    int diff_sum = 0;
    for (int i = 0; i < size; i++)
        diff_sum += abs(new_list1[i] - new_list2[i]);

    free(new_list1);
    free(new_list2);
    return diff_sum;
}

// --------------------------------------------------------------------------------------------------------

int *count_numbers(int size, int *list, int *max)
{
    if (size <= 0) // not useful, just security
        return 0;

    *max = list[0];
    for (int i = 0; i < size; i++)
    {
        if (list[i] > *max)
            *max = list[i];
    }
    int *result = (int *)calloc(*max + 1, sizeof(int)); // +1 necessary because it is indices stored in the array.

    for (int i = 0; i < size; i++)
    {
        result[list[i]]++;
    }
    return result;
}

// Something like dictionaries could allow making an even faster version
int part2(int count, va_list args)
{
    if (count != 3)
        printf("ERROR WITH ARGUMENTS\n");
    int size = va_arg(args, int);
    int *list1 = va_arg(args, int *);
    int *list2 = va_arg(args, int *);

    int new_size1 = 0;
    int *res1 = count_numbers(size, list1, &new_size1);
    int new_size2 = 0;
    int *res2 = count_numbers(size, list2, &new_size2);

    int similarity_score = 0;
    for (int i = 0; i < new_size1; i++)
    {
        if (res1[i] == 0 || res1[i] > new_size2)
            continue;
        similarity_score += i * res1[i] * res2[i];
    }
    free(res1);
    free(res2);
    return similarity_score;
}

// --------------------------------------------------------------------------------------------

void readInput(char *filename, int *size, int **list1, int **list2)
{
    char buffer[MAX_LINE_LEN];
    buffer[0] = 'a';

    FILE *f = fopen(filename, "r");
    fpos_t start;
    fgetpos(f, &start);

    *size = 0;
    while (!feof(f) && buffer[0] != '\n')
    {
        (*size)++;
        fgets(buffer, MAX_LINE_LEN, f);
    }
    if (*size == 0)
        return;

    *list1 = (int *)malloc(sizeof(int) * *size);
    *list2 = (int *)malloc(sizeof(int) * *size);

    fsetpos(f, &start);
    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "%d   %d\n", &(*list1)[i], &(*list2)[i]);
    }

    fclose(f);
}

int main()
{
    int size = 0;
    int *list1, *list2;
    readInput("input/1.txt", &size, &list1, &list2);
    if (size == 0)

        return 1;

    run(1, part1, part2, 3, size, list1, list2);
    free(list1);
    free(list2);
}