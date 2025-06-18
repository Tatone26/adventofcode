#include "intcode.h"

IntcodeComputer *init_computer(lint *input, int size)
{
    lint *memory = (lint *)malloc(sizeof(lint) * size);
    memcpy(memory, input, size * sizeof(lint));
    IntcodeComputer *comp = (IntcodeComputer *)malloc(sizeof(IntcodeComputer));
    comp->memory = memory;
    comp->memory_size = size;
    for (int i = 0; i < ADDITIONAL_MEMORY_SIZE; i++)
        comp->additional_memory[i] = NULL;
    comp->addr = 0;
    comp->input = 0;
    comp->has_input = 0;
    comp->want_input = 0;
    comp->output = 0;
    comp->has_output = 0;
    comp->finished = 0;
    comp->relative_base = 0;
    comp->test_var = 0;
    return comp;
}

void free_computer(IntcodeComputer *computer)
{
    free(computer->memory);
    for (int i = 0; i < ADDITIONAL_MEMORY_SIZE; i++)
    {
        if (!computer->additional_memory[i])
            continue;
        AdditionalMemoryNode *cur = computer->additional_memory[i];
        while (cur)
        {
            AdditionalMemoryNode *temp = cur->next;
            free(cur);
            cur = temp;
        }
    }
    free(computer);
}

void give_input(IntcodeComputer *computer, lint input)
{
    computer->input = input;
    computer->has_input = 1;
    computer->want_input = 0;
}

void reset_computer(IntcodeComputer *computer, lint *input)
{
    memcpy(computer->memory, input, computer->memory_size * sizeof(lint));
    for (int i = 0; i < ADDITIONAL_MEMORY_SIZE; i++)
    {
        if (computer->additional_memory[i] == NULL)
            continue;
        AdditionalMemoryNode *cur = computer->additional_memory[i];
        while (cur)
        {
            AdditionalMemoryNode *temp = cur->next;
            free(cur);
            cur = temp;
        }
        computer->additional_memory[i] = NULL;
    }
    computer->addr = 0;
    computer->finished = 0;
    computer->has_input = 0;
    computer->input = 0;
    computer->want_input = 0;
    computer->output = 0;
    computer->has_output = 0;
    computer->relative_base = 0;
}

#define ADDR_HASH_MASK (ADDITIONAL_MEMORY_SIZE - 1)
#define addr_hash(x) (((x >> 16) ^ x) & ADDR_HASH_MASK)

void set(IntcodeComputer *computer, int addr, lint value)
{
    if (addr < computer->memory_size)
        computer->memory[addr] = value;
    else
    {
        // printf("Setting %d (%lld)\n", addr, value);

        AdditionalMemoryNode *curr = computer->additional_memory[addr_hash(addr)];
        while (curr)
        {
            if (curr->addr == addr)
            {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }
        // not found -> add it
        AdditionalMemoryNode *new = (AdditionalMemoryNode *)malloc(sizeof(AdditionalMemoryNode));
        new->addr = addr;
        new->value = value;
        // add in front
        new->next = computer->additional_memory[addr_hash(addr)];
        computer->additional_memory[addr_hash(addr)] = new;
    }
}

lint get(IntcodeComputer *computer, int addr)
{
    if (addr < computer->memory_size)
        return computer->memory[addr];
    else
    {
        // printf("Wanting %d", addr);
        AdditionalMemoryNode *curr = computer->additional_memory[addr_hash(addr)];
        while (curr)
        {
            if (curr->addr == addr)
            {
                // printf(" ; got %lld\n", curr->value);
                return curr->value;
            }
            curr = curr->next;
        }
        // not found -> return 0 (no need to add it if it has not been modified)
        // printf(" ; got nothing\n");
        return 0;
    }
}

/// @brief Reads a file to return the list of number it contains
/// @param filename
/// @param size
/// @return
lint *read_intcode(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];
    *size = 0;

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return NULL;

    fpos_t start;
    fgetpos(f, &start);
    if (!fgets(buffer, MAX_LINE_LEN, f))
    {
        fclose(f);
        return NULL;
    }
    fsetpos(f, &start);

    for (int i = 0; i < MAX_LINE_LEN && buffer[i] != '\0' && buffer[i] != '\n'; i++)
        if (buffer[i] == ',')
            *size += 1;
    *size += 1; // last one

    lint *input = (lint *)malloc(sizeof(lint) * *size);

    char *offset = buffer;
    for (int i = 0; i < *size; i++)
    {
        input[i] = strtoll(offset, &offset, 10);
        while (*offset == ',' || *offset == '\n' || *offset == ' ')
            offset++;
        if (*offset == '\0')
            break;
    }

    fclose(f);

    return input;
}

