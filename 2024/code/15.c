#include "runner.h"

// Could be done without recursion, but recursion gives a very clear and nice looking code.

const Pos DIRECTIONS[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

Pos moveFromChar(char c)
{
    switch (c)
    {
    case '^':
        return DIRECTIONS[0];
    case '>':
        return DIRECTIONS[1];
    case 'v':
        return DIRECTIONS[2];
    case '<':
        return DIRECTIONS[3];
    }
    return (Pos){0, 0};
}

// First part : count 'O', second part : count '['
luint sumGPS(char *world, int width, int height)
{
    luint res = 0;
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if ((world[INDEX(i, j)] == 'O') || (world[INDEX(i, j)] == '['))
                res += i * 100 + j;
    return res;
}

void displayWorld(char *world, int width, int height, Pos robot)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (robot.x == j && robot.y == i)
                printf("@");
            else
                printf("%c", world[INDEX(i, j)]);
        }
        printf("\n");
    }
}

/// @brief Considering that the box can move, moves the box and all the one it pushes.
/// @param box Represents the position of the current HALF box for part 2, full box for part 1
/// !! Considering that the move is possible !!
void moveBox(char *world, int width, Pos box, Pos direction)
{
    // Next block in the given direction
    Pos next = (Pos){box.x + direction.x, box.y + direction.y};
    char c = world[INDEX_POS(next)];
    if (c == 'O' || c == '[' || c == ']')
    {
        moveBox(world, width, next, direction);
        if (c != 'O' && direction.y != 0)
            moveBox(world, width, (Pos){next.x + (c == '[' ? 1 : -1), next.y}, direction);
    }
    world[INDEX_POS(next)] = world[INDEX_POS(box)];
    world[INDEX_POS(box)] = '.';
}

bool moveWorld(char *world, int width, Pos robot, Pos direction)
{
    // check if movable
    Pos next = (Pos){robot.x + direction.x, robot.y + direction.y};
    while (world[INDEX_POS(next)] == 'O')
        next = (Pos){next.x + direction.x, next.y + direction.y};
    if (world[INDEX_POS(next)] == '#')
        return false;
    // move if movable
    moveBox(world, width, robot, direction);
    return true;
}

luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = *((int **)args)[0];
    int height = *((int **)args)[1];
    char *moves = *((char ***)args)[2];
    int size = *((int **)args)[3];

    char *world = (char *)malloc(sizeof(char) * width * height);
    memcpy(world, input, sizeof(char) * width * height);

    luint res = 0;
    Pos robot = (Pos){-1, -1};
    // get starting position
    for (int i = 0; robot.x == -1 && i < height; i++)
        for (int j = 0; robot.x == -1 && j < width; j++)
            if (world[INDEX(i, j)] == '@')
                robot = (Pos){j, i};
    if (robot.x == -1 || robot.y == -1)
        goto end;
    world[INDEX_POS(robot)] = '.';
    // Iterate over all instructions
    for (int m = 0; m < size; m++)
    {
        // Get direction
        Pos move = moveFromChar(moves[m]);
        // Try to move world, and move robot if neccesary
        if (moveWorld(world, width, robot, move))
            robot = (Pos){robot.x + move.x, robot.y + move.y};
    }
    // calculate result
    res = sumGPS(world, width, height);
end:
    free(world);
    return res;
}

// -----------------------------------------------------------------

/// @brief Return true if the given box can move without issue in the given direction (recursive function)
/// For part 2 only
bool canMove(char *world, int width, Pos box, Pos direction)
{
    // Next block in the given direction
    Pos next = (Pos){box.x + direction.x, box.y + direction.y};
    // Easy cases
    if (world[INDEX_POS(next)] == '#')
        return false;
    if (world[INDEX_POS(next)] == '.')
        return true;
    // Else : the next block is a box (2x1)
    if (direction.x == 0)
    { // going up or down, need to check if both sides of the box can move.
        return canMove(world, width, next, direction) &&
               canMove(world, width, (Pos){next.x + (world[INDEX_POS(next)] == ']' ? -1 : +1), next.y}, direction);
    }
    if (direction.y == 0)
    { // going right or left, need to check the interesting side of the box (the one in contact with potential other boxes or obstacles)
        if (direction.x == -1 && world[INDEX_POS(next)] == ']')
            return canMove(world, width, (Pos){next.x - 1, next.y}, direction);
        else if (direction.x == 1 && world[INDEX_POS(next)] == '[')
            return canMove(world, width, (Pos){next.x + 1, next.y}, direction);
        else
            return canMove(world, width, next, direction);
    }

    return false;
}

