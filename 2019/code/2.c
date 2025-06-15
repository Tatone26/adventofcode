#include "runner.h"

#include "intcode.h"

luint part1(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);
    set(comp, 1, 12);
    set(comp, 2, 2);
    run_intcode(comp, false);
    luint res = comp->memory[0];
    free_computer(comp);
    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    for (int noun = 0; noun < 100; noun++)
        for (int verb = 0; verb < 100; verb++)
        {
            reset_computer(comp, input);
            set(comp, 1, noun);
            set(comp, 2, verb);
            run_intcode(comp, false);
            if (comp->memory[0] == 19690720)
            {
                free_computer(comp);
                return (luint)(100 * noun + verb);
            }
        }

    printf("No result found.");
    return 0;
}

// ----------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    lint *input = read_intcode(argv[1], &size);
    run(2, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}