#include "runner.h"
#include "intcode.h"

luint part1(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);
    give_input(comp, 1);
    run_intcode(comp);
    int res = comp->output;
    free_computer(comp);
    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);
    give_input(comp, 5);
    run_intcode(comp);
    int res = comp->output;
    free_computer(comp);
    return res;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    *size = fileSize(f);
    char *input = (char *)malloc(sizeof(char) * *size);

    int offset = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        strncpy(input + offset, buffer, strlen(buffer) - 1); // -1 to remove \n
        offset += strlen(buffer) - 1;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    int *input = read_intcode(argv[1], &size);
    run(5, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}