#include "runner.h"

#define MAX_HASH 1626

int hash(char *c)
{
    return (((c[0] - 'a') << 6) | (c[1] - 'a'));
}

typedef struct
{
    char text[2];
} doubleChar;

doubleChar unhash(int d)
{
    char a = (d & 63) + 'a';
    char b = (d >> 6) + 'a';
    return (doubleChar){{b, a}};
}

luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    bool **graph = (bool **)malloc(sizeof(bool *) * MAX_HASH);
    for (int i = 0; i < MAX_HASH; i++)
    {
        graph[i] = (bool *)calloc(sizeof(bool), MAX_HASH);
    }
    for (int i = 0; i < size; i++)
    {
        int a = hash(input + i * 6);
        int b = hash(input + i * 6 + 3);
        graph[a][b] = true;
        graph[b][a] = true;
    }

    int res = 0;
    bool done[26] = {0};
    int min_hash = hash("ta");
    int max_hash = hash("tz");
    for (int x = 'a'; x <= 'z'; x++)
    {
        char test[2] = {'t', x};
        int h = hash(test);

        int interconnected[MAX_HASH];
        int count = 0;
        for (int i = 0; i < MAX_HASH; i++)
        {
            if (min_hash <= i && max_hash >= i)
            {
                if (done[i - min_hash])
                    continue;
            }
            if (graph[h][i])
            {
                interconnected[count] = i;
                count++;
            }
        }
        for (int j = 0; j < count; j++)
        {
            int testing = interconnected[j];
            for (int r = j; r < count; r++)
            {
                if (graph[testing][interconnected[r]])
                    res++;
            }
        }
        done[h - min_hash] = true;
    }

    for (int i = 0; i < MAX_HASH; i++)
    {
        free(graph[i]);
    }
    free(graph);

    return res;
}

// -----------------------------------------------------------------

// largest set of fully interconnected computers
// wanted result for test:
// size 6
//
// ka-co
// ta-co
// de-co
// ta-ka
// de-ta
// ka-de

// (ka, co, de, ta)
// recursive version : from a given set S, made from a root R, try to add a point P to it. If possible, recursion with S u {P}. Test every single point not in S. If no point can be added, return S.
// compare all S u {P}, and choose the biggest one to return. S being maximal, we know that it is the biggest one contening the root R. No need to test R again anywhere.

typedef struct
{
    int n;
    int capacity;
    int *points;
} NodeSet;

/// capacity should be the total number of point in the graph.
NodeSet *newSet(int capacity)
{
    NodeSet *new = (NodeSet *)malloc(sizeof(NodeSet));
    new->n = 0;
    new->capacity = capacity;
    new->points = (int *)malloc(sizeof(int) * capacity);
    return new;
}

void freeSet(NodeSet *set)
{
    free(set->points);
    free(set);
}

void addPoint(NodeSet *set, int point)
{
    set->points[set->n] = point;
    set->n += 1;
}

bool contains(const NodeSet *set, int point)
{
    for (int i = 0; i < set->n; i++)
        if (set->points[i] == point)
            return true;
    return false;
}

void removePoint(NodeSet *set, int point)
{
    for (int i = 0; i < set->n; i++)
        if (set->points[i] == point)
        {
            set->points[i] = set->points[set->n - 1];
            set->n -= 1;
            break;
        }
}

NodeSet *copySet(NodeSet *set)
{
    NodeSet *new = newSet(set->capacity);
    new->n = set->n;
    memcpy(new->points, set->points, sizeof(int) * set->capacity);
    return new;
}

// I just wrote a direct application of the Bron-Kerbosh algorithm, which finds all the maximal cliques in a graph. Finding the maximum is easy.
// Slow : bad data structure and a lot of copies.
NodeSet *bronKerbosch(bool **graph, NodeSet *r_set, NodeSet *p_set, NodeSet *x_set)
{
    // if P empty and X empty, then found maximal clique
    if (p_set->n == 0 && x_set->n == 0)
        return copySet(r_set);
    // else do the recursion (and choose the biggest maximal clique)
    int bestSize = 0;
    NodeSet *bestSet = NULL;
    for (int v_i = 0; v_i < p_set->n; v_i++)
    {
        // v in P
        int v = p_set->points[v_i];
        removePoint(p_set, v);

        // R u {v}
        NodeSet *r_set_new = copySet(r_set);
        addPoint(r_set_new, v);

        // update P and X
        NodeSet *p_set_new = copySet(p_set);
        NodeSet *x_set_new = copySet(x_set);
        for (int j = 0; j < MAX_HASH; j++)
        {
            if (!graph[v][j])
            {
                if (contains(p_set_new, j))
                    removePoint(p_set_new, j); // P inter N(v)
                if (contains(x_set_new, j))
                    removePoint(x_set_new, j); // X inter N(v)
            }
        }
        NodeSet *next = bronKerbosch(graph, r_set_new, p_set_new, x_set_new);
        addPoint(x_set, v);
        // if found a maximal clique, keep the biggest one
        if (next)
        {
            if (bestSet == NULL || next->n > bestSize)
            {
                bestSize = next->n;
                if (bestSet)
                    freeSet(bestSet);
                bestSet = next;
            }
            else
                freeSet(next);
        }
        freeSet(r_set_new);
        freeSet(p_set_new);
        freeSet(x_set_new);
    }
    return bestSet;
}

int comp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    bool **graph = (bool **)malloc(sizeof(bool *) * MAX_HASH);
    for (int i = 0; i < MAX_HASH; i++)
    {
        graph[i] = (bool *)calloc(sizeof(bool), MAX_HASH);
    }
    for (int i = 0; i < size; i++)
    {
        int a = hash(input + i * 6);
        int b = hash(input + i * 6 + 3);
        graph[a][b] = true;
        graph[b][a] = true;
        graph[a][a] = true;
        graph[b][b] = true;
    }

    NodeSet *p_set = newSet(MAX_HASH);
    for (int i = 0; i < MAX_HASH; i++)
        if (graph[i][i])
            addPoint(p_set, i);
    NodeSet *x_set = newSet(p_set->n); // p_set->n is the number of point in the graph
    NodeSet *r_set = newSet(p_set->n);

    NodeSet *best = bronKerbosch(graph, r_set, p_set, x_set);
    freeSet(r_set);

    qsort(best->points, best->n, sizeof(int), comp);

    char res[MAX_LINE_LEN] = {0};
    int offset = 0;
    for (int i = 0; i < best->n; i++)
    {
        doubleChar dc = unhash(best->points[i]);
        res[offset++] = dc.text[0];
        res[offset++] = dc.text[1];
        res[offset++] = ',';
    }
    res[offset - 1] = '\0';
    printf("Answer Part 2 : %s\n", res);

    luint temp = best->n;
    freeSet(best);
    freeSet(x_set);
    freeSet(p_set);
    for (int i = 0; i < MAX_HASH; i++)
        free(graph[i]);
    free(graph);

    return temp;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;
    *size = fileSize(f);
    char *input = (char *)malloc(sizeof(char) * *size * 6);

    for (int i = 0; i < *size; i++)
    {
        if (!fgets(buffer, MAX_LINE_LEN, f))
        {
            fclose(f);
            free(input);
            return NULL;
        }
        strncpy(input + i * 6, buffer, 6);
        *(input + i * 6 + 5) = '\0';
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    char *input = readInput(argv[1], &size);
    run(23, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}