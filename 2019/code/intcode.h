#ifndef INTCODE_H
#define INTCODE_H

#include <stdio.h>
#include "runner.h"

typedef struct _additional_memory
{
    int addr;
    lint value;
    struct _additional_memory *next;
} AdditionalMemoryNode;

#define ADDITIONAL_MEMORY_SIZE 64
typedef AdditionalMemoryNode *AdditionalMemory[ADDITIONAL_MEMORY_SIZE];

typedef struct
{
    lint *memory;
    int memory_size;
    AdditionalMemory additional_memory; // for whatever goes beyond the scope of the input, chained list

    int addr;
    lint input;
    bool has_input; // true if the computer can read an input
    bool want_input;
    lint output; // last recorder output value
    bool has_output;
    bool finished; // true if finished running
    int relative_base;

    int test_var;
} IntcodeComputer;

IntcodeComputer *init_computer(lint *input, int size);
void free_computer(IntcodeComputer *computer);
void give_input(IntcodeComputer *computer, lint input);
void reset_computer(IntcodeComputer *computer, lint *input);
lint get(IntcodeComputer *computer, int addr);
void set(IntcodeComputer *computer, int addr, lint value);

lint *read_intcode(char *filename, int *size);
void run_intcode(IntcodeComputer *computer, bool blocking_output);

#endif