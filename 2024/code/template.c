#include "runner.h"

luint part1(va_list args)
{
    char *input = va_arg(args, char *);
    int size = va_arg(args, int);

    printf("input : %s ; size %d\n", input, size);
    return 0;
}

// -----------------------------------------------------------------

luint part2(va_list args)
{
    char *input = va_arg(args, char *);
    int size = va_arg(args, int);

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

int main()
{
    int size = 0;
    char *input = readInput("input/foo.txt", &size);
    run(1, part1, part2, 2, input, size);
    free(input);
    return 0;
}