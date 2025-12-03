#include "runner.h"

luint part1(void *input_v, void **args)
{
    char **input = (char **)input_v;
    int size = ((int *)args)[0];
    int linesize = ((int *)args)[1];

    luint tot = 0;

    for (int i = 0; i < size; i++)
    {
        // simple two max search -> we absolutely need the biggest digit as the first (as long as there is at least 1 behind)
        char first_max_val = 0;
        int first_max_index = linesize + 1;
        for (int x = 0; x < linesize - 1; x++)
        {
            if (input[i][x] > first_max_val)
            {
                first_max_val = input[i][x];
                first_max_index = x;
            }
        }

        char second_max_val = 0;
        for (int y = first_max_index + 1; y < linesize; y++)
            if (input[i][y] > second_max_val)
                second_max_val = input[i][y];
        tot += (int)first_max_val * 10 + second_max_val;
    }
    return tot;
}

// -----------------------------------------------------------------

int max_index(char *input, int linesize)
{
    char max_val = 0;
    int max_index = linesize + 1;
    for (int i = 0; i < linesize; i++)
        if (input[i] > max_val)
        {
            max_val = input[i];
            max_index = i;
        }
    return max_index;
}

luint part2(void *input_v, void **args)
{
    char **input = (char **)input_v;
    int size = ((int *)args)[0];
    int linesize = ((int *)args)[1];

    luint tot = 0;

    for (int i = 0; i < size; i++)
    {
        // same idea as first part, but with 12 digits => always take the biggest one, keeping enough behind.
        luint res = 0;
        int start_index = 0;
        for (int k = 0; k < 12; k++)
        {
            int max_k = max_index(input[i] + start_index, (linesize - (12 - k)) - start_index + 1); // haha (it works)
            res = res * 10 + (input[i] + start_index)[max_k];
            start_index = (start_index + max_k) + 1; // offset by max_k the list
        }
        tot += res;
    }
    return tot;
}

// ----------------------------------------------------------------

char **readInput(char *filename, int *size, int *linesize)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    *size = fileSize(f);
    char **input = (char **)malloc(sizeof(char *) * (*size));

    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        *linesize = strlen(buffer) - 1;

        input[i] = (char *)malloc(sizeof(char) * (*linesize));
        for (int x = 0; x < *linesize; x++)
            input[i][x] = buffer[x] - '0';
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0, linesize = 0;
    char **input = readInput(argv[1], &size, &linesize);
    int args[2] = {size, linesize};
    run(3, part1, part2, input, (void **)args);
    for (int i = 0; i < size; i++)
        free(input[i]);
    free(input);
    return 0;
}
