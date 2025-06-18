#include "runner.h"

const Pos DIRECTIONS[4] = {{0, -1},
                           {1, 0},
                           {0, 1},
                           {-1, 0}};

/// @brief Part 1 : just move the guard step by step, counting every new one.
luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    // Finding the start Position
    Pos start = {-1, -1};
    for (int i = 0; start.x == -1 && i < height; i++)
        for (int j = 0; start.x == -1 && j < width; j++)
            if (input[INDEX(i, j)] == '^')
                start = (Pos){j, i};
    // safety check
    if (start.x == -1 || start.y == -1)
        return 0;

    // seen[INDEX(y, x)] = true if position already seen
    bool *seen = (bool *)calloc(sizeof(bool), width * height);
    // counting the starting position
    (seen[INDEX(start.y, start.x)]) = true;
    luint res = 1;

    Pos current_pos = start;
    int current_direction = 0;
    while (1)
    {
        Pos next_position = (Pos){current_pos.x + DIRECTIONS[current_direction].x, current_pos.y + DIRECTIONS[current_direction].y};
        // Break if the next position is outside of the map
        if (!INBOUNDS(next_position, width, height))
            break;

        int index = INDEX(next_position.y, next_position.x);
        // if can't move, turn 90° else move
        if (input[index] == '#')
            current_direction = (current_direction + 1) % 4;
        else
        {
            if (!seen[index])
            {
                res++;
                seen[index] = true;
            }
            current_pos = next_position;
        }
    }

    free(seen);
    return res;
}

// ----------------------------------------------------------------

/// @brief Looks for a loop in the guard's path
///
/// Uses the already-computed path, pre-processed rows and columns and a given position to block too.
bool checkForLoop(int (*path)[4], int width, Pos position, int direction, int **columns, int **rows, Pos blocked, int counter)
{
    while (1)
    {
        Pos next = {-1, -1};
        // Find next obstacle and get the position just before it
        switch (direction) // north
        {
        case 0:
            next.x = position.x; // x doesn't change
            for (int i = columns[position.x][0]; i > 0; i--)
                // loop over all obstacles on this column, starting from the bottom
                if (columns[position.x][i] < position.y)
                {
                    // stop at the first obstacle on the path
                    next.y = columns[position.x][i] + 1;
                    break;
                }
            // if need, consider the "blocked" position too
            if (blocked.x == position.x && blocked.y < position.y && (blocked.y >= next.y || next.y == -1))
                next.y = blocked.y + 1;
            break;
        case 1: // east
            next.y = position.y;
            for (int i = 1; i < rows[position.y][0] + 1; i++)
                if (rows[position.y][i] > position.x)
                {
                    next.x = rows[position.y][i] - 1;
                    break;
                }
            if (blocked.y == position.y && blocked.x > position.x && (blocked.x <= next.x || next.x == -1))
                next.x = blocked.x - 1;
            break;
        case 2: // south
            next.x = position.x;
            for (int i = 1; i < columns[position.x][0] + 1; i++)
                if (columns[position.x][i] > position.y)
                {
                    next.y = columns[position.x][i] - 1;
                    break;
                }
            if (blocked.x == position.x && blocked.y > position.y && (blocked.y <= next.y || next.y == -1))
                next.y = blocked.y - 1;
            break;
        case 3: // west
            next.y = position.y;
            for (int i = rows[position.y][0]; i > 0; i--)
                if (rows[position.y][i] < position.x)
                {
                    next.x = rows[position.y][i] + 1;
                    break;
                }
            if (blocked.y == position.y && blocked.x < position.x && (blocked.x >= next.x || next.x == -1))
                next.x = blocked.x + 1;
            break;
        }

        // if there were no obstacles on the path, then we are out of the map.
        if (next.x == -1 || next.y == -1)
            return false;
        // turn 90°
        direction = (direction + 1) % 4;
        // check if position already seen
        if (path[INDEX(next.y, next.x)][direction] == 1 || path[INDEX(next.y, next.x)][direction] == counter)
            return true;
        // update path, will never change a "1"
        path[INDEX(next.y, next.x)][direction] = counter;
        position = next;
    }
}

