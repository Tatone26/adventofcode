#include "runner.h"

/// @brief Returns true if the given array has a way to give the total
/// Goes backwards, because it is a LOT faster !
bool isArrayGood(luint total, luint start, int index, luint *array)
{
    if (index == 0)
        return start == 0;
    if (start % array[index] == 0 && isArrayGood(total, start / array[index], index - 1, array))
        return true;
    if (isArrayGood(total, start - array[index], index - 1, array))
        return true;
    return false;
}

luint part1(void *input_v, void **args)
{
    luint **input = (luint **)input_v;
    int size = ((int *)args)[0];

    luint res = 0;

    for (int i = 0; i < size; i++)
    {
        int start_index = 1;
        while (input[i][start_index] != 0)
            start_index++;
        start_index--;

        if (isArrayGood(input[i][0], input[i][0], start_index, input[i]))
            res += input[i][0];
    }
    return res;
}

// -----------------------------------------------------------------

luint pow_ten(int n)
{
    luint temp = 1;
    for (int i = 0; i < n; i++)
        temp *= 10;
    return temp;
}

/// @brief Returns true if the given array has a way to give the total
/// Goes backwards, because that's a lot faster ! -> went from 40ms forward to ~1ms backwards !
bool isArrayGoodBis(luint total, luint start, int index, luint *array)
{
    if (index == 0)
        return start == 0;
    // if can divide
    if (start % array[index] == 0 && isArrayGoodBis(total, start / array[index], index - 1, array))
        return true;
    // if can un-concatenate
    luint temp = pow_ten(nbOfDigits(array[index]));
    if ((start - array[index]) % temp == 0 && isArrayGoodBis(total, (start - array[index]) / temp, index - 1, array))
        return true;
    // if can substract (you can always substract, worst case scenario is overflowing)
    if (isArrayGoodBis(total, start - array[index], index - 1, array))
        return true;

    return false;
}

luint part2(void *input_v, void **args)
{
    luint **input = (luint **)input_v;
    int size = ((int *)args)[0];

    luint res = 0;

    for (int i = 0; i < size; i++)
    {
        int start_index = 1;
        while (input[i][start_index] != 0)
            start_index++;
        start_index--;

        if (isArrayGoodBis(input[i][0], input[i][0], start_index, input[i]))
            res += input[i][0];
    }
    return res;
}

// ----------------------------------------------------------------

/// @brief It works please don't judge me
luint **readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;
    *size = fileSize(f);

    luint **input = (luint **)(malloc(sizeof(luint *) * *size));
    for (int i = 0; i < *size; i++)
    {
        luint first;

        if (!fgets(buffer, MAX_LINE_LEN, f) || sscanf(buffer, "%llu:", &first) != 1)
        {
            printf("ERRROR INPUT READING\n");
            for (int j = 0; j < i; j++)
                free(input[j]);
            free(input);
            return 0;
        }
        int size = 1;         // the nb before ':'
        int start_offset = 2; // ': '
        start_offset += nbOfDigits(first);
        int offset = start_offset;
        luint a;
        while (sscanf(buffer + offset, "%llu", &a) == 1)
        {
            size++;
            offset += nbOfDigits(a) + 1;
        }
        // printf("%s : size %d\n", buffer, size);
        input[i] = (luint *)malloc(sizeof(luint) * (size + 1));
        input[i][0] = first;
        offset = start_offset;
        int j = 1;
        while (sscanf(buffer + offset, "%llu", &a) == 1)
        {
            input[i][j] = a;
            j++;
            offset += nbOfDigits(a) + 1;
        }
        input[i][size] = 0;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    luint **input = readInput(argv[1], &size);
    run(7, part1, part2, input, (void **)&size);

    for (int i = 0; i < size; i++)
        free(input[i]);
    free(input);
    return 0;
}