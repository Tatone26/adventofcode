#include "runner.h"

luint part1(void *input_v, void **args)
{
    bool **input = (bool **)input_v;
    int height = ((int *)args)[0];
    int width = ((int *)args)[1];

    int best_find = 0;

    // I hate this totally. Will redo later, and do part 2.
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (input[y][x])
            {
                bool *seen = (bool *)calloc(width * height, sizeof(bool));
                int total = 0;

                for (int dx = -1; dx >= -x; dx--)
                {
                    for (int dy = -1; dy >= -y; dy--)
                    {
                        int nx = x + dx;
                        int ny = y + dy;
                        int stop = 0;
                        // find first asteroid there (if any)
                        while (nx >= 0 && nx < width && ny >= 0 && ny < height && !seen[ny * width + nx])
                        {
                            if (!stop && input[ny][nx])
                            {
                                stop = 1;
                                total += 1;
                            }
                            seen[ny * width + nx] = 1;
                            nx = nx + dx;
                            ny = ny + dy;
                        }
                    }
                    for (int dy = 0; y + dy < height; dy++)
                    {
                        int nx = x + dx;
                        int ny = y + dy;
                        int stop = 0;
                        // find first asteroid there (if any)
                        while (nx >= 0 && nx < width && ny >= 0 && ny < height && !seen[ny * width + nx])
                        {
                            if (!stop && input[ny][nx])
                            {
                                stop = 1;
                                total += 1;
                            }
                            seen[ny * width + nx] = 1;
                            nx = nx + dx;
                            ny = ny + dy;
                        }
                    }
                }

                for (int dx = 0; x + dx < width; dx++)
                {
                    for (int dy = 0; y + dy < height; dy++)
                    {
                        int nx = x + dx;
                        int ny = y + dy;
                        int stop = 0;
                        // find first asteroid there (if any)
                        while (nx >= 0 && nx < width && ny >= 0 && ny < height && !seen[ny * width + nx])
                        {
                            if (!stop && input[ny][nx])
                            {
                                stop = 1;
                                total += 1;
                            }
                            seen[ny * width + nx] = 1;
                            nx = nx + dx;
                            ny = ny + dy;
                        }
                    }
                    for (int dy = -1; dy >= -y; dy--)
                    {
                        int nx = x + dx;
                        int ny = y + dy;
                        int stop = 0;
                        // find first asteroid there (if any)
                        while (nx >= 0 && nx < width && ny >= 0 && ny < height && !seen[ny * width + nx])
                        {
                            if (!stop && input[ny][nx])
                            {
                                stop = 1;
                                total += 1;
                            }
                            seen[ny * width + nx] = 1;
                            nx = nx + dx;
                            ny = ny + dy;
                        }
                    }
                }

                free(seen);

                if (total - 1 > best_find) // -1 to remove the current one
                    best_find = total - 1;
            }

    return best_find;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    return 0;
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    printf("input : %s : size %d\n", input, size);
    return 0;
}

// ----------------------------------------------------------------

bool **readInput(char *filename, int *height, int *width)
{
    char buffer[MAX_LINE_LEN] = {0};

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    *height = fileSize(f);
    bool **input = (bool **)malloc(sizeof(bool *) * *height);

    fpos_t start;
    fgetpos(f, &start);
    fgets(buffer, MAX_LINE_LEN, f);
    *width = strlen(buffer) - 1;
    fsetpos(f, &start);

    int y = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        bool *line = (bool *)malloc(sizeof(bool) * *width);
        for (int i = 0; i < *width; i++)
            line[i] = (buffer[i] == '#');
        input[y++] = line;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int height = 0, width = 0;
    bool **input = readInput(argv[1], &height, &width);
    int sizes[2] = {height, width};
    run(10, part1, part2, input, (void **)&sizes);
    for (int i = 0; i < height; i++)
        free(input[i]);
    free(input);
    return 0;
}