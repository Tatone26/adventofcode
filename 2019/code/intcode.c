#include "intcode.h"

IntcodeComputer *init_computer(int *input, int size)
{
    int *memory = (int *)malloc(sizeof(int) * size);
    memcpy(memory, input, size * sizeof(int));
    IntcodeComputer *comp = (IntcodeComputer *)malloc(sizeof(IntcodeComputer));
    comp->memory = memory;
    comp->memory_size = size;
    comp->addr = 0;
    comp->input = 0;
    comp->has_input = 0;
    comp->output = 0;
    comp->finished = 0;
    return comp;
}

void free_computer(IntcodeComputer *computer)
{
    free(computer->memory);
    free(computer);
}

void give_input(IntcodeComputer *computer, int input)
{
    computer->input = input;
    computer->has_input = 1;
}

void reset_computer(IntcodeComputer *computer, int *input)
{
    memcpy(computer->memory, input, computer->memory_size * sizeof(int));
    computer->addr = 0;
    computer->finished = 0;
    computer->has_input = 0;
    computer->input = 0;
    computer->output = 0;
}

/// @brief Reads a file to return the list of number it contains
/// @param filename
/// @param size
/// @return
int *read_intcode(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];
    *size = 0;

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;

    fpos_t start;
    fgetpos(f, &start);
    fgets(buffer, MAX_LINE_LEN, f);
    fsetpos(f, &start);

    for (int i = 0; i < MAX_LINE_LEN && buffer[i] != '\0' && buffer[i] != '\n'; i++)
        if (buffer[i] == ',')
            *size += 1;
    *size += 1; // last one

    int *input = (int *)malloc(sizeof(int) * *size);

    int offset = 0;
    int start_offset = 0;
    for (int i = 0; i < *size; i++)
    {
        while (buffer[offset] != '\n' && buffer[offset] != '\0' && buffer[offset] != ',')
            offset++;
        char number[100];
        strncpy(number, buffer + start_offset, offset - start_offset);
        number[offset - start_offset] = '\0';
        input[i] = atoi(number);
        offset++;
        start_offset = offset;
    }

    fclose(f);

    return input;
}

/// @brief Runs the program for as long as possible. Stops if input needed or if finished.
/// @param computer
void run_intcode(IntcodeComputer *computer)
{
    /* for (int i = 0; i < size; i++)
        printf("%d,", memory[i]);
    printf("\n"); */

    // copying that here but really not necessary (but still, I like it)
    int *memory = computer->memory;
    int memory_size = computer->memory_size;
    int addr = computer->addr;

    // while (true) + (little) safety
    while (addr < memory_size)
    {
        // get the parameters, depending on mode (that is the SLOW part...)
        int test_modes = memory[addr] / 100;
        int parameters[10] = {0};
        for (int i = 0; addr + i + 1 < memory_size && i < 10; i++)
        {
            int mode = test_modes % 10;
            if (mode == 1)
                parameters[i] = addr + i + 1;
            else if (mode == 0)
                parameters[i] = memory[addr + i + 1];
            test_modes /= 10;
        }

        // run the instruction
        // the opcode is the last two digits
        switch (memory[addr] % 100)
        {
        case 1: // add
            memory[parameters[2]] = memory[parameters[0]] + memory[parameters[1]];
            addr += 4;
            break;
        case 2: // multiply
            memory[parameters[2]] = memory[parameters[0]] * memory[parameters[1]];
            addr += 4;
            break;
        case 3: // input
            if (computer->has_input)
            {
                memory[parameters[0]] = computer->input;
                computer->has_input = 0;
                addr += 2;
            }
            else
                goto save; // waiting for input
            break;
        case 4: // output
            computer->output = memory[parameters[0]];
            addr += 2;
            break;
        case 5: // jump-if-true
            if (memory[parameters[0]])
                addr = memory[parameters[1]];
            else
                addr += 3;
            break;
        case 6: // jump-if-false
            if (!memory[parameters[0]])
                addr = memory[parameters[1]];
            else
                addr += 3;
            break;
        case 7: // less-than
            if (memory[parameters[0]] < memory[parameters[1]])
                memory[parameters[2]] = 1;
            else
                memory[parameters[2]] = 0;
            addr += 4;
            break;
        case 8: // equals
            if (memory[parameters[0]] == memory[parameters[1]])
                memory[parameters[2]] = 1;
            else
                memory[parameters[2]] = 0;
            addr += 4;
            break;
        case 99: // end
            computer->finished = 1;
            goto save;
        default:
            printf("ERROR INTCODE : %d\n", memory[addr]);
            goto save;
        }

        /* for (int i = 0; i < size; i++)
            printf("%d,", memory[i]);
        printf("\n"); */
    }

save:
{
    computer->addr = addr;
    // may have other things to add here
}
}