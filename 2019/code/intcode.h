#ifndef INTCODE_H
#define INTCODE_H

#include <stdio.h>
#include "runner.h"

typedef struct
{
    int *memory;
    int memory_size;
    int addr;
    int input;
    bool has_input; // true if the computer can read an input
    int output;     // last recorder output value
    bool finished;  // true if finished running
} IntcodeComputer;

IntcodeComputer *init_computer(int *input, int size);
void free_computer(IntcodeComputer *computer);
void give_input(IntcodeComputer *computer, int input);
void reset_computer(IntcodeComputer *computer, int *input);

int *read_intcode(char *filename, int *size);
void run_intcode(IntcodeComputer *computer);

#endif