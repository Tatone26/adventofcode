#include "runner.h"

luint part1(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    int result = 0;

    for (int i = 0; i < size; i++)
        result += (input[i] / 3) - 2;

    return result;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    int result = 0;
    for (int i = 0; i < size; i++)
        while (input[i] / 3 - 2 > 0)
            result += (input[i] = (input[i] / 3) - 2);

    return result;
}

// ----------------------------------------------------------------

int *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    *size = fileSize(f);
    int *input = (int *)malloc(sizeof(int) * *size);

    int offset = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        input[offset] = atoi(buffer);
        offset += 1;
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
    run(1, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}