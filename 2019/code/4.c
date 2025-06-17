#include "runner.h"

luint part1(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];
    if (size != 2)
        return -1;

    int res = 0;

    for (int x = input[0]; x <= input[1];)
    {
        // test double digits
        int test = x;
        bool has_double = 0;
        while (!has_double && test >= 10)
        {
            if ((test % 100) % 11 == 0) // are the last two digits a multiple of 11 ?
                has_double = 1;
            test = test / 10;
        }
        if (!has_double)
        {
            int last_digit = (x / 10) % 10;
            if (last_digit < x % 10)
                last_digit += 1;
            x = (x / 100) * 100 + last_digit * 10 + last_digit; // NEXT acceptable value
            continue;
        }
        // test digit order
        test = x;
        bool wrong_order = 0;
        for (int i = 0; !wrong_order && i < 5; i++)
        {
            if (test % 10 < (test / 10) % 10)
            {
                wrong_order = 1;
                // we change the wrong digit to be good
                int good_part = (test / 10) * 10 + (test / 10) % 10;
                // i is the index (in reverse order) where the order is wrong, need 10^i
                int remainder_power = 1;
                for (int l = 0; l < i; l++)
                    remainder_power *= 10;
                // and compute the "good" part with the part that was already good.
                x = good_part * remainder_power + x % remainder_power; // NEXT acceptable value
            }
            test = test / 10;
        }
        if (!wrong_order)
        {
            res += 1;
            x += 1;
        }
    }

    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];
    if (size != 2)
        return -1;

    int res = 0;

    for (int x = input[0]; x <= input[1];)
    {
        // test double digits
        int test = x;
        bool has_double = 0;
        bool has_too_much = 0;
        while (!has_double && test >= 10)
        {
            if ((test % 100) % 11 == 0) // are the last two digits a multiple of 11 ?
            {
                int digit = test % 10;
                if (((test / 100)) % 10 != digit) // is the third digit the same too ?
                    has_double = 1;               // no ? then it is exactly a double ! so okay
                else
                {
                    has_too_much = 1;
                    // it is too much... still needs to search for another digit so skip all those one
                    while (test > 0 && test % 10 == digit)
                        test = test / 10;
                }
            }
            else
                test = test / 10;
        }
        if (!has_double && !has_too_much)
        {
            int last_digit = (x / 10) % 10;
            if (last_digit < x % 10)
                last_digit += 1;
            x = (x / 100) * 100 + last_digit * 10 + last_digit; // NEXT acceptable value
            continue;
        }

        if (!has_double && has_too_much)
        {
            x += 1;
            continue;
        }

        // test digit order
        test = x;
        bool wrong_order = 0;
        for (int i = 0; !wrong_order && i < 5; i++)
        {
            if (test % 10 < (test / 10) % 10)
            {
                wrong_order = 1;
                // we change the wrong digit to be good
                int good_part = (test / 10) * 10 + (test / 10) % 10;
                // i is the index (in reverse order) where the order is wrong, need 10^i
                int remainder_power = 1;
                for (int l = 0; l < i; l++)
                    remainder_power *= 10;
                // and compute the "good" part with the part that was already good.
                x = good_part * remainder_power + x % remainder_power; // NEXT acceptable value
            }
            test = test / 10;
        }

        if (!wrong_order)
        {
            res += 1;
            x += 1;
        }
    }

    return res;
}

// ----------------------------------------------------------------

int *readInput(char *filename)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return NULL;
    if (!fgets(buffer, MAX_LINE_LEN, f))
    {
        fclose(f);
        return NULL;
    }
    int *input = (int *)malloc(sizeof(int) * 2);
    input[0] = atoi(buffer);
    input[1] = atoi(buffer + 7);
    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 2;
    int *input = readInput(argv[1]);
    run(4, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}