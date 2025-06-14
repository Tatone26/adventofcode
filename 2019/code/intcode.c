#include "intcode.h"

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

/// @brief Runs the given program, copies the input
/// @param input the memory representation
/// @param size the number of instruction
/// @param arg1 the value to put at address 1 (cf day2), -1 if not necessary
/// @param arg2 the value to put at address 2 (cf day2), -1 if not necessary
/// @param in the value of the input (cf day5)
/// @param res what to output : -1 for output, else memory[res]
/// @return the result of the program
int run_intcode(int *input, int size, int arg1, int arg2, int *in, int res)
{
    int *memory = (int *)malloc(sizeof(int) * size);
    memcpy(memory, input, size * sizeof(int));

    if (arg1 != -1)
        memory[1] = arg1;
    if (arg2 != -1)
        memory[2] = arg2;

    /* for (int i = 0; i < size; i++)
        printf("%d,", memory[i]);
    printf("\n"); */

    int addr = 0; // code pointer

    int in_n = 0;   // the next input
    int output = 0; // value of the output
    int tick = 0;   // just to know how many cycles it took

    int last_output = 0; // value of the output, one tick before (it is the one returned and check for real)
    bool finished = 0;   // true if need to exit
    // while (true) + safety
    while (addr < size)
    {
        // get the parameters, depending on mode
        int test_modes = memory[addr] / 100;
        int parameters[10] = {0};
        for (int i = 0; addr + i < size && i < 10; i++)
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
            memory[parameters[0]] = in[in_n];
            in_n++;
            addr += 2;
            break;
        case 4: // output
            output = memory[parameters[0]];
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
            finished = 1;
            break;
        default:
            // printf("ERROR INTCODE\n");
            goto bad_ending;
        }

        /* for (int i = 0; i < size; i++)
            printf("%d,", memory[i]);
        printf("\n"); */

        if (finished)
        // good ending
        {
            int temp;
            if (res != -1)
                temp = memory[res];
            else
                temp = last_output;
            free(memory);
            return temp;
        }
        // if output != 0 not followed by ending, error
        if (last_output != 0 && !finished)
        {
            printf("ERROR : at tick %d\n", tick);
            goto bad_ending;
        }
        // save output, update tick etc etc
        last_output = output;
        output = 0;
        tick++;
    }
// just to make sure the memory is clean, does nothing really.
bad_ending:
{
    free(memory);
    return 0;
}
}