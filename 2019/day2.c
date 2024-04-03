#include "utils.h"

/* Reads the tab of integers */
int *readFile(FILE *f, int *size)
{
    *size = 1; // first number has to exist, all the other ones come after a comma.
    fpos_t start;
    fgetpos(f, &start);
    while (!feof(f))
    {
        if (fgetc(f) == ',')
            (*size)++;
    }
    fsetpos(f, &start);

    int *tab = (int *)malloc(sizeof(int) * (*size));
    for (int i = 0; i < *size; i++)
    {
        tab[i] = -1;
    }
    int pos_in_tab = 0;
    char buffer[256];
    int pos_in_buffer = 0;
    while (!feof(f) && pos_in_tab < *size)
    {
        char c = fgetc(f);
        if (c == ',' || c == '\0' || c == '\n')
        {
            tab[pos_in_tab] = atoi(buffer);
            resetBuffer(buffer, 256);
            pos_in_tab++;
            pos_in_buffer = 0;
        }
        else
        {
            buffer[pos_in_buffer] = c;
            pos_in_buffer++;
        }
    }
    return tab;
}

int checkBoundsOpcode(int *tab, int pos, int size)
{
    return tab[pos + 3] >= size || tab[pos + 3] < 0 || tab[pos + 2] >= size || tab[pos + 2] < 0 || tab[pos + 1] >= size || tab[pos + 1] < 0;
}

void runIntcode(int *tab, int size, int start, int print)
{
    int pos = 0;
    while (pos + 3 < size)
    {
        if (tab[pos] == 99)
        {
            return;
        }
        if (checkBoundsOpcode(tab, pos, size))
        {
            if (print)
                printf("Error reading opcodes - bounds.\n");
            return;
        }
        switch (tab[pos])
        {
        case 1:
            tab[tab[pos + 3]] = tab[tab[pos + 1]] + tab[tab[pos + 2]];
            pos += 4;

            break;
        case 2:
            tab[tab[pos + 3]] = tab[tab[pos + 1]] * tab[tab[pos + 2]];
            pos += 4;

            break;
        default:
            if (print)
                printf("Error reading opcodes.\n");
            return;
        }
    }
}

void day2(FILE *f)
{

    int size;
    int *original_tab = readFile(f, &size);
    if (size <= 0)
    {
        printf("Error reading size.\n");
        free(original_tab);
        return;
    }

    int *tab = (int *)malloc(sizeof(int) * size);
    // my VSCode says memcpy is not safe so let's do that manually... rip speed
    for (int i = 0; i < size; i++)
    {
        tab[i] = original_tab[i];
    }
    tab[1] = 12;
    tab[2] = 2;
    runIntcode(tab, size, 0, 1);

    int result = -1;
    for (int noun = 0; noun < 100; noun++)
    {
        for (int verb = 0; verb < 100; verb++)
        {
            int *temp_tab = (int *)malloc(sizeof(int) * size);
            for (int i = 0; i < size; i++)
            {
                temp_tab[i] = original_tab[i];
            }
            temp_tab[1] = noun;
            temp_tab[2] = verb;
            runIntcode(temp_tab, size, 0, 0);
            if (temp_tab[0] == 19690720)
            {
                result = 100 * noun + verb;
                free(temp_tab);
                goto after;
            }
            free(temp_tab);
        }
    }
after:

    printf("Day 2 -- Part 1 : %d ; Part 2 : %d\n", tab[0], result);

    free(tab);
    free(original_tab);
}