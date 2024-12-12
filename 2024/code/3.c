#include "runner.h"
#include <regex.h>

// I tested with regex, and it was slower :-)
luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    luint sum = 0;

    int x, y;
    for (int offset_input = 0;
         offset_input < size; offset_input++)
    {
        char dummy[2]; // used for forcing the pattern matching
        if (offset_input + 4 <= size && strncmp(input + offset_input, "mul(", 4) == 0)
        {
            // "mul(%d,%d%1[)]" -> the last "%1[)]" forces sscanf to check for a closing parenthesis following the number.
            if (sscanf(input + offset_input, "mul(%d,%d%1[)]", &x, &y, dummy) == 3)
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

luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    luint sum = 0;

    int x, y;
    int enabled = 1;
    for (int offset_input = 0; offset_input < size; offset_input++)
    {
        if (strncmp(input + offset_input, "do()", 4) == 0)
        {
            enabled = 1;
            offset_input += 3; // "do()" - 1
        }
        else if (strncmp(input + offset_input, "don't()", 7) == 0)
        {
            enabled = 0;
            offset_input += 6; // "don't()" - 1
        }
        else if (enabled)
        {
            // copy of part 1
            char dummy[2];
            if (strncmp(input + offset_input, "mul(", 4) == 0)
            {
                if (sscanf(input + offset_input, "mul(%d,%d%1[)]", &x, &y, dummy) == 3)
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
    char *input = (char *)malloc(sizeof(char) * n);

    fsetpos(f, &start);
    int offset = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        strncpy(input + offset, buffer, strlen(buffer) - 1); // -1 to remove \n
        offset += strlen(buffer) - 1;
        input[offset] = '\0';
    }
    *size = strlen(input);

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    char *input = readInput(argv[1], &size);
    run(3, part1, part2, input, (void *)&size);
    free(input);
    return 0;
}