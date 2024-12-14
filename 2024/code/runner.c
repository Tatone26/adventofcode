#include "runner.h"

#define COLOR_OFF "\033[m"
#define BOLD "\033[1m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define UNDERLINE "\033[4m"
#define DIM "\033[2m"

/// @brief Runs a "day", given a function / part.
/// @param dayNum
/// @param part1
/// @param part2
/// @param count
/// @param parameters The parameters to pass to the functions.
void run(int dayNum, luint (*part1)(void *input, void **args), luint (*part2)(void *input, void **args), void *input, void **args)
{
    printf(BOLD GREEN "\n  -- Day %d --  " COLOR_OFF "\n\n", dayNum);

    clock_t startPart1 = clock();           // Timer
    luint res1 = (luint)part1(input, args); // First part
    clock_t endPart1 = clock();

    printf(UNDERLINE "Part 1" COLOR_OFF " : %llu\n", res1);

    clock_t startPart2 = clock();           // Timer
    luint res2 = (luint)part2(input, args); // Second part
    clock_t endPart2 = clock();

    printf(UNDERLINE "Part 2" COLOR_OFF " : %llu\n\n", res2);
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

/// @brief Counts the number of digits of the given number in base 10, by divisions
/// @param n The number
/// @return The number of digits
int nbOfDigits(luint n)
{
    if (n == 0)
        return 1;
    int res = 1;
    while (n /= 10)
        res++;
    return res;
}

/// @brief Counts the number of lines that are not empty (ignores the last one)
/// @param f an open file, set to the desired position
/// @return The number of interesting lines (size of input)
int fileSize(FILE *f)
{
    fpos_t start;
    fgetpos(f, &start);
    char buffer[MAX_LINE_LEN];
    int n = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
        n++;
    fsetpos(f, &start);
    return n;
}

/// @brief Uses euclide's algorithm to calculate the pgcd and the two Bezout's coefficient
/// @param a
/// @param b
/// @param u First Bezout coeff (for a) (CAN BE NEGATIVE)
/// @param v Second Bezout coeff (for b) (CAN BE NEGATIVE)
/// @return pgcd(a, b)
luint euclide(luint a, luint b, long long *u, long long *v)
{
    long long r1 = a, u1 = 1, v1 = 0, r2 = b, u2 = 0, v2 = 1;
    while (r2)
    {
        long long q = r1 / r2;
        long long rs = r1;
        long long us = u1;
        long long vs = v1;
        r1 = r2;
        u1 = u2;
        v1 = v2;
        r2 = rs - q * r2;
        u2 = us - q * u2;
        v2 = vs - q * v2;
    }
    *u = u1;
    *v = v1;
    return r1;
}