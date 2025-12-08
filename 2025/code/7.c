#include "runner.h"

// Easy day ! Just a bit of problem comprehension :)

luint part1(void *input_v, void **args)
{
    char **input = (char **)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    char *ray = (char *)calloc(width, sizeof(char));
    for (int x = 0; x < width; x++) // finding start position
        if (input[0][x] == 'S')
        {
            ray[x] = 1;
            break;
        }

    luint count = 0;

    for (int i = 1; i < height; i++)
    {
        for (int j = 1; j < width - 1; j++) // no '^' on the borders !
        {
            assert(j < width); // just making linter happy
            if (!ray[j])
                continue;
            if (input[i][j] == '^')
            {
                count++;
                ray[j] = 0; // we can reset because never two '^' next to each other
                ray[j - 1] = 1;
                ray[j + 1] = 1;
            }
        }
    }

    free(ray);
    return count;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    char **input = (char **)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    luint *ray = (luint *)calloc(width, sizeof(luint));
    for (int x = 0; x < width; x++) // finding start position
        if (input[0][x] == 'S')
        {
            ray[x] = 1;
            break;
        }

    luint count = 0;

    for (int i = 1; i < height; i++)
    {
        for (int j = 1; j < width - 1; j++) // no '^' on the borders !
        {
            assert(j < width); // just making linter happy
            if (!ray[j])
                continue;
            if (input[i][j] == '^')
            {
                // only difference : count the number of rays that arrived here... instead of just counting 1.
                ray[j - 1] += ray[j];
                ray[j + 1] += ray[j];
                ray[j] = 0; // we can reset because never two '^' next to each other
            }
        }
    }

    // last row = number of rays that arrived there => sum = nb of timelines total
    for (int x = 0; x < width; x++)
        count += ray[x];

    free(ray);
    return count;
}

// ----------------------------------------------------------------

char **readInput(char *filename, int *width, int *height)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    int temp = fileSize(f);
    char **input = (char **)malloc(sizeof(char *) * (temp));
    *height = temp;
    for (int i = 0; i < temp; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        input[i] = strndup(buffer, strlen(buffer) - 1);
        int l = strlen(input[i]);
        if (l > *width)
            *width = l;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int height = 0, width = 0;
    char **input = readInput(argv[1], &width, &height);
    int args[2] = {width, height};
    run(7, part1, part2, input, (void **)args);
    for (int i = 0; i < height; i++)
        free(input[i]);
    free(input);
    return 0;
}