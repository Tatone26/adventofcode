#include "runner.h"

// Definitely not the most efficient way (because of recursion and all) but it runs in under 4ms for both parts and part 2 isn't significally slower.
// A better way for part 1 would probably be looking at the plot without recursion (using a heap and doing BFS I guess)
// Same for part 2, it is probably possible without recursion, maybe with some kind of "agent" that follows the walls only ?

const Pos DIRECTIONS[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

typedef struct
{
    int perimeter;
    int area;
    int sides;
} PlotInfo;

PlotInfo analyzeBigPlot(char *input, Pos start, int width, int height, bool *seen)
{
    // check all four sides : if different perimeter + 1 else recursion
    seen[INDEX_POS(start)] = true;
    char c = input[INDEX_POS(start)];
    PlotInfo res = (PlotInfo){0, 1, 0};
    // for all directions
    for (int d = 0; d < 4; d++)
    {
        Pos next = (Pos){start.x + DIRECTIONS[d].x, start.y + DIRECTIONS[d].y};
        if (INBOUNDS(next, width, height) && input[INDEX_POS(next)] == c && !seen[INDEX_POS(next)])
        {
            // if in bounds, never seen and same character, do recurrence
            PlotInfo rec = analyzeBigPlot(input, next, width, height, seen);
            res = (PlotInfo){res.perimeter + rec.perimeter, res.area + rec.area, 0};
        }
        else if (!INBOUNDS(next, width, height) || input[INDEX_POS(next)] != c)
        {
            // else if it is not in bounds or not the same character, we are at the limit of the plot
            res.perimeter += 1;
        }
    }
    return res;
}

luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    luint sum = 0;

    bool *seen = (bool *)calloc(width * height, sizeof(bool));
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            if (!seen[INDEX(i, j)])
            {
                PlotInfo res = analyzeBigPlot(input, (Pos){j, i}, width, height, seen);
                sum += res.area * res.perimeter;
            }
        }

    free(seen);
    return sum;
}

// -----------------------------------------------------------------

void updateCounted(char *input, Pos start, int width, int height, int (*counted)[4], int counter, int direction)
{
    *(counted[INDEX_POS(start)] + direction) = counter;
    char c = input[INDEX_POS(start)];
    Pos current, test;
    switch (direction)
    {
    case 0:
    case 2:
        current = (Pos){start.x + 1, start.y};
        // following the wall until end of map or found hole or hill
        for (; current.x < width; (current.x)++)
        {
            test = (Pos){current.x, start.y + (direction == 0 ? -1 : 1)};
            // look for "hole" or up / down "hill"
            if ((input[INDEX_POS(current)] != c) ||
                (INBOUNDS(test, width, height) && input[INDEX_POS(test)] == c))
                break;
            // if no hole or hill, update the array
            *(counted[INDEX_POS(current)] + direction) = counter;
        }
        current.x = start.x - 1;
        for (; current.x >= 0; (current.x--))
        {
            test = (Pos){current.x, start.y + (direction == 0 ? -1 : 1)};
            // look for "hole" or up / down "hill"
            if ((input[INDEX_POS(current)] != c) ||
                (INBOUNDS(test, width, height) && input[INDEX_POS(test)] == c))
                break;
            // if no hole or hill, update the array
            *(counted[INDEX_POS(current)] + direction) = counter;
        }
        break;
    case 1:
    case 3:
        current = (Pos){start.x, start.y + 1};
        for (; current.y < height; (current.y)++)
        {
            Pos test = (Pos){start.x + (direction == 1 ? 1 : -1), current.y};
            // look for "hole" or right / left "hill"
            if ((input[INDEX_POS(current)] != c) ||
                (INBOUNDS(test, width, height) && input[INDEX_POS(test)] == c))
                break;
            // if no hole or hill, update the array
            *(counted[INDEX_POS(current)] + direction) = counter;
        }
        current.y = start.y - 1;
        for (; current.y >= 0; (current.y)--)
        {
            Pos test = (Pos){start.x + (direction == 1 ? 1 : -1), current.y};
            // look for "hole" or right / left "hill"
            if ((input[INDEX_POS(current)] != c) ||
                (INBOUNDS(test, width, height) && input[INDEX_POS(test)] == c))
                break;
            // if no hole or hill, update the array
            *(counted[INDEX_POS(current)] + direction) = counter;
        }
        break;
    }
}

PlotInfo analyzeBigPlotBis(char *input, Pos start, int width, int height, bool *seen, int (*counted)[4], int counter)
{
    char c = input[INDEX_POS(start)];
    seen[INDEX_POS(start)] = true;
    PlotInfo res = (PlotInfo){0, 1, 0};
    // for all 4 directions
    for (int d = 0; d < 4; d++)
    {
        Pos next = (Pos){start.x + DIRECTIONS[d].x, start.y + DIRECTIONS[d].y};
        if (INBOUNDS(next, width, height) && input[INDEX_POS(next)] == c && !seen[INDEX_POS(next)])
        {
            // if in bounds, never seen and same character, do recurrence
            PlotInfo rec = analyzeBigPlotBis(input, next, width, height, seen, counted, counter);
            res = (PlotInfo){0, res.area + rec.area, res.sides + rec.sides};
        }
        else if ((!INBOUNDS(next, width, height) || input[INDEX_POS(next)] != c) &&
                 counted[INDEX_POS(start)][d] != counter)
        // if we haven't counted this side yet, do and update all sides
        {
            res.sides += 1;
            updateCounted(input, start, width, height, counted, counter, d);
        }
    }
    return res;
}

luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    luint sum = 0;
    // used to make sure we don't check any place twice
    bool *seen = (bool *)calloc(width * height, sizeof(bool));
    // used to make sure we don't count a side twice (use counter to know which value to use)
    int(*counted)[4] = (int(*)[4])calloc(width * height, sizeof(int[4]));
    int counter = 1;
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            if (!seen[INDEX(i, j)])
            {
                PlotInfo res = analyzeBigPlotBis(input, (Pos){j, i}, width, height, seen, counted, counter);
                sum += res.area * res.sides;
            }
        }

    free(counted);
    free(seen);
    return sum;
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
    fsetpos(f, &start);
    *width = strlen(buffer) - 1;
    char *input = (char *)malloc(sizeof(char) * *height * *width);

    for (int y = 0; y < *height; y++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        strncpy(input + y * *width, buffer, *width);
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

    run(12, part1, part2, input, (void **)sizes);
    free(input);
    return 0;
}