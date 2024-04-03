#include "utils.h"

void day1(FILE *f)
{
    int sum = 0;
    int more_sum = 0;
    char buffer[256];
    while (!feof(f))
    {
        fgets(buffer, 256, f);
        // number gives 0 if empty line.
        int number = atoi(buffer);
        resetBuffer(buffer, 256);
        if (number == 0)
            continue;

        int fuel = (number / 3) - 2;
        sum += fuel;
        while (fuel > 0)
        {
            more_sum += fuel;
            fuel = (fuel / 3) - 2;
        }
    }

    printf("Day 1 -- Part 1 : %d ; Part 2 : %d\n", sum, more_sum);
}
