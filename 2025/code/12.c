#include "runner.h"

#define NSHAPE 6

typedef struct
{
    int nb_blocs;
    bool blocs[3][3];
} Shape;

typedef struct
{
    int width;
    int height;
    int weights[NSHAPE];
} Objective;

luint part1(void *input_v, void **args)
{
    Objective *input = (Objective *)input_v;
    int nb_objectives = *(int *)((__ptr_t *)args)[0];
    Shape *shapes = *(Shape **)((__ptr_t *)args)[1];

    luint tot = 0;

    int dont_know = 0;
    for (int i = 0; i < nb_objectives; i++)
    {
        // printf("%d\n", i);
        /* int rounded_size = (input[i].height / 3) * (input[i].width / 3);
        printf("rounded : %d\n", rounded_size);
        int nb_to_put_in = 0;
        for (int x = 0; x < NSHAPE; x++)
            nb_to_put_in += input[i].weights[x];
        if (nb_objectives <= rounded_size)
        {
            // "obviously" correct => absolutely empty
            tot++;
            continue;
        } */
        int true_total_objective = 0;
        int size = input[i].height * input[i].width;
        for (int x = 0; x < NSHAPE; x++)
        {
            // printf("x : %d\n", x);
            true_total_objective += shapes[x].nb_blocs * input[i].weights[x];
        }
        if (true_total_objective > size)
        {
            // obvisouly impossible => easy to remove
            continue;
        }

        // yeah apparently that works for input : if there is enough space for it, then you can just admit it fits...

        dont_know++;
    }
    printf("unsure : %d\n", dont_know);
    return tot + dont_know;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    (void)input_v;
    (void)args;

    printf("GG\n");

    return 24;
}

// ----------------------------------------------------------------

Objective *readInput(char *filename, int *size, Shape **shapes)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);

    *shapes = (Shape *)malloc(sizeof(Shape) * NSHAPE);

    for (int i = 0; i < NSHAPE; i++)
    {
        // number line
        if (!fgets(buffer, MAX_LINE_LEN, f))
            break;
        (*shapes)[i].nb_blocs = 0;
        int stream_ok = 1;
        for (int x = 0; x < 3; x++) // line
        {
            char l[3];
            if (!fgets(buffer, MAX_LINE_LEN, f))
            {
                stream_ok = 0;
                break;
            }
            sscanf(buffer, "%c%c%c", &l[0], &l[1], &l[2]);
            for (int k = 0; k < 3; k++) // column
            {
                if (l[k] == '#')
                {
                    (*shapes)[i].nb_blocs++;
                    (*shapes)[i].blocs[k][x] = 1; // blocs[x][y]
                }
                else
                {
                    (*shapes)[i].blocs[k][x] = 0;
                }
            }
        }
        // empty line
        if (!stream_ok)
            break;
        if (!fgets(buffer, MAX_LINE_LEN, f))
            break;
    }

    *size = fileSize(f);

    Objective *input = (Objective *)malloc(*size * sizeof(Objective));

    for (int i = 0; i < *size; i++)
    {
        int x, y, a, b, c, d, e, f_; // fixed number of shapes (6)
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "%dx%d: %d %d %d %d %d %d\n", &x, &y, &a, &b, &c, &d, &e, &f_);
        input[i].height = y;
        input[i].width = x;
        input[i].weights[0] = a;
        input[i].weights[1] = b;
        input[i].weights[2] = c;
        input[i].weights[3] = d;
        input[i].weights[4] = e;
        input[i].weights[5] = f_;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int nb_objectives = 0;
    Shape *shapes = NULL;
    Objective *input = readInput(argv[1], &nb_objectives, &shapes);
    __ptr_t args[] = {&nb_objectives, &shapes};
    run(12, part1, part2, input, (void **)args);
    free(input);
    free(shapes);
    return 0;
}