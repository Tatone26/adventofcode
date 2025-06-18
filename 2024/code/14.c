#include "runner.h"

#define HEIGHT 103
#define WIDTH 101

typedef struct
{
    Pos position;
    Pos velocity;
} Robot;

int quadrant(Pos position)
{
    if (position.x < WIDTH / 2)
    {
        if (position.y < HEIGHT / 2)
            return 0;
        else if (position.y > HEIGHT / 2)
            return 1;
    }
    else if (position.x > WIDTH / 2)
    {
        if (position.y < HEIGHT / 2)
            return 2;
        else if (position.y > HEIGHT / 2)
            return 3;
    }
    return 4;
}

luint part1(void *input_v, void **args)
{
    Robot *input = (Robot *)input_v;
    int size = ((int *)args)[0];

    int results[5] = {0, 0, 0, 0, 0};

    for (int r = 0; r < size; r++)
    {
        Pos after100Turns = (Pos){(input[r].position.x + 100 * input[r].velocity.x) % WIDTH, (input[r].position.y + 100 * input[r].velocity.y) % HEIGHT};
        if (after100Turns.x < 0)
            after100Turns.x += WIDTH;
        if (after100Turns.y < 0)
            after100Turns.y += HEIGHT;
        (results[quadrant(after100Turns)])++;
    }
    return results[0] * results[1] * results[2] * results[3];
}

// -----------------------------------------------------------------

void display(Robot *robots, int size)
{
    int test[WIDTH * HEIGHT] = {0};
    int c = 0;
    for (int j = 0; c < size && j < WIDTH; j++)
        for (int i = 0; c < size && i < HEIGHT; i++)
            for (int x = 0; c < size && x < size; x++)
            {
                if (robots[x].position.x == i && robots[x].position.y == j)
                {
                    (test[i * WIDTH + j])++;
                    c++;
                }
            }

    for (int j = 0; j < WIDTH; j++)
    {
        for (int i = 0; i < HEIGHT; i++)
        {
            if (test[i * WIDTH + j] == 0)
                printf(" ");
            else
                printf("%d", test[i * WIDTH + j]);
        }
        printf("\n");
    }
}

// Strange repartition in quadrants probably means that the Tree has been found -> looking for a quadrant with > 30% of robots seems to work to find patterns.
// This strategy seems to work pretty well with the given inputs.
bool suspect(Robot *robots, int size, int *quadOne, int *quadTwo)
{
    int quad[] = {0, 0, 0, 0, 0};
    int limit = size / 3;
    *quadOne = 5;
    *quadTwo = 5;
    bool first = true;
    for (int r = 0; r < size; r++)
    {
        int q = quadrant(robots[r].position);
        if ((++quad[q]) > limit && q < 4 && q != *quadOne)
        {
            if (first)
            {
                *quadOne = q;
                first = false;
            }
            else
            {
                *quadTwo = q;
                return true;
            }
        }
    }
    return false;
}

luint part2(void *input_v, void **args)
{
    Robot *input = (Robot *)input_v;
    int size = ((int *)args)[0];

    int first = -1, firstQuadOne = -1, firstQuadTwo = -1, period = -1;
    int second = -1;
    bool possibleJump = false;
    for (int i = 0; i < 20000; i++) // security, start at
    {
        // test for potential pattern found
        int a, b;
        if (suspect(input, size, &a, &b))
        {
            if (first == -1)
            {
                // if we find a first pattern, keep it.
                first = i;
                firstQuadOne = a;
                firstQuadTwo = b;
            }
            else
            {
                // if we find the second pattern, keep it
                if (second == -1 && (a != firstQuadOne || b != firstQuadTwo))
                {
                    second = i;
                    possibleJump = true;
                }
                // if we found the period for the first pattern before finding the second one (doesn't happen in the input, because of the sizes too)
                else if (a == firstQuadOne && b == firstQuadTwo)
                    period = i - first;
            }
        }
        // if both patterns have been found, jump to next possible pattern for the first one
        if (possibleJump)
        {
            int jump = (HEIGHT < WIDTH ? HEIGHT : WIDTH) - i + first - 1;
            if (jump > 0)
            {
                for (int r = 0; r < size; r++)
                {
                    Pos nextTurn = (Pos){(input[r].position.x + jump * input[r].velocity.x) % WIDTH, (input[r].position.y + jump * input[r].velocity.y) % HEIGHT};
                    if (nextTurn.x < 0)
                        nextTurn.x += WIDTH;
                    if (nextTurn.y < 0)
                        nextTurn.y += HEIGHT;
                    input[r].position = nextTurn;
                }
            }
            possibleJump = false;
            i += jump;
        }
        // if we found the period for the first one, we can deduce the period for the second one and the first time they will meet
        if (period != -1)
        {
            if (period != WIDTH && period != HEIGHT)
                printf("WRONG : %d\n", period);
            int secondPeriod = (period == WIDTH ? HEIGHT : WIDTH);
            luint bigModulo = WIDTH * HEIGHT; // use ppcm here, but we consider HEIGHT and WIDTH to be prime relative to each other
            long long u, v;
            euclide(period, secondPeriod, &u, &v);                        // get Bezout's identity
            long long x = period * u * second + secondPeriod * v * first; // Find a solution
            while (x < 0)                                                 // get the first positive answer
                x += bigModulo;
            return x;
        }

        // update position
        for (int r = 0; r < size; r++)
        {
            Pos nextTurn = (Pos){(input[r].position.x + 1 * input[r].velocity.x) % WIDTH, (input[r].position.y + 1 * input[r].velocity.y) % HEIGHT};
            if (nextTurn.x < 0)
                nextTurn.x += WIDTH;
            if (nextTurn.y < 0)
                nextTurn.y += HEIGHT;
            input[r].position = nextTurn;
        }
    }
    return 0;
}

// ----------------------------------------------------------------

Robot *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;
    *size = fileSize(f);
    Robot *input = (Robot *)malloc(sizeof(Robot) * *size);

    for (int i = 0; i < *size; i++)
    {
        if (!fgets(buffer, MAX_LINE_LEN, f))
        {
            fclose(f);
            free(input);
            return NULL;
        }
        int a, b, c, d;
        if (sscanf(buffer, "p=%d,%d v=%d,%d", &a, &b, &c, &d) != 4)
            printf("WARNING INPUT READING\n");
        input[i] = (Robot){(Pos){a, b}, (Pos){c, d}};
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Robot *input = readInput(argv[1], &size);
    run(14, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}