/// @brief Runs the program for as long as possible. Stops if input needed or if finished.
/// @param computer
void run_intcode(IntcodeComputer *comp, bool blocking_output)
{
    // copying that here but really not necessary (but still, I like it)
    int addr = comp->addr;

    comp->has_output = 0;

    // while (true) + (little) safety
    while (!comp->finished)
    {
        // printf("opcode %lld (%lld)\n", get(comp, addr) % 100, get(comp, addr));

        // get the parameters, depending on mode (that is the SLOW part...)
        int test_modes = get(comp, addr) / 100;
        lint parameters[3] = {0};
        for (int i = 0; i < 3; i++)
        {
            int mode = test_modes % 10; // again, that's SLOW.
            switch (mode)
            {
            case 0: // position mode
                parameters[i] = get(comp, addr + i + 1);
                break;
            case 1: // immediate mode
                parameters[i] = addr + i + 1;
                break;
            case 2: // relative mode
                parameters[i] = comp->relative_base + get(comp, addr + i + 1);
                break;
            default:
                parameters[i] = 0; // should not be used anyway
            }
            test_modes /= 10;
        }

        // run the instruction
        // the opcode is the last two digits
        switch (get(comp, addr) % 100)
        {
        case 1: // add
            set(comp, parameters[2], get(comp, parameters[0]) + get(comp, parameters[1]));
            addr += 4;
            break;
        case 2: // multiply
            set(comp, parameters[2], get(comp, parameters[0]) * get(comp, parameters[1]));
            addr += 4;
            break;
        case 3: // input
            if (comp->has_input)
            {
                set(comp, parameters[0], comp->input);
                comp->has_input = 0;
                addr += 2;
            }
            else
            {
                comp->want_input = 1;
                goto save; // waiting for input
            }
            break;
        case 4: // output
            // printf("output from %lld : %lld\n", parameters[0], get(comp, parameters[0]));
            comp->output = get(comp, parameters[0]);
            addr += 2;
            if (blocking_output)
            {
                comp->has_output = 1;
                goto save;
            }
            break;
        case 5: // jump-if-true
            if (get(comp, parameters[0]))
                addr = get(comp, parameters[1]);
            else
                addr += 3;
            break;
        case 6: // jump-if-false
            if (!get(comp, parameters[0]))
                addr = get(comp, parameters[1]);
            else
                addr += 3;
            break;
        case 7: // less-than
            if (get(comp, parameters[0]) < get(comp, parameters[1]))
                set(comp, parameters[2], 1);
            else
                set(comp, parameters[2], 0);
            addr += 4;
            break;
        case 8: // equals
            if (get(comp, parameters[0]) == get(comp, parameters[1]))
                set(comp, parameters[2], 1);
            else
                set(comp, parameters[2], 0);
            addr += 4;
            break;
        case 9: // adjust relative base
            comp->relative_base += get(comp, parameters[0]);
            addr += 2;
            break;
        case 99: // end
            comp->finished = 1;
            break;
        default: // error
            printf("ERROR INTCODE : %llu\n", get(comp, addr));
            goto save;
        }

        /* for (int i = 0; i < size; i++)
            printf("%d,", memory[i]);
        printf("\n"); */
    }

save:
{
    comp->addr = addr;
    // may have other things to add here
}
}