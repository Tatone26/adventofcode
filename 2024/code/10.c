#include "runner.h"

const Pos DIRECTIONS[4] = {{0, -1},
                           {1, 0},
                           {0, 1},
                           {-1, 0}};

int trailPath(short *input, bool *seen, Pos position, int width, int height)
{
    if (seen && seen[INDEX_POS(position)])
        return 0;
    if (seen)
        seen[INDEX_POS(position)] = true;
    if (input[INDEX_POS(position)] == 9)
        return 1;
    int res = 0;
    int check = input[INDEX_POS(position)] + 1;
    // unfolded loop on all four directions
    Pos next = (Pos){position.x + DIRECTIONS[0].x, position.y + DIRECTIONS[0].y};
    if (INBOUNDS(next, width, height) && input[INDEX_POS(next)] == check)
        res += trailPath(input, seen, next, width, height);

    next = (Pos){position.x + DIRECTIONS[1].x, position.y + DIRECTIONS[1].y};
    if (INBOUNDS(next, width, height) && input[INDEX_POS(next)] == check)
        res += trailPath(input, seen, next, width, height);

    next = (Pos){position.x + DIRECTIONS[2].x, position.y + DIRECTIONS[2].y};
    if (INBOUNDS(next, width, height) && input[INDEX_POS(next)] == check)
        res += trailPath(input, seen, next, width, height);

    next = (Pos){position.x + DIRECTIONS[3].x, position.y + DIRECTIONS[3].y};
    if (INBOUNDS(next, width, height) && input[INDEX_POS(next)] == check)
        res += trailPath(input, seen, next, width, height);

    return res;
}

/// @brief Both parts are so similar that copying all the code would be stupid
luint commonPart(short *input, int width, int height, bool part2)
{
    luint res = 0;

    bool *seen = (bool *)malloc(width * height * sizeof(bool));

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (input[INDEX(i, j)] == 0)
            {
                if (!part2)
                    memset(seen, 0, width * height * sizeof(bool));
                res += trailPath(input, part2 ? NULL : seen, (Pos){j, i}, width, height);
            }

    free(seen);
    return res;
}

luint part1(void *input_v, void **args)
{
    return commonPart((short *)input_v, ((int *)args)[0], ((int *)args)[1], false);
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    return commonPart((short *)input_v, ((int *)args)[0], ((int *)args)[1], true);
}

// ----------------------------------------------------------------

short *readInput(char *filename, int *width, int *height)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *height = fileSize(f);
    fpos_t start;
    fgetpos(f, &start);
    fgets(buffer, MAX_LINE_LEN, f);
    *width = strlen(buffer) - 1;

    short *input = (short *)malloc(sizeof(short) * *width * *height);

    fsetpos(f, &start);
    for (int i = 0; i < *height; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        for (int j = 0; j < *width; j++)
            input[i * *width + j] = buffer[j] - '0';
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
    short *input = readInput(argv[1], &sizes[0], &sizes[1]);

    run(10, part1, part2, input, (void **)sizes);
    free(input);
    return 0;
}