#include "runner.h"

long part1(int count, va_list args)
{
    if (count != 2)
    {
        printf("ERROR WITH ARGUMENTS\n");
        return -1;
    }

    char *input = va_arg(args, char *);
    int size = va_arg(args, int);

    printf("input : %s ; size %d\n", input, size);
    return 0;
}

// -----------------------------------------------------------------

long part2(int count, va_list args)
{
    if (count != 2)
    {
        printf("ERROR WITH ARGUMENTS\n");
        return -1;
    }

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
    fpos_t start;
    fgetpos(f, &start);
    int n = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
        n += strlen(buffer) + 1;
    if (n == 0)
    {
        printf("Error reading input.\n");
        return 0;
    }
    *size = n;
    char *input = (char *)malloc(sizeof(char) * n);

    fsetpos(f, &start);
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