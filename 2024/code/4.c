#include "runner.h"

// #define INPUT(y, x) input[(y) * width + (x)]
#define INPUT(y, x) input[INDEX(y, x)]

int checkPattern(char *input, int y, int x, int dy, int dx, int width)
{
    const char TEST1[4] = {'X', 'M', 'A', 'S'};
    const char TEST2[4] = {'S', 'A', 'M', 'X'};
    // horizontal
    char check = 3; // check = 0b11
    for (int t = 0; t < 4; t++)
    {
        int d = INPUT(y + dy * t, x + dx * t);
        if (check & 1 && d != TEST1[t])
        {
            check &= 2; // remove last bit
            if (!check)
                return 0;
        }
        if (check & 2 && d != TEST2[t])
        {
            check &= 1; // remove first bit
            if (!check)
                return 0;
        }
    }
    return 1;
}

// > 1ms, which is pretty bad for this simple function.
luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    luint result = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width - 3; x++)
        {
            // no need to check 'A' or 'M'
            if (INPUT(y, x) != 'X' && INPUT(y, x) != 'S')
                continue;

            // horizontal
            if (checkPattern(input, y, x, 0, 1, width))
                result++;

            // diagonal up
            if (y >= 3 && checkPattern(input, y, x, -1, 1, width))
                result++;

            if (y < height - 3)
            {
                // diagonal down
                if (checkPattern(input, y, x, 1, 1, width))
                    result++;
                // vertical down
                if (checkPattern(input, y, x, 1, 0, width))
                    result++;
            }
        }
        if (y < height - 3)
        {
            for (int x = width - 3; x < width; x++)
            {
                // vertical down (the last 3 columns not tested before)
                if (checkPattern(input, y, x, 1, 0, width))
                    result++;
            }
        }
    }

    return result;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    luint result = 0;
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++) // we test only diagonals going to the right so no need to check to close to the right
        {
            // checking only for 'A'
            if (INPUT(y, x) != 'A')
                continue;
            // checking if the diagonals are good
            if (((INPUT(y - 1, x - 1) == 'M' && INPUT(y + 1, x + 1) == 'S') || (INPUT(y - 1, x - 1) == 'S' && INPUT(y + 1, x + 1) == 'M')) && // first diagonal
                ((INPUT(y + 1, x - 1) == 'M' && INPUT(y - 1, x + 1) == 'S') || (INPUT(y + 1, x - 1) == 'S' && INPUT(y - 1, x + 1) == 'M')))   // second diagonal
                result++;
        }
    }

    return result;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *width, int *height)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;
    fpos_t start;
    fgetpos(f, &start);

    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
        (*height)++;
    if (*height == 0)
    {
        printf("Error reading input.\n");
        fclose(f);
        return 0;
    }

    fsetpos(f, &start);
    if (!fgets(buffer, MAX_LINE_LEN, f))
    {
        fclose(f);
        return NULL;
    }
    *width = strlen(buffer) - 1;

    fsetpos(f, &start);

    char *input = (char *)malloc(sizeof(char) * *height * *width);

    for (int y = 0; y < *height; y++)
    {
        if (!fgets(buffer, MAX_LINE_LEN, f))
        {
            free(input);
            fclose(f);
            return NULL;
        }
        strncpy(&(input[y * *width]), buffer, *width);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int sizes[2] = {0,
                    0};
    char *input = readInput(argv[1], &sizes[0], &sizes[1]);

    run(4, part1, part2, input, (void **)sizes);

    free(input);
    return 0;
}