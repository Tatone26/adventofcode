#include "runner.h"

// I guess a way of speeding things up is some kind of cache ? Don't really know what can be cached though.
// First day I don't see how to bring under 10 ms (for part 2)

const Pos DIRECTIONS[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

luint countCheats(Pos *path, int path_size, int *costs, int width, int height, int bound, int duration)
{
    luint total = 0;
    // try every single position in the track
    Pos currentPos;
    for (int i = 0; i < path_size; i++)
    {
        currentPos = path[i];
        const int ymin = (currentPos.y >= duration ? currentPos.y - duration : 0);
        const int ymax = (currentPos.y + duration < height ? currentPos.y + duration : height - 1);
        for (int y = ymin; y <= ymax; y++)
        {
            const int dy = abs((int)(currentPos.y - y));
            const int xmin = (currentPos.x >= duration - dy ? currentPos.x - duration + dy : 0);
            const int xmax = (currentPos.x + duration - dy < width ? currentPos.x + duration - dy : width - 1);
            for (int x = xmin; x <= xmax; x++)
            {
                const int d = dy + abs((int)(currentPos.x - x));
                if (d > 1 && costs[INDEX(y, x)] != -1)
                {
                    int test = costs[INDEX(y, x)] - costs[INDEX_POS(currentPos)] - d;
                    if (test >= bound)
                        total++;
                }
            }
        }
    }
    return total;
}

luint commonPart(void *input_v, void **args, int bound, int duration)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];
    // find start and end positions
    Pos start = (Pos){-1, -1};
    for (int i = 0; start.x == -1 && i < height; i++)
    {
        for (int j = 0; start.x == -1 && j < width; j++)
        {
            if (input[INDEX(i, j)] == 'S')
                start = (Pos){j, i};
        }
    }
    if (start.x == -1)
    {
        printf("No starting or ending point in input.\n");
        return 0;
    }
    // read the path (only one choice in the path)
    int *costs = (int *)malloc(sizeof(int) * width * height);
    memset(costs, 255, sizeof(int) * width * height); // set at MAX_INT

    Pos *path = (Pos *)malloc(sizeof(Pos) * width * height);
    int path_size = 0;
    path[0] = start;

    int lastDirection = -3;
    Pos currentPos = start;
    costs[INDEX_POS(start)] = 0;
    while (input[INDEX_POS(currentPos)] != 'E')
    {
        for (int d = 0; d < 4; d++)
        {
            if (d == (lastDirection + 2) % 4)
                continue;
            Pos next = (Pos){currentPos.x + DIRECTIONS[d].x, currentPos.y + DIRECTIONS[d].y};
            if (INBOUNDS(next, width, height) && input[INDEX_POS(next)] != '#')
            {
                costs[INDEX_POS(next)] = ++path_size;
                path[path_size] = next;
                lastDirection = d;
                currentPos = next;
                break;
            }
        }
    }

    luint res = countCheats(path, path_size, costs, width, height, bound, duration);

    free(path);
    free(costs);
    return res;
}

luint part1(void *input_v, void **args)
{
    return commonPart(input_v, args, 100, 2);
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    return commonPart(input_v, args, 100, 20);
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *width, int *height)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *height = fileSize(f);
    fpos_t start;
    fgetpos(f, &start);
    fgets(buffer, MAX_LINE_LEN, f);
    *width = strlen(buffer) - 1;
    fsetpos(f, &start);
    char *input = (char *)malloc(sizeof(char) * *width * *height);

    for (int i = 0; i < *height; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        strncpy(input + i * *width, buffer, *width);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int sizes[2] = {0, 0};
    char *input = readInput(argv[1], &sizes[0], &sizes[1]);
    run(20, part1, part2, input, (void **)&sizes);
    free(input);
    return 0;
}