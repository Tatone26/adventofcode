#include "runner.h"

// In this one, the difficulty is only the parsing.
// Since I've done everything manually, the second part is much faster then the first !

luint part1(void *input_v, void **args)
{
    char **input = (char **)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    luint tot = 0;
    int offset = 0;
    while (offset < width)
    {
        char op = (input[height - 1] + offset)[0];
        luint temp = 0;
        if (op == '*')
            temp = 1;

        int max_n = 0;

        for (int i = 0; i < height - 1; i++)
        {
            luint a = (luint)atoi(input[i] + offset); // this is the bottleneck but i can't be bothered to do it manually
            switch (op)
            {
            case '+':
                temp += a;
                break;
            case '*':
                temp *= a;
                break;
            }
            int a_n = nbOfDigits(a);
            if (a_n > max_n)
                max_n = a_n;
        }
        tot += temp;
        offset += max_n + 1;
    }

    return tot;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    char **input = (char **)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    luint tot = 0;
    int offset = 0;
    while (offset < width)
    {
        char op = (input[height - 1] + offset)[0]; // that doesn't change.
        luint temp = 0;
        if (op == '*')
            temp = 1;

        int column = 0;
        int has_number = 1;
        while (has_number && (offset + column) < width)
        {
            has_number = 0;
            int number = 0;
            for (int i = 0; i < height - 1; i++)
            {
                char c = (input[i] + offset + column)[0];
                if (c != ' ')
                    number = number * 10 + (c - '0');
            }

            if (number != 0)
            {
                switch (op)
                {
                case '+':
                    temp += number;
                    break;
                case '*':
                    temp *= number;
                    break;
                }
                has_number = 1; // check next column
            }

            column++;
        }

        tot += temp;
        offset += column;
    }

    return tot;
}

// ----------------------------------------------------------------

char **readInput(char *filename, int *width, int *height)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    int temp = fileSize(f);
    char **input = (char **)malloc(sizeof(char *) * (temp));
    *height = temp;
    for (int i = 0; i < temp; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        input[i] = strndup(buffer, strlen(buffer) - 1);
        int l = strlen(input[i]);
        if (l > *width)
            *width = l;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int width = 0, height = 0;
    char **input = readInput(argv[1], &width, &height);
    int args[2] = {width, height};
    run(6, part1, part2, input, (void **)args);
    for (int i = 0; i < height; i++)
        free(input[i]);
    free(input);
    return 0;
}