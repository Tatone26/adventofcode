#include "runner.h"

typedef struct
{
    int x, y, z;
    int dx, dy, dz;
} Moon;

luint part1(void *input_v, void **args)
{
    Moon *input_ = (Moon *)input_v;
    int size = ((int *)args)[0];

    Moon *input = (Moon *)malloc(sizeof(Moon) * size);
    memcpy(input, input_, sizeof(Moon) * size);

    for (int tick = 0; tick < 1000; tick++)
    {
        // apply gravity
        for (int m = 0; m < size; m++)
        {
            for (int n = 0; n < size; n++)
            {
                if (input[m].x < input[n].x)
                    input[m].dx++;
                else if (input[m].x > input[n].x)
                    input[m].dx--;

                if (input[m].y < input[n].y)
                    input[m].dy++;
                else if (input[m].y > input[n].y)
                    input[m].dy--;

                if (input[m].z < input[n].z)
                    input[m].dz++;
                else if (input[m].z > input[n].z)
                    input[m].dz--;
            }
        }
        // move moons
        for (int m = 0; m < size; m++)
        {
            input[m].x += input[m].dx;
            input[m].y += input[m].dy;
            input[m].z += input[m].dz;
        }
    }

    int total_energy = 0;
    for (int m = 0; m < size; m++)
    {
        int pot = abs(input[m].x) + abs(input[m].y) + abs(input[m].z);
        int kin = abs(input[m].dx) + abs(input[m].dy) + abs(input[m].dz);
        total_energy += pot * kin;
    }

    free(input);

    return total_energy;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    // we can try simulating for as long as the velocity isn't 0 for everything maybe ?
    // that works in the simplest example because the repeated state is the original one. Lets hope that's always the case.

    // to simulate the universe more efficiently, wtf
    // clearly need to find some cycles and do math then but doesn't work for me for now (will come back)
    Moon *input_ = (Moon *)input_v;
    int size = ((int *)args)[0];

    Moon *input = (Moon *)malloc(sizeof(Moon) * size);
    memcpy(input, input_, sizeof(Moon) * size);

    Moon initial_state[4];
    memcpy(initial_state, input, sizeof(Moon) * 4);

    int first_reset[4] = {-1, -1, -1, -1};
    int second_reset[4] = {-1, -1, -1, -1};

    bool skip = 0;
    int first_reset_pos[4] = {-1, -1, -1, -1};
    int second_reset_pos[4] = {-1, -1, -1, -1};

    for (int tick = 0;; tick++)
    {
        // apply gravity
        for (int m = 0; m < size; m++)
        {
            for (int n = 0; n < size; n++)
            {
                if (input[m].x < input[n].x)
                    input[m].dx++;
                else if (input[m].x > input[n].x)
                    input[m].dx--;

                if (input[m].y < input[n].y)
                    input[m].dy++;
                else if (input[m].y > input[n].y)
                    input[m].dy--;

                if (input[m].z < input[n].z)
                    input[m].dz++;
                else if (input[m].z > input[n].z)
                    input[m].dz--;
            }
        }
        // move moons
        for (int m = 0; m < size; m++)
        {
            input[m].x += input[m].dx;
            input[m].y += input[m].dy;
            input[m].z += input[m].dz;

            if (!skip && !(input[0].x != initial_state[0].x || input[0].y != initial_state[0].y || input[0].z != initial_state[0].z))
            {
                if (first_reset_pos[m] == -1)
                {
                    first_reset_pos[m] = tick;
                    printf("first pos for %d at %d\n", m, tick);
                }
                else if (second_reset_pos[m] == -1)
                {
                    second_reset_pos[m] = tick;
                    printf("second pos for %d at %d\n", m, tick);
                }
            }
            else
            {
                skip = 0;
            }
            if (!(input[0].dx != initial_state[0].dx || input[0].dy != initial_state[0].dy || input[0].dz != initial_state[0].dz))
            {
                if (first_reset[m] == -1)
                {
                    first_reset[m] = tick;
                    printf("first for %d at %d\n", m, tick);
                    skip = 1;
                }
                else if (second_reset[m] == -1)
                {
                    second_reset[m] = tick;
                    printf("second for %d at %d\n", m, tick);
                    skip = 1;
                }
            }
        }
        bool stop = 1;
        for (int m = 0; stop && m < size; m++)
            if (second_reset[m] == -1 || second_reset_pos[m] == -1)
                stop = 0;
        if (stop)
            break;
    }

    for (int m = 0; m < size; m++)
    {
        printf("%d\n", second_reset[m] - first_reset[m]);
    }

    for (int m = 0; m < size; m++)
    {
        printf("%d\n", second_reset_pos[m] - first_reset_pos[m]);
    }

    int a = second_reset[1] - first_reset[1];
    int b = second_reset_pos[1] - first_reset_pos[1];
    long long u, v;
    luint pgcd = euclide(a, b, &u, &v);

    printf("%lld, pgcd %lld\n", a * u + b * v, pgcd);

    free(input);
    return 0;
}

// ----------------------------------------------------------------

Moon *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    *size = fileSize(f);
    if (*size < 4)
    {
        fclose(f);
        return NULL;
    }
    else
        *size = 4;

    Moon *input = (Moon *)malloc(sizeof(Moon) * *size);

    int moon = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f) && moon < 4)
    {
        sscanf(buffer, "<x=%d, y=%d, z=%d>", &input[moon].x, &input[moon].y, &input[moon].z);
        input[moon].dx = 0;
        input[moon].dy = 0;
        input[moon].dz = 0;
        moon++;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Moon *input = readInput(argv[1], &size);
    run(12, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}