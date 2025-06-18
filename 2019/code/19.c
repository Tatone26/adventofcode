#include "runner.h"
#include "intcode.h"

#define WIDTH 50
#define HEIGHT 50

int get_beam(IntcodeComputer *comp, lint *input, int x, int y)
{
    reset_computer(comp, input);
    run_intcode(comp, 1);
    if (comp->want_input)
        give_input(comp, x);
    run_intcode(comp, 1);
    if (comp->want_input)
        give_input(comp, y);
    run_intcode(comp, 1);
    if (comp->has_output)
        return comp->output;
    return 0;
}

luint part1(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    luint res = 0;
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            int out = get_beam(comp, input, x, y);
            res += out;
            // printf("%c", out ? '#' : '.');
        }
        // printf("\n");
    }
    free_computer(comp);
    return res;
}

// -----------------------------------------------------------------

// NOT OPTIMIZED AT ALL, WILL NEED TO MEASURE THE X AND Y GROWTH RATE TO SKIP AHEAD
luint part2(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    luint res = 0;
    for (int y = 100;; y++)
    {
        int start_x = 0;
        int end_x = 0;
        for (int x = 0;; x++)
        {
            int out = get_beam(comp, input, x, y);
            if (!start_x && out == 1)
                start_x = x;
            else if (start_x && !end_x && out == 0)
            {
                end_x = x;
                break;
            }
        }
        if (end_x - start_x < 100)
        {
            y += 100 - (end_x - start_x); // skipping some lines because we can
            continue;
        }
        // else potential fit (it will always start at the right of the beam because the beam is linear)
        int pot_x = end_x - 100;
        int count_height = 1;
        for (int y_bis = y + 1; count_height < 100; y_bis++)
        {
            int out = get_beam(comp, input, pot_x, y_bis);
            if (!out)
                break;
            count_height++;
        }
        if (count_height == 100)
        {
            // found it
            res = pot_x * 10000 + y;
            break;
        }
        else // skipping some lines because flatter thant 45° so thinking 45° is okay
            y += 100 - count_height - 1;
    }
    free_computer(comp);
    return res;
}

// ----------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    lint *input = read_intcode(argv[1], &size);
    run(19, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}