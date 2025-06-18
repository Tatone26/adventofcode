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
            for (int n = m; n < size; n++)
            {
                if (input[m].x < input[n].x)
                {
                    input[m].dx++;
                    input[n].dx--;
                }
                else if (input[m].x > input[n].x)
                {
                    input[m].dx--;
                    input[n].dx++;
                }

                if (input[m].y < input[n].y)
                {
                    input[m].dy++;
                    input[n].dy--;
                }
                else if (input[m].y > input[n].y)
                {
                    input[m].dy--;
                    input[n].dy++;
                }

                if (input[m].z < input[n].z)
                {
                    input[m].dz++;
                    input[n].dz--;
                }
                else if (input[m].z > input[n].z)
                {
                    input[m].dz--;
                    input[n].dz++;
                }
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

    luint x_reset = 0;
    luint y_reset = 0;
    luint z_reset = 0;

    for (int tick = 1;; tick++)
    {
        // apply gravity
        for (int m = 0; m < size; m++)
        {
            for (int n = m; n < size; n++)
            {
                if (input[m].x < input[n].x)
                {
                    input[m].dx++;
                    input[n].dx--;
                }
                else if (input[m].x > input[n].x)
                {
                    input[m].dx--;
                    input[n].dx++;
                }

                if (input[m].y < input[n].y)
                {
                    input[m].dy++;
                    input[n].dy--;
                }
                else if (input[m].y > input[n].y)
                {
                    input[m].dy--;
                    input[n].dy++;
                }

                if (input[m].z < input[n].z)
                {
                    input[m].dz++;
                    input[n].dz--;
                }
                else if (input[m].z > input[n].z)
                {
                    input[m].dz--;
                    input[n].dz++;
                }
            }
        }
        // move moons
        for (int m = 0; m < size; m++)
        {
            input[m].x += input[m].dx;
            input[m].y += input[m].dy;
            input[m].z += input[m].dz;
        }
        // check reset on each axis (they are independant)

        bool reset_x = 1;
        bool reset_y = 1;
        bool reset_z = 1;
        for (int m = 0; m < size; m++)
        {
            if (input[m].x != initial_state[m].x || input[m].dx != initial_state[m].dx)
            {
                reset_x = 0;
            }
            if (input[m].y != initial_state[m].y || input[m].dy != initial_state[m].dy)
            {
                reset_y = 0;
            }
            if (input[m].z != initial_state[m].z || input[m].dz != initial_state[m].dz)
            {
                reset_z = 0;
            }
        }
        if (!x_reset && reset_x)
            x_reset = tick;
        if (!y_reset && reset_y)
            y_reset = tick;
        if (!z_reset && reset_z)
            z_reset = tick;

        if (x_reset && y_reset && z_reset)
            break;
    }

    luint ppcm_x_y = (x_reset * y_reset) / pgcd(x_reset, y_reset);
    luint res = (ppcm_x_y * z_reset) / pgcd(ppcm_x_y, z_reset);

    free(input);
    return res;
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