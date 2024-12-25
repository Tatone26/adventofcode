#include "runner.h"

#define WIDTH 5
#define TOTAL_KEY_HEIGHT 7

typedef struct
{
    int columns[WIDTH];
    bool isKey;
} Key;

/// @brief really bad code here lol, but its the last day yay
Key *readKeys(char **input, int height, int *count)
{
    int c = height / TOTAL_KEY_HEIGHT;
    *count = c;
    Key *keys = (Key *)malloc(sizeof(Key) * c);
    for (int i = 0; i < c; i++)
    {
        bool isKey = true;
        if (input[i * TOTAL_KEY_HEIGHT][0] == '#')
            isKey = false;
        int columns[WIDTH] = {0};
        if (isKey)
        {
            for (int m = 0; m < TOTAL_KEY_HEIGHT - 1; m++)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    if (columns[x] == 0 && input[i * TOTAL_KEY_HEIGHT + m][x] == '#')
                        columns[x] = TOTAL_KEY_HEIGHT - 1 - m;
                }
            }
        }
        else
        {
            for (int m = 1; m < TOTAL_KEY_HEIGHT - 1; m++)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    if (input[i * TOTAL_KEY_HEIGHT + m][x] == '#')
                        columns[x] = m;
                }
            }
        }
        keys[i] = (Key){{columns[0], columns[1], columns[2], columns[3], columns[4]}, isKey};
    }
    return keys;
}

luint part1(void *input_v, void **args)
{
    char **input = (char **)input_v;
    int height = ((int *)args)[1];
    int count = 0;
    Key *keys = readKeys(input, height, &count);

    luint res = 0;
    for (int i = 0; i < count; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (keys[i].isKey == !keys[j].isKey)
            {
                bool good = true;
                for (int x = 0; x < WIDTH; x++)
                {
                    if (keys[i].columns[x] + keys[j].columns[x] > 5)
                    {
                        good = false;
                        break;
                    }
                }
                if (good)
                    res++;
            }
        }
    }
    free(keys);
    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    char **input = (char **)input_v;
    int height = ((int *)args)[0];
    printf("Answer Part 2 : there is no answer !\n");
    return height / strlen(input[0]);
}

// ----------------------------------------------------------------

char **readInput(char *filename, int *height, int *width)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *height = fileSize(f);
    *width = WIDTH;
    char **input = (char **)malloc(sizeof(char *) * *height);

    int count = 0;
    for (int i = 0; i < *height; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        if (strlen(buffer) <= 1)
            continue;
        input[count++] = strndup(buffer, *width);
    }
    *height = count;
    if (count > 0)
        input = realloc(input, sizeof(char *) * count);

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int sizes[2] = {0, 0};
    char **input = readInput(argv[1], &sizes[1], &sizes[0]);
    run(25, part1, part2, input, (void **)&sizes);
    for (int i = 0; i < sizes[1]; i++)
        free(input[i]);
    free(input);
    return 0;
}