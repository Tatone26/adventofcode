#include "runner.h"

luint part1(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    for (int i = 0; i < size; i++)
        printf("%d\n", input[i]);

    return 0;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    printf("input : _ : size %d\n", size);
    return 0;
}

// ----------------------------------------------------------------

int *readInput(char *filename, int *size)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);

    int *input = toIntList(f, size);

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