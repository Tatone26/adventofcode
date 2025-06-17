#include "runner.h"

luint part1(void *input_v, void **args)
{
    short *input = (short *)input_v;
    int size = ((int *)args)[0];

    short *current = (short *)malloc(sizeof(short) * size); // one is enough, because to find new digit k, all first k - 1 digits will be x0.
    memcpy(current, input, size * sizeof(short));

    for (int _ = 0; _ < 100; _++)
    {
        for (int i = 0; i < size; i++)
        {
            int period = i + 1;
            int start_ones = i;
            int start_minus = i + 2 * period;
            int new_digit = 0;
            while (start_ones < size) // all "1" ranges
            {
                int end = start_ones + period;
                if (end > size)
                    end = size;
                for (int x = start_ones; x < end; x++)
                    new_digit += current[x];
                start_ones += 4 * period;
            }
            while (start_minus < size) // all "-1" ranges
            {
                int end = start_minus + period;
                if (end > size)
                    end = size;
                for (int x = start_minus; x < end; x++)
                    new_digit -= current[x];
                start_minus += 4 * period;
            }
            current[i] = (new_digit < 0 ? -new_digit : new_digit) % 10;
        }
    }

    luint res = 0;
    for (int i = 0; i < 8; i++)
        res = res * 10 + current[i];

    free(current);

    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    short *input = (short *)input_v;
    int size_ = ((int *)args)[0];
    luint size_2 = (luint)size_ * 10000;

    luint offset = 0;
    for (int i = 0; i < 7; i++)
        offset = offset * 10 + input[i];

    if (offset < size_2 / 2)
    {
        printf("I can't solve this problem.\n");
        return 0;
    }

    // copying the necessary part of the input
    luint size = size_2 - offset; // size of the useful part
    short *first = (short *)malloc(sizeof(short) * size);
    luint repetitions = size / size_;
    // copying the necessary part to go back to input size multiple
    luint little_offset_size = (size - repetitions * size_);
    memcpy(first, input + offset % size_, sizeof(short) * little_offset_size);
    // copying the input until the end
    for (int i = 0; i < (int)repetitions; i++)
        memcpy(first + little_offset_size + i * size_, input, size_ * sizeof(short));

    // need a second input to write to because I'm going backwards
    short *second = (short *)malloc(sizeof(short) * size);

    // When offset is in second half, a given digit is just the sum of all the digits after it
    // we can then go from the end and just compute the sum

    for (int _ = 0; _ < 100; _++)
    {
        unsigned int sum = 0;
        for (int i = size - 1; i >= 0; i--)
        {
            sum += first[i];
            second[i] = sum % 10;
        }
        short *temp = first;
        first = second;
        second = temp;
    }

    luint res = 0;
    for (int i = 0; i < 8; i++)
        res = res * 10 + first[i];

    free(first);
    free(second);

    return res;
}

// ----------------------------------------------------------------

short *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    int s = fileSize(f);
    if (s <= 0 || !fgets(buffer, MAX_LINE_LEN, f))
    {
        fclose(f);
        return NULL;
    }

    *size = strlen(buffer);
    if (buffer[*size - 1] == '\n')
        *size = *size - 1;

    short *input = (short *)malloc(sizeof(short) * *size);
    for (int i = 0; i < *size; i++)
    {
        input[i] = buffer[i] - '0';
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    short *input = readInput(argv[1], &size);
    run(16, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}