#include "utils.h"

void run(int dayNum, int (*part1)(int count, va_list args), int (*part2)(int count, va_list args), int count, ...)
{
    printf("\n-- Day %d -- \n\n", dayNum);

    va_list list;

    clock_t startPart1 = clock();       // Timer
    va_start(list, count);              // Copying variadic arguments into list
    int res1 = (int)part1(count, list); // First part
    clock_t endPart1 = clock();

    printf("Part 1 : %d\n", res1);

    clock_t startPart2 = clock();       // Timer
    va_start(list, count);              // Copying variadic arguments into list
    int res2 = (int)part2(count, list); // Second part
    clock_t endPart2 = clock();

    printf("Part 2 : %d\n\n", res2);

    va_end(list);

    // Timing calculations
    float timePart1 = (float)(endPart1 - startPart1) / CLOCKS_PER_MS;
    float timePart2 = (float)(endPart2 - startPart2) / CLOCKS_PER_MS;

    printf("Time Part 1 : %.3fms\nTime Part 2 : %.3fms\nTotal Time : %.3fms\n\n", timePart1, timePart2, timePart1 + timePart2);
}