// Trying to move the robot here
int moveWorldBis(char *world, int width, Pos robot, Pos direction)
{
    Pos next = (Pos){robot.x + direction.x, robot.y + direction.y};
    if (world[INDEX_POS(next)] == '#')
        return false;
    if (world[INDEX_POS(next)] == '.')
        return true;
    if (world[INDEX_POS(next)] == '[')
        if (canMove(world, width, next, direction) && canMove(world, width, (Pos){next.x + 1, next.y}, direction))
        {
            moveBox(world, width, next, direction);
            if (direction.y != 0)
                moveBox(world, width, (Pos){next.x + 1, next.y}, direction);

            return true;
        }
    if (world[INDEX_POS(next)] == ']')
    {
        if (canMove(world, width, next, direction) && canMove(world, width, (Pos){next.x - 1, next.y}, direction))
        {
            moveBox(world, width, next, direction);
            if (direction.y != 0)
                moveBox(world, width, (Pos){next.x - 1, next.y}, direction);
            return true;
        }
    }
    return false;
}

/// This could be done more efficiently iteratively by "grouping" together all blocks that need to move.
luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = *((int **)args)[0];
    int height = *((int **)args)[1];
    char *moves = *((char ***)args)[2];
    int size = *((int **)args)[3];
    // Creating world for part 2
    Pos robot = (Pos){-1, -1};
    width = width * 2;
    char *world = (char *)calloc(width * 2 * height, sizeof(char));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            switch (input[i * (width / 2) + j])
            {
            case '#':
                world[INDEX(i, j * 2)] = '#';
                world[INDEX(i, j * 2 + 1)] = '#';
                break;
            case 'O':
                world[INDEX(i, j * 2)] = '[';
                world[INDEX(i, j * 2 + 1)] = ']';
                break;
            case '.':
                world[INDEX(i, j * 2)] = '.';
                world[INDEX(i, j * 2 + 1)] = '.';
                break;
            case '@':
                world[INDEX(i, j * 2)] = '.';
                world[INDEX(i, j * 2 + 1)] = '.';
                robot = (Pos){j * 2, i};
                break;
            default:
                free(world);
                printf("ERROR\n");
                return 0;
            }
        }
    }
    luint res = 0;
    if (robot.x == -1 || robot.y == -1)
        goto end;

    for (int m = 0; m < size; m++)
    {
        // get move
        Pos move = moveFromChar(moves[m]);
        // move world and robot if necessary
        if (moveWorldBis(world, width, robot, move))
            robot = (Pos){robot.x + move.x, robot.y + move.y};
    }
    res = sumGPS(world, width, height);
end:
    free(world);
    return res;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *width, int *height, char **moves, int *size)
{
    char buffer[MAX_LINE_LEN];
    buffer[0] = '\0';

    FILE *f = fopen(filename, "r");
    fpos_t start;
    fgetpos(f, &start);
    int n = 0;
    fgets(buffer, MAX_LINE_LEN, f);
    while (strlen(buffer) > 1)
    {
        n++;
        fgets(buffer, MAX_LINE_LEN, f);
    }

    fsetpos(f, &start);
    fgets(buffer, MAX_LINE_LEN, f);
    *width = strlen(buffer) - 1;
    *height = n;
    if (*width == 0 || *height == 0)
        return NULL;

    char *input = (char *)malloc(sizeof(char) * *width * *height);
    fsetpos(f, &start);
    for (int i = 0; i < *height; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        strncpy(input + i * *width, buffer, *width);
    }
    fgets(buffer, MAX_LINE_LEN, f); // empty line
    fpos_t start_bis;
    fgetpos(f, &start_bis);

    int total = 0;
    int lines = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        total += strlen(buffer) - 1;
        lines++;
    }
    fsetpos(f, &start_bis);
    if (total != 0)
        *moves = (char *)malloc(sizeof(char) * total);
    *size = total;
    if (*moves == NULL)
        return input;
    int offset = 0;
    for (int i = 0; i < lines; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        strncpy(*moves + offset, buffer, strlen(buffer) - 1);
        offset += strlen(buffer) - 1;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int width = 0, height = 0, size = 0;
    char *moves = NULL;
    char *input = readInput(argv[1], &width, &height, &moves, &size);

    void *args[4] = {&width, &height, &moves, &size};

    /* printf("%d %d %d\n", width, height, size);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%c", input[INDEX(i, j)]);
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < size; i++)
    {
        printf("%c", moves[i]);
    }
    printf("\n"); */

    run(15, part1, part2, input, (void **)&args);
    free(input);
    free(moves);
    return 0;
}