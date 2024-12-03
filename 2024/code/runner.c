#include "runner.h"

#define COLOR_OFF "\033[m"
#define BOLD "\033[1m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define UNDERLINE "\033[4m"
#define DIM "\033[2m"

void run(int dayNum, long (*part1)(int count, va_list args), long (*part2)(int count, va_list args), int count, ...)
{
    printf(BOLD GREEN "\n  -- Day %d --  " COLOR_OFF "\n\n", dayNum);

    va_list list;

    clock_t startPart1 = clock();         // Timer
    va_start(list, count);                // Copying variadic arguments into list
    long res1 = (long)part1(count, list); // First part
    clock_t endPart1 = clock();

    printf(UNDERLINE "Part 1" COLOR_OFF " : %ld\n", res1);

    clock_t startPart2 = clock();        // Timer
    va_start(list, count);               // Copying variadic arguments into list
    long res2 = (int)part2(count, list); // Second part
    clock_t endPart2 = clock();

    printf(UNDERLINE "Part 2" COLOR_OFF " : %ld\n\n", res2);

    va_end(list);

    // Timing calculations
    float timePart1 = (float)(endPart1 - startPart1) / CLOCKS_PER_MS;
    float timePart2 = (float)(endPart2 - startPart2) / CLOCKS_PER_MS;
    // Pretty printing time
    const char *colorPart1 = (timePart1 <= 25.0 ? GREEN : RED);
    const char *colorPart2 = (timePart2 <= 50.0 ? GREEN : RED);
    const char *colorTotal = (timePart1 + timePart2 <= 80.0 ? GREEN : RED);
    printf(DIM "Part 1 :%s %.3fms\n" COLOR_OFF, colorPart1, timePart1);
    printf(DIM "Part 2 :%s %.3fms\n" COLOR_OFF, colorPart2, timePart2);
    printf(DIM "Total Time :%s %.3fms\n" COLOR_OFF, colorTotal, timePart1 + timePart2);
}