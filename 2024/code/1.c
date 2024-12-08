#include "runner.h"

// --------------------------------------------------------------------------------------------

int comp(const void *elem1, const void *elem2)
{
    int f = *((int *)elem1);
    int s = *((int *)elem2);
    return f - s;
}

luint part1(va_list args)
{
    int size = va_arg(args, int);

    int *list1 = va_arg(args, int *);
    int *list2 = va_arg(args, int *);

    // sorting COPIES of the arrays (not modifying input)
    int *new_list1 = (int *)malloc(sizeof(int) * size);
    int *new_list2 = (int *)malloc(sizeof(int) * size);
    memcpy(new_list1, list1, size * sizeof(int));
    memcpy(new_list2, list2, size * sizeof(int));

    // That's the slow part
    qsort(new_list1, size, sizeof(int), comp);
    qsort(new_list2, size, sizeof(int), comp);

    luint diff_sum = 0;
    for (int i = 0; i < size; i++)
        diff_sum += abs(new_list1[i] - new_list2[i]);

    free(new_list1);
    free(new_list2);
    return diff_sum;
}

// --------------------------------------------------------------------------------------------------------

/// @brief Makes my own kind of hashmap : just an array of int, size being the max value contained in the original array (+1)
/// @param size The size of the original list
/// @param list The list
/// @param new_size The size of the new list == the max of the original list, will be modified
/// @return The "hashmap"
int *count_numbers(int size, int *list, int *new_size)
{
    if (size <= 0) // not useful, just security
        return 0;

    *new_size = list[0];
    for (int i = 0; i < size; i++)
    {
        if (list[i] > *new_size)
            *new_size = list[i];
    }
    int *result = (int *)calloc(*new_size + 1, sizeof(int)); // +1 necessary because it is indices stored in the array.

    for (int i = 0; i < size; i++)
        result[list[i]]++;

    return result;
}

luint part2(va_list args)
{
    int size = va_arg(args, int);
    int *list1 = va_arg(args, int *);
    int *list2 = va_arg(args, int *);

    int new_size2 = 0;
    int *res2 = count_numbers(size, list2, &new_size2);

    luint similarity_score = 0;
    for (int i = 0; i < size; i++)
    {
        if (list1[i] > new_size2)
            continue;
        similarity_score += list1[i] * res2[list1[i]];
    }
    free(res2);
    return similarity_score;
}

// --------------------------------------------------------------------------------------------

void readInput(char *filename, int *size, int **list1, int **list2)
{
    char buffer[MAX_LINE_LEN];
    buffer[0] = 'a';

    FILE *f = fopen(filename, "r");
    *size = fileSize(f);

    if (*size == 0)
        return;

    *list1 = (int *)malloc(sizeof(int) * *size);
    *list2 = (int *)malloc(sizeof(int) * *size);

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
    return 0;
}