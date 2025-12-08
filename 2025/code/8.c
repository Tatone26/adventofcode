#include "runner.h"

// WHY EUCLIDIAN DISTANCE

typedef struct
{
    int x;
    int y;
    int z;
} Box;

typedef struct
{
    int a;
    int b;
    double d;
} Distance;

#define squared(x) ((double)(x) * (double)(x))

static int compare_distance(const void *a, const void *b)
{
    return (*(Distance *)a).d - (*(Distance *)b).d;
}

static int compare_int(const void *a, const void *b)
{
    return (*(int *)b - *(int *)a);
}

#define N_1 1000

luint part1(void *input_v, void **args)
{
    Box *input = (Box *)input_v;
    int size = ((int *)args)[0];
    assert(size >= N_1);

    Distance *distances = (Distance *)calloc(size * size, sizeof(Distance));
    int c = 0;
    for (int i = 0; i < size; i++)
        for (int j = i + 1; j < size; j++)
        {
            double d = sqrt(squared((double)input[i].x - (double)input[j].x) + squared((double)input[i].y - (double)input[j].y) + squared((double)input[i].z - (double)input[j].z));
            distances[c].a = i;
            distances[c].b = j;
            distances[c++].d = d;
        }
    qsort(distances, c, sizeof(Distance), compare_distance);

    int *circuit = (int *)malloc(size * sizeof(int));
    int *counts = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        circuit[i] = i;
        counts[i] = 1;
    }

    for (int i = 0; i < N_1; i++)
    {
        if (circuit[distances[i].b] == circuit[distances[i].a])
            continue;
        counts[circuit[distances[i].a]] += counts[circuit[distances[i].b]];
        counts[circuit[distances[i].b]] = 0;
        int temp = circuit[distances[i].b];
        for (int x = 0; x < size; x++)
            if (circuit[x] == temp)
                circuit[x] = circuit[distances[i].a];
    }

    qsort(counts, size, sizeof(int), compare_int);

    luint res = (luint)counts[0] * (luint)counts[1] * (luint)counts[2];

    free(circuit);
    free(counts);
    free(distances);
    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    Box *input = (Box *)input_v;
    int size = ((int *)args)[0];

    Distance *distances = (Distance *)calloc(size * size, sizeof(Distance));
    int c = 0;
    for (int i = 0; i < size; i++)
        for (int j = i + 1; j < size; j++)
        {
            double d = sqrt(squared((double)input[i].x - (double)input[j].x) + squared((double)input[i].y - (double)input[j].y) + squared((double)input[i].z - (double)input[j].z));
            distances[c].a = i;
            distances[c].b = j;
            distances[c++].d = d;
        }
    qsort(distances, c, sizeof(Distance), compare_distance);

    int *circuit = (int *)malloc(size * sizeof(int));
    int *counts = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        circuit[i] = i;
        counts[i] = 1;
    }

    int i = -1;
    int max_count = 1;
    int last_x_1 = 0;
    int last_x_2 = 0;
    while (max_count < size)
    {
        i++;
        if (circuit[distances[i].b] == circuit[distances[i].a])
            continue;

        last_x_1 = input[distances[i].a].x;
        last_x_2 = input[distances[i].b].x;

        counts[circuit[distances[i].a]] += counts[circuit[distances[i].b]];
        counts[circuit[distances[i].b]] = 0;

        int temp = circuit[distances[i].b];
        for (int x = 0; x < size; x++)
            if (circuit[x] == temp)
                circuit[x] = circuit[distances[i].a];

        if (counts[circuit[distances[i].a]] > max_count)
            max_count = counts[circuit[distances[i].a]];
    }
    luint res = (luint)last_x_1 * (luint)last_x_2;

    free(circuit);
    free(counts);
    free(distances);
    return res;
}

// ----------------------------------------------------------------

Box *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    *size = fileSize(f);
    Box *input = (Box *)malloc(sizeof(Box) * (*size));

    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "%d,%d,%d\n", &input[i].x, &input[i].y, &input[i].z);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Box *input = readInput(argv[1], &size);
    run(8, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}