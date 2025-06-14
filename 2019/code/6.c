#include "runner.h"

// Problem description : given a graph, count the sum of all paths to the COM, minus 1 each time.

typedef char Node[3];
typedef Node Link[2];

/// @brief Allows going from XXX, with X being 0-9 or A-Z, to a unique hash. Max is 46 656 so unsigned short is enough
/// @param node
/// @return
unsigned short hash(Node node)
{
    unsigned short res = 0;
    for (int i = 0; i < 3; i++)
    {
        if (node[i] >= '0' && node[i] <= '9')
            res = res * 36 + (node[i] - '0');
        else if (node[i] >= 'A' && node[i] <= 'Z')
            res = res * 36 + (node[i] - 'A' + 10);
    }
    return res;
}

luint part1(void *input_v, void **args)
{
    Link *input = (Link *)input_v;
    int size = ((int *)args)[0];

    // reading graph
    unsigned short graph[36 * 36 * 36] = {0};
    for (int i = 0; i < size; i++)
        graph[hash(input[i][1])] = hash(input[i][0]);

    unsigned short end = hash((Node){'C', 'O', 'M'});

    int res = 0;

    for (int i = 0; i < size; i++)
    {
        unsigned short current = hash(input[i][1]);
        int count;
        for (count = 0; current != end; count++)
            current = graph[current];
        res += count;
    }

    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    Link *input = (Link *)input_v;
    int size = ((int *)args)[0];

    // reading graph
    unsigned short graph[36 * 36 * 36] = {0};
    for (int i = 0; i < size; i++)
        graph[hash(input[i][1])] = hash(input[i][0]);

    // strategy : both santa and us are going toward the center and keeping track of where and when we were
    // if one of us arrives at the object the other has already been, then we know how many transfers we took...
    // ...and how many they took and that means we know how long the entire trip takes
    unsigned short santa = hash((Node){'S', 'A', 'N'});
    unsigned short us = hash((Node){'Y', 'O', 'U'});

    unsigned short seen[36 * 36 * 36] = {0};
    unsigned short seen_by_santa[36 * 36 * 36] = {0};

    unsigned short steps;
    for (steps = 1; seen_by_santa[us] == 0 && seen[santa] == 0; steps++)
    {
        seen_by_santa[santa] = steps;
        seen[us] = steps;
        us = graph[us];
        santa = graph[santa];
    }

    int distance;
    if (seen_by_santa[us])
        distance = seen_by_santa[us] + steps - 2; // -2 here because -1 for the steps++ done in last loop and -1 because can't count twice the same objects
    else
        distance = seen[santa] + steps - 2; // idem here

    return distance - 2; // -2 here because santa and us aren't object to orbit around, we want to reach the objects just before
}

// ----------------------------------------------------------------

Link *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    *size = fileSize(f);
    Link *input = (Link *)malloc(sizeof(Link) * *size);

    int offset = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        // find the ')'
        int mid = 0;
        for (int x = 1; x < (int)strlen(buffer) && mid == 0; x++)
            if (buffer[x] == ')')
                mid = x;
        // read before the ')'
        for (int i = 0; i < mid && i < 3; i++)
            input[offset][0][i] = buffer[i];
        // read after the ')'
        for (int i = 0; i < 3 && i < (int)strlen(buffer) - mid; i++)
            input[offset][1][i] = buffer[mid + 1 + i];
        buffer[0] = '\0'; // safety reset
        offset += 1;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Link *input = readInput(argv[1], &size);
    run(6, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}