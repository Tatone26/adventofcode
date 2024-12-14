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

typedef int Quadrants[5];

void calculateQuadrants(Robot *input, int size, Quadrants res)
{
    for (int i = 0; i < 5; i++)
        res[i] = 0;
    for (int r = 0; r < size; r++)
        (res[quadrant(input[r].position)])++;
}
// Strange repartition in quadrants probably means that the Tree has been found -> looking for a quadrant with > 30% of robots seems to work to find patterns.
bool suspect(Robot *robots, int size, int *quadOne, int *quadTwo)
{
    Quadrants quad;
    calculateQuadrants(robots, size, quad);
    bool first = true;
    for (int i = 0; i < 4; i++)
    {
        if (quad[i] > size / 3)
        {
            if (first)
            {
                *quadOne = i;
                first = false;
            }
            else
            {
                *quadTwo = i;
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

    int first = -1, firstQuadOne = -1, firstQuadTwo = -1;
    int second = -1;
    for (int i = 0; i < 20000; i++) // security, start at
    {
        // test for potential pattern found
        int a, b;
        if (suspect(input, size, &a, &b))
        {
            if (first == -1)
            {
                // if we find a first pattern, keep it.
                {
                    first = i;
                    firstQuadOne = a;
                    firstQuadTwo = b;
                }
            }
            else
            {
                // if we find the second pattern, keep it
                if (second == -1 && (a != firstQuadOne || b != firstQuadTwo))
                    second = i;
                // that means we found the first AGAIN, so we have its period and we can find the answer !
                else if (a == firstQuadOne && b == firstQuadTwo)
                {
                    int firstModulo = i - first;
                    if (firstModulo != WIDTH && firstModulo != HEIGHT)
                        printf("WRONG : %d\n", firstModulo);
                    int secondModulo = (firstModulo == WIDTH ? HEIGHT : WIDTH);
                    // we know those two modulos are 101 and 103, which are primes relative to one another
                    // so we can use the theorem directly to find the answer : (modulo 101 * 103)
                    luint bigModulo = WIDTH * HEIGHT;
                    long long u, v;
                    euclide(firstModulo, secondModulo, &u, &v);
                    long long x = firstModulo * u * second + secondModulo * v * first;
                    while (x < 0)
                        x += bigModulo;
                    return x;
                }
            }
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
    *size = fileSize(f);
    Robot *input = (Robot *)malloc(sizeof(Robot) * *size);

    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
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
    run(1, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}