#ifndef RUNNER_H
#define RUNNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

/// @brief Max characters to read in a file line. Can be increased.
#define MAX_LINE_LEN 20000

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)

typedef struct
{
    int x;
    int y;
} Pos;

#define INDEX(y, x) ((y) * width + (x))
#define INDEX_POS(pos) (INDEX(pos.y, pos.x))

#define INBOUNDS(pos, width, height) (pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height)

typedef unsigned long long luint;

/// @brief Runs a given adventofcode.com problem.
/// @param dayNum The number of the day to run.
/// @param part1 The function for the first part. Must return the result as an int, and takes a certain amount of arguments [count] and a va_list containing those arguments.
/// @param part2 The function for the second part. Works like the first one, and takes the same arguments.
/// @param count The number of arguments to give to both functions.
/// @param all The arguments to give to both functions.
void run(int dayNum, luint (*part1)(void *input, void **args), luint (*part2)(void *input, void **args), void *input, void **args);

int nbOfDigits(luint n);

int fileSize(FILE *f);

#endif