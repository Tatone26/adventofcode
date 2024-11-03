#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/// @brief Max characters to read in a file line. Can be increased.
#define MAX_LINE_LEN 10000

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)

/// @brief Runs a given adventofcode.com problem.
/// @param dayNum The number of the day to run.
/// @param part1 The function for the first part. Must return the result as an int, and takes a certain amount of arguments [count] and a va_list containing those arguments.
/// @param part2 The function for the second part. Works like the first one, and takes the same arguments.
/// @param count The number of arguments to give to both functions.
/// @param all The arguments to give to both functions.
void run(int dayNum, int (*part1)(int count, va_list args), int (*part2)(int count, va_list args), int count, ...);

#endif