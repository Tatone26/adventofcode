#include "runner.h"

#define WIDTH 25
#define HEIGHT 6
#define PIXELS (WIDTH * HEIGHT)

luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    if (size % PIXELS != 0)
    {
        printf("File may be incorrect.\n");
        return -1;
    }

    int nb_layers = size / PIXELS;
    int best_layer = 0;
    int nb_0_best_layer = __INT32_MAX__;

    for (int i = 0; i < nb_layers; i++)
    {
        int nb_zeros = 0;
        for (int x = 0; x < PIXELS; x++)
            if (input[i * PIXELS + x] == '0')
                nb_zeros++;
        if (nb_zeros < nb_0_best_layer)
        {
            nb_0_best_layer = nb_zeros;
            best_layer = i;
        }
    }

    int ones = 0, twos = 0;
    for (int x = 0; x < PIXELS; x++)
    {
        if (input[best_layer * PIXELS + x] == '1')
            ones++;
        else if (input[best_layer * PIXELS + x] == '2')
            twos++;
    }

    return ones * twos;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    if (size % PIXELS != 0)
    {
        printf("File may be incorrect.\n");
        return -1;
    }

    int nb_layers = size / PIXELS;

    char *result = (char *)malloc(sizeof(char) * size);

    for (int i = 0; i < PIXELS; i++)
    {
        int layer = 0;
        while (input[layer * PIXELS + i] == '2' && layer < nb_layers)
            layer++;
        result[i] = input[layer * PIXELS + i];
    }

    printf("\nMessage : \n");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (result[y * WIDTH + x] == '1')
                printf("#");
            else
                printf(" ");
        }
        printf("\n");
    }

    return 0;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    if (!fgets(buffer, MAX_LINE_LEN, f))
    {
        fclose(f);
        return NULL;
    }
    for (int i = strlen(buffer) - 1; i >= 0; i--)
        if (buffer[i] == ' ' || buffer[i] == '\n')
            buffer[i] = '\0';
    *size = strlen(buffer);
    char *input = (char *)malloc(sizeof(char) * *size);
    strncpy(input, buffer, strlen(buffer)); // \n final

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    char *input = readInput(argv[1], &size);
    run(8, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}