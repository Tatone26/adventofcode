#include "runner.h"

// I think part 2 could be faster, but I don't see how right now. ~35 ms on my laptop on a single core is pretty good, I guess

typedef struct
{
    int x;
    int y;
} Pos;

const Pos DIRECTIONS[4] = {{0, -1},
                           {1, 0},
                           {0, 1},
                           {-1, 0}};

luint part1(va_list args)
{
    char *input = va_arg(args, char *);
    int width = va_arg(args, int);
    int height = va_arg(args, int);

    // Finding the start Position
    Pos start = {-1, -1};
    for (int i = 0; start.x == -1 && i < height; i++)
        for (int j = 0; start.x == -1 && j < width; j++)
            if (input[INDEX(i, j)] == '^')
                start = (Pos){j, i};
    // safety check
    if (start.x == -1)
        return 0;
    // seen[INDEX(y, x)] = true if position already seen
    bool *seen = (bool *)calloc(sizeof(bool), width * height);

    // counting the starting position
    (seen[INDEX(start.y, start.x)]) = true;
    luint res = 1;

    int direction = 0;
    Pos position = start;
    // Looping while we are in the map
    while (1)
    {
        Pos next = (Pos){position.x + DIRECTIONS[direction].x, position.y + DIRECTIONS[direction].y};
        // Break if the next position is outside of the map
        if (next.x < 0 || next.x >= width || next.y < 0 || next.y >= height)
            break;
        int index = INDEX(next.y, next.x);
        // if can't move, turn
        if (input[index] == '#')
            direction = (direction + 1) % 4; // turn 90°
        // If can move, move
        else
        {
            position = next;
            // if never seen before, count it
            if (!seen[index])
            {
                res++;
                seen[index] = true;
            }
        }
    }

    free(seen);
    return res;
}

// ----------------------------------------------------------------

bool checkForLoop(char *input, bool (*path)[4], int width, int height, Pos position, int direction, Pos blocked)
{
    while (1)
    {
        Pos next = (Pos){position.x + DIRECTIONS[direction].x, position.y + DIRECTIONS[direction].y};
        if (next.x < 0 || next.x >= width || next.y < 0 || next.y >= height)
            break;
        // If can't move, turn
        if (input[INDEX(next.y, next.x)] == '#' || (next.x == blocked.x && next.y == blocked.y))
            direction = (direction + 1) % 4; // turn 90°
        // If can move, do and update seen
        else
        {
            // If we have already been on this position with this direction, loop found
            if (path[INDEX(next.y, next.x)][direction])
                return true;
            // update path
            path[INDEX(next.y, next.x)][direction] = true;
            position = next;
        }
    }
    return false;
}

luint part2(va_list args)
{
    char *input = va_arg(args, char *);
    int width = va_arg(args, int);
    int height = va_arg(args, int);

    // Find starting position
    Pos start = {-1, -1};
    for (int i = 0; start.y == -1 && i < height; i++)
        for (int j = 0; start.y == -1 && j < width; j++)
            if (input[INDEX(i, j)] == '^')
                start = (Pos){j, i};
    // safety check
    if (start.x == -1)
        return 0;
    // path[INDEX][i] = true means that we have already been on pos INDEX with direction i
    bool(*path)[4] = (bool(*)[4])calloc(sizeof(bool[4]), width * height);
    bool(*temp_path)[4] = (bool(*)[4])malloc(sizeof(bool[4]) * width * height);

    bool *tested = (bool *)calloc(sizeof(bool), width * height);
    // cannot put anything on the starting position.
    tested[INDEX(start.y, start.x)] = true;

    luint res = 0;

    Pos current_pos = start;
    int current_direction = 0;
    while (1)
    {
        Pos next_position = (Pos){current_pos.x + DIRECTIONS[current_direction].x, current_pos.y + DIRECTIONS[current_direction].y};
        // Break if the next position is outside of the map
        if (next_position.x < 0 || next_position.x >= width || next_position.y < 0 || next_position.y >= height)
            break;

        int index = INDEX(next_position.y, next_position.x);
        // if can't move, turn
        if (input[index] == '#')
            current_direction = (current_direction + 1) % 4; // turn 90°
        // If can move, move
        else
        {
            // If we haven't already, analyze the rest of the path considering that the next pos is blocked.
            if (!tested[index])
            {
                memcpy(temp_path, path, sizeof(bool[4]) * width * height);
                if (checkForLoop(input, temp_path, width, height, current_pos, current_direction, next_position))
                    res++;
                tested[index] = true;
            }
            // update global pos and path
            path[index][current_direction] = true;
            current_pos = next_position;
        }
    }

    free(path);
    free(tested);
    free(temp_path);
    return res;
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

    char *input = (char *)malloc(sizeof(char) * *width * *height);

    fsetpos(f, &start);
    for (int j = 0; j < *height; j++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        strncpy(input + (j * *width), buffer, *width);
    }

    fclose(f);
    return input;
}

int main()
{
    int width = 0;
    int height = 0;
    char *input = readInput("input/6.txt", &width, &height);
    if (!input)
    {
        printf("error\n");
        return 1;
    }
    run(6, part1, part2, 3, input, width, height);
    free(input);
    return 0;
}