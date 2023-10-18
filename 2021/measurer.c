#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/// Convert seconds to milliseconds
#define SEC_TO_MS(sec) ((sec) * 1000)
/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec) * 1000000)
/// Convert seconds to nanoseconds
#define SEC_TO_NS(sec) ((sec) * 1000000000)

/// Convert nanoseconds to seconds
#define NS_TO_SEC(ns) ((ns) / 1000000000)
/// Convert nanoseconds to milliseconds
#define NS_TO_MS(ns) ((ns) / 1000000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns) ((ns) / 1000)

unsigned long int millis()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    unsigned long int ms = SEC_TO_MS((unsigned long int)ts.tv_sec) + NS_TO_MS((unsigned long int)ts.tv_nsec);
    return ms;
}

int main()
{

    unsigned long int start, end;

    start = millis();

    int nbOfPrograms = 11;
    char programs[11][10] = {"./day1", "./day2", "./day3", "./day4", "./day5", "./day6", "./day7", "./day8", "./day9", "./day10", "./day11"};

    for (int i = 0; i < nbOfPrograms; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            static char *argv[] = {""};
            execv(programs[i], argv);
        }
        else
        {
            wait(NULL);
        }
    }

    end = millis();

    double diff = (double)(end - start) / 1000;

    printf("It took %.4f seconds (real time, and not precise at all) to compute every problems done so far !\n\n", diff);
    return 0;
}