#include "runner.h"

int test_line(int *input)
{
    // safety net
    if (input[0] == 0 || input[1] == 0)
    {
        return 0;
    }
    // ascending or descending
    int ascending = input[1] - input[0];
    if (ascending < 0)
        ascending = 0;

    int previous_number = input[0];
    for (int j = 1; input[j] != 0; j++)
    {
        int diff = input[j] - previous_number;
        if ((abs(diff) == 0) || (abs(diff) > 3) ||
            (ascending && diff < 0) || (!ascending && diff > 0))
        {
            return 0;
        }
        previous_number = input[j];
    }
    return 1;
}

long part1(int count, va_list args)
{
    if (count != 2)
    {
        printf("ERROR WITH ARGUMENTS\n");
        return -1;
    }

    int nb_reports = va_arg(args, int);
    int **input = va_arg(args, int **);

    int count_safe = 0;

    for (int i = 0; i < nb_reports; i++)
    {
        count_safe += test_line(input[i]);
    }

    return count_safe;
}

// -----------------------------------------------------------------

long part2(int count, va_list args)
{
    if (count != 2)
    {
        printf("ERROR WITH ARGUMENTS\n");
        return -1;
    }
    int nb_reports = va_arg(args, int);
    int **input = va_arg(args, int **);

    int count_safe = 0;

    // On all reports (line of input)
    for (int i = 0; i < nb_reports; i++)
    {
        // get size of report
        int size;
        for (size = 0; input[i][size] != 0; size++)
            ;
        size++;
        // copy report
        int *copy = (int *)malloc(size * sizeof(int));
        int skip = -1;
        while (input[i][++skip] != 0)
        {
            int copy_index = 0;
            for (int j = 0; j < size; j++)
            {
                // skip one value when copying the array
                if (j == skip)
                    continue;
                copy[copy_index++] = input[i][j];
            }
            // test report
            if (test_line(copy))
            {
                count_safe++;
                break;
            }
        }
        free(copy);
    }

    return count_safe;
}

// ----------------------------------------------------------------

void readInput(char *filename, int *nb_reports, int ***reports)
{
    char buffer[MAX_LINE_LEN];
    buffer[0] = 'a';

    FILE *f = fopen(filename, "r");
    fpos_t start;
    fgetpos(f, &start);

    *nb_reports = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
        (*nb_reports)++;

    if (*nb_reports == 0)
        return;

    fsetpos(f, &start);

    *reports = (int **)malloc(sizeof(int *) * *nb_reports);

    int temp[8];
    for (int i = 0; i < *nb_reports; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        int count = sscanf(buffer, "%d %d %d %d %d %d %d %d", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
        (*reports)[i] = (int *)malloc(sizeof(int) * (count + 1));
        for (int j = 0; j < count; j++)
        {
            (*reports)[i][j] = temp[j];
        }
        (*reports)[i][count] = 0; // I had a "0" at the end to know the end of the array (like a string)
    }

    fclose(f);
}

int main()
{
    int nb_reports = 0;
    int **reports = 0;
    readInput("input/2.txt", &nb_reports, &reports);
    if (nb_reports == 0)
        return 1;

    run(2, part1, part2, 2, nb_reports, reports);

    for (int r = 0; r < nb_reports; r++)
        free(reports[r]);
    free(reports);
    return 0;
}