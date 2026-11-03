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

uint64_t compute(Op *input, uint64_t size, uint64_t x, uint64_t y)
{
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
            int v1, v2;
            int dec3 = atoi(input[i].result + 1);
            v1 = getValue(input[i].first, &x, &y, regs, nb_regs);
            v2 = getValue(input[i].second, &x, &y, regs, nb_regs);
            if (v1 == -1 || v2 == -1)
                continue;
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

luint part1(void *input_v, void **args)
{
    Op *input = (Op *)input_v;
    uint64_t size = ((uint64_t *)args)[0];
    uint64_t x = ((uint64_t *)args)[1];
    uint64_t y = ((uint64_t *)args)[2];
    return compute(input, size, x, y);
}

// -----------------------------------------------------------------

// I would not have found those 4 conditions myself, I have to admit.
// But when you look at them, it is pretty obvisou : we are just forcing some adder properties on input/outputs.

int cmp(const void *one, const void *two)
{
    return strncmp((*(Op **)one)->result, (*(Op **)two)->result, 3);
}

luint part2(void *input_v, void **args)
{
    Op *input = (Op *)input_v;
    uint64_t size = ((uint64_t *)args)[0];
    uint64_t x = ((uint64_t *)args)[1];
    uint64_t y = ((uint64_t *)args)[2];

    bool flagged[size];
    memset(flagged, false, sizeof(bool) * size);
    for (int i = 0; i < size; i++)
    {
        // 1. all "z" outputs come from XOR (not the last one, z45, which is a carry)
        if (input[i].result[0] == 'z' && input[i].operation != 2 && !(input[i].result[1] == '4' && input[i].result[2] == '5'))
        {
            flagged[i] = true;
            // printf("Case 1 : %s %d %s = %s\n", input[i].first, input[i].operation, input[i].second, input[i].result);
        }

        // 2. if not z, and input not x and y, can't be xor
        bool has_xy_inputs = (input[i].first[0] == 'x' || input[i].first[0] == 'y' ||
                              input[i].second[0] == 'x' || input[i].second[0] == 'y');
        if (input[i].result[0] != 'z' && !has_xy_inputs && input[i].operation == 2)
        {
            flagged[i] = true;
            // printf("Case 2 : %s %d %s = %s\n", input[i].first, input[i].operation, input[i].second, input[i].result);
        }

        // 3. if x ^ y, output must be fed into XOR.
        bool has_only_xy_inputs = (input[i].first[0] == 'x' && input[i].second[0] == 'y') || (input[i].first[0] == 'y' && input[i].second[0] == 'x');
        bool is_bit_00 = (!strncmp(input[i].first, "x00", 3) && !strncmp(input[i].second, "y00", 3)) ||
                         (!strncmp(input[i].first, "y00", 3) && !strncmp(input[i].second, "x00", 3));
        if (input[i].operation == 2 && has_only_xy_inputs)
        {
            bool found_correct_out = is_bit_00;
            for (int j = 0; j < size; j++)
            {
                if (i == j || found_correct_out)
                    continue;
                if ((!strncmp(input[i].result, input[j].first, 3) || !strncmp(input[i].result, input[j].second, 3)) && input[j].operation == 2)
                {
                    found_correct_out = true;
                    break;
                }
            }

            if (!found_correct_out)
            {
                flagged[i] = true;
                // printf("Case 3 : %s %d %s = %s\n", input[i].first, input[i].operation, input[i].second, input[i].result);
            }
        }

        // 4. if AND -> fed into XOR
        if (input[i].operation == 0)
        {
            bool found_correct_out = is_bit_00;
            for (int j = 0; j < size; j++)
            {
                if (i == j)
                    continue;
                if ((!strncmp(input[i].result, input[j].first, 3) || !strncmp(input[i].result, input[j].second, 3)) && input[j].operation == 1)
                {
                    found_correct_out = true;
                    break;
                }
            }

            if (!found_correct_out)
            {
                flagged[i] = true;
                // printf("Case 4 : %s %d %s = %s\n", input[i].first, input[i].operation, input[i].second, input[i].result);
            }
        }
    }

    Op *results[8];
    int tot = 0;
    for (int i = 0; i < size && tot <= 8; i++)
        if (flagged[i])
        {
            results[tot++] = &input[i];
        }
    if (tot != 8)
    {
        printf("Error in initial conditions ; did not get 8 wires to move.\n");
        return -1;
    }
    qsort(results, tot, sizeof(Op *), cmp);

    printf("\n");
    for (int i = 0; i < tot; i++)
        printf("%s,", results[i]->result);
    printf("\n(remove last comma)\n");

    return tot;
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

    // what is left, so all ops
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
    run(24, part1, part2, input, (void **)&args);
    free(input);
    return 0;
}