/// @brief Processes the input into lists of positions of obstacles, one per column (or row). First value being the total number of obstacles.
int **processInput(char *input, int width, int height, bool columns)
{
    int **array = (int **)malloc(sizeof(int *) * (columns ? width : height));
    for (int i = 0; i < (columns ? width : height); i++)
    {
        array[i] = (int *)malloc(sizeof(int) * (columns ? width + 1 : height + 1));
        int d = 1;
        for (int j = 0; j < (columns ? height : width); j++)
            if (input[INDEX(columns ? j : i, columns ? i : j)] == '#')
            {
                array[i][d] = j;
                d++;
            }
        array[i][0] = d - 1; // nb of obstacles found
    }
    return array;
}

/// @brief Part 2 : pre-process the input into lists of obstacles positions per row or column. Allows "teleporting" to the next obstacle more efficiently
/// when looking for loops. Still moves step by step like part one to find the start of the path and the possible positions.
luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    luint res = 0;

    // Find starting position
    Pos start = {-1, -1};
    for (int i = 0; start.y == -1 && i < height; i++)
        for (int j = 0; start.y == -1 && j < width; j++)
            if (input[INDEX(i, j)] == '^')
                start = (Pos){j, i};

    // To gain time later : pre-processing the input into two arrays, for the column and the rows, keeping only the coordinates of the blocks
    // Each array is made as [size, pos1, pos2]... with size being the number of positions
    int **columns = processInput(input, width, height, true);
    int **rows = processInput(input, width, height, false);

    // path[INDEX][i] = true means that we have already been on pos INDEX with direction i
    int (*path)[4] = (int (*)[4])calloc(sizeof(int[4]), width * height);
    bool *tested = (bool *)calloc(sizeof(bool), width * height);
    // this "test_counter" value allows multiple instances of the checkLoop function and the main loop to share the same "path" instead of copying it
    int test_counter = 2;
    // safety check
    if (start.x == -1)
        goto end;
    // cannot put anything on the starting position.
    tested[INDEX(start.y, start.x)] = true;

    Pos current_pos = start;
    int current_direction = 0;
    while (1)
    {
        Pos next_position = (Pos){current_pos.x + DIRECTIONS[current_direction].x, current_pos.y + DIRECTIONS[current_direction].y};
        // Break if the next position is outside of the map
        if (!INBOUNDS(next_position, width, height))
            break;

        int index = INDEX(next_position.y, next_position.x);
        if (input[index] == '#')
            current_direction = (current_direction + 1) % 4; // turn 90°
        else
        {
            // If we haven't already, analyze the rest of the path considering that the next pos is blocked.
            if (!tested[index])
            {
                if (checkForLoop(path, width, current_pos, current_direction, columns, rows, next_position, test_counter))
                    res++;
                tested[index] = true;
                test_counter++;
            }
            // update global pos and path
            path[index][current_direction] = 1;
            current_pos = next_position;
        }
    }
end:
    for (int i = 0; i < width; i++)
        free(columns[i]);
    free(columns);
    for (int i = 0; i < height; i++)
        free(rows[i]);
    free(rows);

    free(path);
    free(tested);
    return res;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *width, int *height)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;
    *height = fileSize(f);
    fpos_t start;
    fgetpos(f, &start);

    if (!fgets(buffer, MAX_LINE_LEN, f))
    {
        fclose(f);
        return NULL;
    }
    *width = strlen(buffer) - 1;

    char *input = (char *)malloc(sizeof(char) * *width * *height);

    fsetpos(f, &start);
    for (int j = 0; j < *height; j++)
    {
        if (!fgets(buffer, MAX_LINE_LEN, f))
        {
            free(input);
            fclose(f);
            return NULL;
        }
        strncpy(input + (j * *width), buffer, *width);
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
    run(6, part1, part2, input, (void **)&sizes);
    free(input);
    return 0;
}