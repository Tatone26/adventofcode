#include "runner.h"
#include "intcode.h"

luint part1(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);
    give_input(comp, 1);
    run_intcode(comp, false);
    luint res = comp->output;
    free_computer(comp);
    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);
    give_input(comp, 2);
    run_intcode(comp, false);
    luint res = comp->output;
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
    run(9, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}