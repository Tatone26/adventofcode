#ifndef INTCODE_H
#define INTCODE_H

#include <stdio.h>
#include "runner.h"

int *read_intcode(char *filename, int *size);
int run_intcode(int *input, int size, int arg1, int arg2, int *in, int res);

#endif