#include "runner.h"

luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    printf("input : %s ; size %d\n", input, size);
    return 0;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    printf("input : %s : size %d\n", input, size);
    return 0;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *size = fileSize(f);
    char *input = (char *)malloc(sizeof(char) * *size);

    int offset = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        strncpy(input + offset, buffer, strlen(buffer) - 1); // -1 to remove \n
        offset += strlen(buffer) - 1;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    char *input = readInput(argv[1], &size);
    run(1, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}