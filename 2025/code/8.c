#include "runner.h"

// WHY EUCLIDIAN DISTANCE
// Not my proudest day, and can be optimised a LOT with a bit of motivation and better data structures.

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
    long long d;
} Distance;

#define squared(x) ((long long)(x) * (long long)(x))

static int compare_int(const void *a, const void *b)
{
    return (*(int *)b - *(int *)a);
}

// Because we are working on positive integers only, a radix sort is a lot better than any comparative sorting algorithm (it is in O(n))
// I have to admit I discovered this technique today, and would probably not be able to reproduce it myself :)
void radix_sort_distances(Distance *arr, int n)
{
    if (n <= 0)
        return;
    Distance *temp = (Distance *)malloc(n * sizeof(Distance));
    int count[256];

    // a long long (Distance.d) is 8 bytes long, so we have 64 bits in total
    // we sort byte by byte
    for (int shift = 0; shift < 8 * 8; shift += 8)
    {
        // Reset histogram
        for (int i = 0; i < 256; i++)
            count[i] = 0;

        // 1. Count occurrences in each bucket
        for (int i = 0; i < n; i++)
            count[(arr[i].d >> shift) & 0xFF]++;

        // 2. Get position of each bucket
        for (int i = 1; i < 256; i++)
            count[i] += count[i - 1];

        // 3. Move elements to their place (backwards to keep stability)
        for (int i = n - 1; i >= 0; i--)
        {
            int byte = (arr[i].d >> shift) & 0xFF;
            count[byte]--;
            temp[count[byte]] = arr[i];
        }

        // 4. Copying into arr
        for (int i = 0; i < n; i++)
            arr[i] = temp[i];
    }
    free(temp);
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
            long long d = (squared(input[i].x - input[j].x) + squared(input[i].y - input[j].y) + squared(input[i].z - input[j].z));
            distances[c].a = i;
            distances[c].b = j;
            distances[c++].d = d;
        }
    // this sort is the bottleneck. A smarter data structure might me useful (like a max-heap)
    radix_sort_distances(distances, c);

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

        // this is pretty stupid and puts us in O(n²) but... it works. There are a lot of other data structures that can make it faster.
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
            long long d = (squared(input[i].x - input[j].x) + squared(input[i].y - input[j].y) + squared(input[i].z - input[j].z));
            distances[c].a = i;
            distances[c].b = j;
            distances[c++].d = d;
        }
    radix_sort_distances(distances, c);

    int *circuit = (int *)malloc(size * sizeof(int));
    int *counts = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        circuit[i] = i;
        counts[i] = 1;
    }

    // keeping the last two x coordinates we read (so we can return them)
    int last_x_1 = 0;
    int last_x_2 = 0;

    // max_count is the biggest circuit. Testing its size allow us to know if there is more only 1 circuit left.
    int max_count = 1;
    for (int i = 0; max_count < size; i++)
    {
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