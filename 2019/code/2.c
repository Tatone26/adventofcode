#include "runner.h"

#include "intcode.h"

luint part1(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    return run_intcode(input, size, 12, 2, NULL, 0);
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    for (int noun = 0; noun < 100; noun++)
        for (int verb = 0; verb < 100; verb++)
            if (run_intcode(input, size, noun, verb, NULL, 0) == 19690720)
                return 100 * noun + verb;

    printf("No result found.");
    return 0;
}

// ----------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    int *input = read_intcode(argv[1], &size);
    run(2, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}