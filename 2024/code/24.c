#include "runner.h"

typedef struct
{
    char first[4];
    char second[4];
    char result[4];
    int operation;
} Op;

typedef struct
{
    char name[4];
    uint64_t value;
} Register;

int getValue(char text[4], uint64_t *x, uint64_t *y, Register *regs, int nb_regs)
{
    int dec = atoi(text + 1);
    if (text[0] == 'x' || text[0] == 'y')
    {
        // printf("dec : %d\n", dec);
        return ((text[0] == 'x' ? *x : *y) >> dec) & 1; // thanks chat gpt
    }
    else
    {
        for (int x = 0; x < nb_regs; x++)
        {
            if (!strncmp(regs[x].name, text, 3))
            {
                return regs[x].value;
            }
        }
    }
    return -1;
}

// 1557948145 too low
// 1557953482 wrong
// 1557947505 too low
// 1557938177 wrong
luint part1(void *input_v, void **args)
{
    Op *input = (Op *)input_v;
    uint64_t size = ((uint64_t *)args)[0];
    uint64_t x = ((uint64_t *)args)[1];
    uint64_t y = ((uint64_t *)args)[2];
    uint64_t z = 0;

    Register *regs = (Register *)malloc(size * sizeof(Register));
    int nb_regs = 0;

    bool *done = (bool *)calloc(size, sizeof(bool));
    bool cont = true;
    while (cont)
    {
        cont = false;
        for (int i = 0; i < (int)size; i++)
        {
            if (done[i])
                continue;
            // printf("%c%c%c\n", input[i].first[0], input[i].first[1], input[i].first[2]);
            int v1, v2;
            int dec3 = atoi(input[i].result + 1);
            // printf("(%s %s)\n", input[i].first, input[i].second);
            v1 = getValue(input[i].first, &x, &y, regs, nb_regs);
            v2 = getValue(input[i].second, &x, &y, regs, nb_regs);
            if (v1 == -1 || v2 == -1)
                continue;
            // printf("%s %s : %d %d\n", input[i].first, input[i].second, v1, v2);
            //  printf("possible\n");
            int value;
            switch (input[i].operation)
            {
            case 0:
                value = v1 & v2;
                break;
            case 1:
                value = v1 | v2;
                break;
            case 2:
                value = v1 ^ v2;
                break;
            default:
                value = 0;
            }
            // printf("value : %d\n", value);
            // printf("dec : %d\n", dec);
            if (input[i].result[0] == 'z')
            {
                uint64_t f = 1ULL << dec3;
                if (value)
                    z = (z | f);
                else
                    z = (z & ~f);
            }
            else
            {
                bool exist = false;
                for (int x = 0; x < nb_regs; x++)
                {
                    if (!strncmp(regs[x].name, input[i].result, 3))
                    {
                        regs[x].value = value;
                        exist = true;
                        break;
                    }
                }
                if (!exist)
                {
                    strncpy(regs[nb_regs].name, input[i].result, 4);
                    regs[nb_regs].value = value;
                    nb_regs++;
                }
            }
            cont = true;
            done[i] = true;
        }
    }

    free(done);
    free(regs);
    return z;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{

    return 0;
}

// ----------------------------------------------------------------

Op *readInput(char *filename, int *size, uint64_t *x, uint64_t *y)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");

    fgets(buffer, MAX_LINE_LEN, f);
    while (strlen(buffer) > 2)
    {
        bool y_b = true;
        if (buffer[0] == 'x')
            y_b = false;
        uint64_t a, b;
        if (y_b)
        {
            if (sscanf(buffer, "y%lu: %lu", &a, &b) != 2)
                printf("error reading y\n");
            (*y) = (*y) | (b << a);
        }
        else
        {
            if (sscanf(buffer, "x%lu: %lu", &a, &b) != 2)
                printf("error reading x\n");
            (*x) = (*x) | (b << a);
        }

        fgets(buffer, MAX_LINE_LEN, f);
    }
    *size = fileSize(f);

    char **input = (char **)malloc(sizeof(char *) * *size);
    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        input[i] = strdup(buffer);
    }

    Op *operations = (Op *)malloc(sizeof(Op) * *size);
    for (int i = 0; i < *size; i++)
    {
        char a[4], b[4], c[4];
        int opnumber = 4;
        if (sscanf(input[i], "%s AND %s -> %s\n", a, b, c) == 3)
            opnumber = 0;
        else if (sscanf(input[i], "%s OR %s -> %s\n", a, b, c) == 3)
            opnumber = 1;
        else if (sscanf(input[i], "%s XOR %s -> %s\n", a, b, c) == 3)
            opnumber = 2;
        else
            printf("ERROR READING\n");
        operations[i] = (Op){{a[0], a[1], a[2], ('\0')}, {b[0], b[1], b[2], '\0'}, {c[0], c[1], c[2], '\0'}, opnumber};
    }

    for (int i = 0; i < *size; i++)
        free(input[i]);
    free(input);

    fclose(f);
    return operations;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    uint64_t x = 0, y = 0;
    Op *input = readInput(argv[1], &size, &x, &y);
    uint64_t args[3] = {(uint64_t)size, x, y};
    run(23, part1, part2, input, (void **)&args);
    free(input);
    return 0;
}