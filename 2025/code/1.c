#include "runner.h"

struct _line
{
    char dir;
    int val;
};

typedef struct _line Line;

luint part1(void *input_v, void **args)
{
    Line *input = (Line *)input_v;
    int size = ((int *)args)[0];

    int count = 0;
    int val = 50;

    for (int i = 0; i < size; i++)
    {
        switch (input[i].dir)
        {
        case 'L':
            val = (val - input[i].val) % 100;
            break;
        case 'R':
            val = (input[i].val + val) % 100;
        }
        if (val == 0)
            count++;
    }

    return count;
}

// -----------------------------------------------------------------

/**
 * Off-by-one paradise...
 */
luint part2(void *input_v, void **args)
{
    Line *input = (Line *)input_v;
    int size = ((int *)args)[0];

    int count = 0;
    int val = 50;

    for (int i = 0; i < size; i++)
    {
        int a = 0;
        switch (input[i].dir)
        {
        case 'L':
            a = -input[i].val;
            break;
        case 'R':
            a = input[i].val;
        }

        if (val + a < 0)
        {                                                     // wrapping from negative to positive
            count += -((val + a) / 100) + (val == 0 ? 0 : 1); // infamous off-by-one
            val = (100 + (val + a) % 100) % 100;
        }
        else
        { // easy case, everything is good
            count += (val + a) / 100;
            val = (val + a) % 100;
            if (a < 0 && val == 0) // NOPE if going left and landed on zero, off-by-one error
                count++;
        }
    }

    return count;
}

// ----------------------------------------------------------------

Line *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];
    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);

    *size = fileSize(f);
    Line *input = (Line *)malloc(sizeof(Line) * (*size));
    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        input[i].dir = buffer[0];
        input[i].val = atoi(buffer + 1);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Line *input = readInput(argv[1], &size);
    run(1, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}