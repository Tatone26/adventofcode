#include "runner.h"
#include <regex.h>

// I tested with regex, and it was slower :-)
long part1(int count, va_list args)
{
    if (count != 2)
    {
        printf("ERROR WITH ARGUMENTS\n");
        return -1;
    }

    char *input = va_arg(args, char *);
    int size = va_arg(args, int);

    long sum = 0;

    int x, y;
    for (char *offset_input = input; offset_input < input + size; offset_input++)
    {
        char dummy[2]; // used for forcing the pattern matching
        if (strncmp(offset_input, "mul(", 4) == 0)
        {
            // "mul(%d,%d%1[)]" -> the last "%1[)]" forces sscanf to check for a closing parenthesis following the number.
            if (sscanf(offset_input, "mul(%d,%d%1[)]", &x, &y, dummy) == 3)
            {
                sum += (x) * (y);
                offset_input += 3; // "%d,%d)" - 1
            }
            offset_input += 3; // "mul(" - 1
        }
    }

    return sum;
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

    long sum = 0;

    int x, y;
    int enabled = 1;
    for (char *offset_input = input; offset_input < input + size; offset_input++)
    {
        if (strncmp(offset_input, "do()", 4) == 0)
        {
            enabled = 1;
            offset_input += 3; // "do()" - 1
        }
        else if (strncmp(offset_input, "don't()", 7) == 0)
        {
            enabled = 0;
            offset_input += 6; // "don't()" - 1
        }
        else if (enabled)
        {
            // copy of part 1
            char dummy[2];
            if (strncmp(offset_input, "mul(", 4) == 0)
            {
                if (sscanf(offset_input, "mul(%d,%d%1[)]", &x, &y, dummy) == 3)
                {
                    sum += (x) * (y);
                    offset_input += 3;
                }
                offset_input += 3;
            }
        }
    }

    return sum;
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
    char *input = readInput("input/3.txt", &size);
    run(3, part1, part2, 2, input, size);
    free(input);
    return 0;
}