#include "runner.h"
#include <math.h>

typedef struct _range
{
    luint start;
    luint end;
} Range;

/**
 * Not sure I can really explain what I did, but it does work pretty well ?
 */
luint part1(void *input_v, void **args)
{
    Range *input = (Range *)input_v;
    int size = ((int *)args)[0];

    luint count = 0;

    for (int i = 0; i < size; i++)
    {
        luint start = input[i].start;
        luint start_length = nbOfDigits(start);

        luint end = input[i].end;
        luint end_length = nbOfDigits(end);

        // odd-sized numbers aren't interesting anyway, so we can cut the range.
        // it is also absolutely necessary for the optimised algorithm I use...

        if (start_length % 2 == 1)
        {
            start = fast_pow(10, start_length);
            start_length++; // we will reuse that
        }

        if (end_length % 2 == 1) // that is technically not necessary but good to have anyway.
        {
            end = fast_pow(10, end_length - 1);
            // end--; // if it was odd, the last even number is 9999...9 (but that doesn't change anything)
        }

        // div is the number to use to have first and second part (modulo and division)
        int div = (int)fast_pow(10, start_length / 2);

        luint current = start;
        while (current <= end)
        {
            if (current % div == current / div)
            {
                count += current;
                // if first part == second part, we know we can skip to the next 'first part' digit
                current += fast_pow(10, start_length / 2);
            }
            current++;
        }
    }
    return count;
}

// -----------------------------------------------------------------

/**
 * Pretty frustated because I feel like I missed something to cut the number of numbers to test.
 */
luint part2(void *input_v, void **args)
{

    Range *input = (Range *)input_v;
    int size = ((int *)args)[0];

    luint count = 0;

    for (int i = 0; i < size; i++)
    {
        luint start = input[i].start;
        luint end = input[i].end;

        // couldn't find any condition to cut the range directly without testing.

        luint current = start;

        int digits = nbOfDigits(current);
        int nb_of_digits_can_change = (digits != nbOfDigits(end));

        while (current <= end)
        {
            // we never need to try more than half the (max) digits as sequence size
            int max_size = digits / 2;

            luint div = 1; // used as mask to get d digits
            // The absolute optimisation would be to find a better way to take factors of digits...
            for (int d = 1; d <= max_size; d++)
            {
                div *= 10;
                if (digits % d != 0) // can't use sequences of size that doesn't divide the total number of digits
                    continue;

                luint val = current % div; // first d digits

                luint temp = current / div;
                int is_valid = 1;
                while (temp)
                {
                    if (temp % div != val)
                    { // this part of sequence is different than the next one => invalid
                        is_valid = 0;
                        break;
                    }
                    temp /= div;
                }
                if (is_valid)
                {
                    // valid sequence size => valid ID
                    count += current;

                    // I thought I could skip ahead like in first part but...
                    // I couldn't manage to do so by more than 2 digits (+11)
                    // there may be a way, but it would probably require more conditions and tests.
                    // ex that break with += div:
                    //    978978 (d = 3) - 979797 (d = 2) - 979979 (d = 3) => +div would skip 979797
                    current += 10;

                    break; // VERY important break
                }
            }

            current++;
            if (nb_of_digits_can_change && current % 10 == 0) // allows limiting the number of calls to nbOfDigits. %10 works for every number, and I'm not bothered to do better as it is not the bottleneck anymore :)
                digits = nbOfDigits(current);
        }
    }
    return count;
}

// ----------------------------------------------------------------

Range *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);

    fgets(buffer, MAX_LINE_LEN, f);
    *size = 1;
    int temp = strlen(buffer);
    for (int i = 0; i < temp; i++)
        if (buffer[i] == ',')
            (*size)++;
    Range *input = (Range *)malloc(sizeof(Range) * (*size)); // WARNING : change size of alloc

    int offset = 0;
    for (int i = 0; i < *size; i++)
    {
        luint start, end;
        sscanf(buffer + offset, "%llu-%llu,", &start, &end);
        input[i].end = end;
        input[i].start = start;
        offset += nbOfDigits(input[i].end) + nbOfDigits(input[i].start) + 2;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Range *input = readInput(argv[1], &size);
    run(2, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}