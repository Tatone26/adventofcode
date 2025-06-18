#include "runner.h"

typedef struct
{
    unsigned long registers[3];
    int ins_pointer;
    int *instructions;
    int nb_instructions;
} ThreeBitsComputer;

void reset(ThreeBitsComputer *computer, unsigned long n)
{
    computer->ins_pointer = 0;
    computer->registers[0] = n;
    computer->registers[1] = 0;
    computer->registers[2] = 0;
}

unsigned long combo(ThreeBitsComputer *computer)
{
    int c = computer->instructions[computer->ins_pointer + 1];
    switch (c)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        return (unsigned long)c;
    case 4:
    case 5:
    case 6:
        return computer->registers[c - 4];
    case 7:
        printf("ERROR\n");
    }
    return 0;
}

bool update(ThreeBitsComputer *computer, char *out, int *offset)
{
    if (computer->ins_pointer >= computer->nb_instructions)
        return false;
    int ins = computer->instructions[computer->ins_pointer];
    int c = computer->instructions[computer->ins_pointer + 1];
    switch (ins)
    {
    case 0: // adv
        computer->registers[0] = computer->registers[0] >> combo(computer);
        computer->ins_pointer += 2;
        break;
    case 1: // bxl
        computer->registers[1] = computer->registers[1] ^ c;
        computer->ins_pointer += 2;
        break;
    case 2: // bst
        computer->registers[1] = combo(computer) % 8;
        computer->ins_pointer += 2;
        break;
    case 3: // jnz
        if (computer->registers[0] == 0)
            computer->ins_pointer += 2;
        else
            computer->ins_pointer = c;
        break;
    case 4: // bxc
        computer->registers[1] = computer->registers[1] ^ computer->registers[2];
        computer->ins_pointer += 2;
        break;
    case 5: // out
        out[(*offset)++] = (combo(computer) % 8) + '0';
        computer->ins_pointer += 2;
        break;
    case 6: // bdv
        computer->registers[1] = computer->registers[0] >> combo(computer);
        computer->ins_pointer += 2;
        break;
    case 7: // cdv
        computer->registers[2] = computer->registers[0] >> combo(computer);
        computer->ins_pointer += 2;
        break;
    default:
        printf("ERROR COMPUTING\n");
        return false;
    }
    return true;
}

luint part1(void *input_v, void **args)
{
    ThreeBitsComputer *computer = (ThreeBitsComputer *)input_v;
    int becauseTheCompilerIsMad = ((int *)args)[0];
    reset(computer, becauseTheCompilerIsMad);
    char test[100] = {0};
    int offset = 0;
    // running the computer
    while (update(computer, test, &offset))
        ;
    char answer[100] = {0};
    for (int i = 0; i < (int)strlen(test); i++)
    {
        answer[2 * i] = test[i];
        answer[2 * i + 1] = ',';
    }
    answer[strlen(answer) - 1] = '\0';
    printf("Answer Part 1 : %s\n", answer);
    return atoi(test);
}

// -----------------------------------------------------------------

// Using some input perks : >> 3 every time and loop until A = 0;
unsigned long breaking(ThreeBitsComputer *computer, unsigned long current, char *goal, int size, int offset)
{
    // end case : we can be sure that it works, and that it is the smallest possible.
    if (offset >= size)
    {
        return current;
    }
    for (int d = 0; d < 8; d++)
    {
        // run the computer onto current
        unsigned long new = current << 3 | d;
        reset(computer, new);
        char test[100] = {0};
        int o = 0;
        while (update(computer, test, &o))
            ;
        // check if the last digits produced are good, if they are we found a potential solution step
        if (!strncmp(goal + size - offset - 1, test, offset + 1))
        {
            // try to find the remaining bits of register A, by recursion
            unsigned long res = breaking(computer, new, goal, size, offset + 1);
            // if we found the solution, return it
            if (res != 0)
                return res;
        }
    }
    // we did'nt find any solutions.
    return 0;
}

luint part2(void *input_v, void **args)
{
    ThreeBitsComputer *computer = (ThreeBitsComputer *)input_v;
    int becauseTheCompilerIsMad = ((int *)args)[0];
    reset(computer, becauseTheCompilerIsMad);
    // get the string corresponding to the goal (without ',' because we don't care)
    char goal[100] = {0};
    for (int i = 0; i < computer->nb_instructions; i++)
        goal[i] = computer->instructions[i] + '0';

    unsigned long test = breaking(computer, 0, goal, computer->nb_instructions, 0);
    return test;
}

// ----------------------------------------------------------------

ThreeBitsComputer *readInput(char *filename, int *init)
{
    char buffer[MAX_LINE_LEN];
    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;

    int *instructions = NULL;
    int a, b, c;
    if (!fgets(buffer, MAX_LINE_LEN, f))
        goto clean;
    if (sscanf(buffer, "Register A: %d", &a) != 1)
        return NULL;
    if (!fgets(buffer, MAX_LINE_LEN, f))
        goto clean;
    if (sscanf(buffer, "Register B: %d", &b) != 1)
        return NULL;
    if (!fgets(buffer, MAX_LINE_LEN, f))
        goto clean;
    if (sscanf(buffer, "Register C: %d", &c) != 1)
        return NULL;

    if (!fgets(buffer, MAX_LINE_LEN, f))
        goto clean; // empty line
    if (!fgets(buffer, MAX_LINE_LEN, f))
        goto clean;
    if (!strncmp(buffer, "Program: ", 10))
        return NULL;
    int offset = 0;
    while (*(buffer + offset + 10) != '\n')
        offset += 2;
    int size = offset / 2 + 1;

    instructions = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
        instructions[i] = atoi((buffer + 9 + 2 * i));

    ThreeBitsComputer *computer = (ThreeBitsComputer *)malloc(sizeof(ThreeBitsComputer));
    *computer = (ThreeBitsComputer){{a, b, c}, 0, instructions, size};
    *init = a;
    fclose(f);
    return computer;
clean:
{
    fclose(f);
    if (instructions)
        free(instructions);
    return NULL;
}
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int init = 0;
    ThreeBitsComputer *input = readInput(argv[1], &init);
    if (input == NULL)
        return 2;
    run(17, part1, part2, input, (void **)&init);

    free(input->instructions);
    free(input);
    return 0;
}