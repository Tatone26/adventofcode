#include "runner.h"
#include "intcode.h"

luint part1(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    int max_res = 0;
    int sequence[5] = {0, 1, 2, 3, 4};
    while (1)
    {
        int in = 0;
        for (int x = 0; x < 5; x++)
        {
            int current_input[2] = {sequence[x], in};
            in = run_intcode(input, size, -1, -1, current_input, -1);
        }
        if (in > max_res)
            max_res = in;

        // next permutation (cf wikipedia) - this is optimizable (probably)
        // at least it is not 5 nested loops with conditions...

        int k = -1;
        for (int i = 0; i < 4; i++)
            if (sequence[i] < sequence[i + 1])
                k = i;
        if (k == -1)
            break;

        int l = k + 1;
        while (l < 5 && sequence[k] < sequence[l]) // sequence[k] < sequence[k + 1] anyway, so no problem here (I guess)
            l++;
        l--;

        // swap
        int temp = sequence[k];
        sequence[k] = sequence[l];
        sequence[l] = temp;
        // reverse
        int new_sequence[5];
        for (int i = 0; i < 5 - k - 1; i++)
            new_sequence[k + 1 + i] = sequence[5 - i - 1];
        for (int i = k + 1; i < 5; i++)
            sequence[i] = new_sequence[i];
    }
    return max_res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    int *input = (int *)input_v;
    int size = ((int *)args)[0];

    int max_res = 0;
    int sequence[5] = {5, 6, 7, 8, 9};
    while (1)
    {
        int in = 0;
        for (int x = 0; x < 5; x++)
        {
            int current_input[2] = {sequence[x], in};
            in = run_intcode(input, size, -1, -1, current_input, -1);
        }
        if (in > max_res)
            max_res = in;

        // next permutation (cf wikipedia) - this is optimizable (probably)
        // at least it is not 5 nested loops with conditions...

        int k = -1;
        for (int i = 0; i < 4; i++)
            if (sequence[i] < sequence[i + 1])
                k = i;
        if (k == -1)
            break;

        int l = k + 1;
        while (l < 5 && sequence[k] < sequence[l]) // sequence[k] < sequence[k + 1] anyway, so no problem here (I guess)
            l++;
        l--;

        // swap
        int temp = sequence[k];
        sequence[k] = sequence[l];
        sequence[l] = temp;
        // reverse
        int new_sequence[5];
        for (int i = 0; i < 5 - k - 1; i++)
            new_sequence[k + 1 + i] = sequence[5 - i - 1];
        for (int i = k + 1; i < 5; i++)
            sequence[i] = new_sequence[i];
    }
    return max_res;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    *size = fileSize(f);
    char *input = (char *)malloc(sizeof(char) * *size);

    int offset = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        strncpy(input + offset, buffer, strlen(buffer) - 1); // -1 to remove \n
        offset += strlen(buffer) - 1;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    int *input = read_intcode(argv[1], &size);
    run(7, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}