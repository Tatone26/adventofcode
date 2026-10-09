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

    bool *graph = (bool *)calloc(MAX_HASH * MAX_HASH, sizeof(bool));
    for (int i = 0; i < size; i++)
    {
        int a = hash(input + i * 6);
        int b = hash(input + i * 6 + 3);
        graph[a * MAX_HASH + b] = true;
        graph[b * MAX_HASH + a] = true;
    }

    int res = 0;
    bool done[26] = {0};
    int min_hash = hash("ta");
    int max_hash = hash("tz");
    for (int h = min_hash; h <= max_hash; h++)
    {
        int interconnected[MAX_HASH];
        int count = 0;
        for (int i = 0; i < MAX_HASH; i++)
        {
            if (min_hash <= i && max_hash >= i && done[i - min_hash]) // 'ta' -> 'tz' already tested, do not re-count
                continue;

            if (graph[h * MAX_HASH + i])
            {
                interconnected[count] = i;
                count++;
            }
        }
        for (int j = 0; j < count; j++)
        {
            int testing = interconnected[j];
            for (int r = j; r < count; r++) // three imbricated loops -> a three-member clique.
            {
                if (graph[testing * MAX_HASH + interconnected[r]])
                    res++;
            }
        }
        done[h - min_hash] = true;
    }

    free(graph);

    return res;
}

// -----------------------------------------------------------------
#define MAX_SET_SIZE 32

void rec_check(const bool *graph, int *cur_set, int cur_len,
               const bool *cand, int prev_node,
               int nb_nodes, int *best_find, int *best_len)
{
    if (nb_nodes <= 0)
        return;
    bool candidates[nb_nodes];
    memcpy(candidates, cand, nb_nodes * sizeof(bool));

    for (int c = prev_node + 1; c < nb_nodes; c++)
    {
        if (!cand[c])
            continue;

        // Choose vertex c
        cur_set[cur_len] = c;

        int cand_count = 0;
        memset(candidates, 0, nb_nodes * sizeof(bool));
        for (int i = c + 1; i < nb_nodes; i++)
        {
            // choose new potential candidates (nodes linked to all nodes in current set)
            // only iterate on nodes starting from this one, as we tested all the other ones.
            if (cand[i] && graph[c * nb_nodes + i])
            {
                candidates[i] = true;
                cand_count++; // ALWAYS count instead of just putting "true". Same cost, may be useful.
            }
        }

        if (cur_len + cand_count < *best_len) // the last magical pruning. Most cliques have really little candidates.
            continue;                         // This allow us to concentrate the efforts only on the big, lot-of-possibilities ones

        if (cand_count)
        {
            // if any, then recursive search with node c included and length updated
            rec_check(graph, cur_set, cur_len + 1, candidates, c,
                      nb_nodes, best_find, best_len);
        }
        else
        {
            // Maximal clique reached : we can't add anything to it.
            if (cur_len + 1 > *best_len) // we check if it is the best we ever found (all branches)
            {
                *best_len = cur_len + 1;
                memcpy(best_find, cur_set, (cur_len + 1) * sizeof(int));
            }
        }
    }
}

luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int size = ((int *)args)[0];

    int nb_nodes = 0;
    int *nodes = (int *)malloc(MAX_HASH * sizeof(int));
    int *rev = (int *)malloc(MAX_HASH * sizeof(int));
    memset(rev, -1, MAX_HASH * sizeof(int));

    for (int i = 0; i < size; i++)
    {
        int h1 = hash(input + i * 6);
        int h2 = hash(input + i * 6 + 3);

        if (rev[h1] == -1)
        {
            rev[h1] = nb_nodes;
            nodes[nb_nodes++] = h1;
        }
        if (rev[h2] == -1)
        {
            rev[h2] = nb_nodes;
            nodes[nb_nodes++] = h2;
        }
    }

    if (nb_nodes <= 0)
    {
        free(nodes);
        free(rev);
        return -1;
    }

    bool *graph = (bool *)calloc(nb_nodes * nb_nodes, sizeof(bool));
    for (int i = 0; i < size; i++)
    {
        int a = rev[hash(input + i * 6)];
        int b = rev[hash(input + i * 6 + 3)];
        graph[a * nb_nodes + b] = true;
        graph[b * nb_nodes + a] = true;
    }

    int cur_set[MAX_SET_SIZE] = {0};
    int best_find[MAX_SET_SIZE] = {0};
    int best_len = 0;

    bool candidates[nb_nodes];
    for (int i = 0; i < nb_nodes; i++)
        candidates[i] = true;

    rec_check(graph, cur_set, 0, candidates, -1, nb_nodes, best_find, &best_len);

    // Mark who is in best_find for quick sorted printing
    bool in_best[MAX_HASH] = {false};
    for (int i = 0; i < best_len; i++)
        in_best[best_find[i]] = true;

    printf("Answer : ");
    for (int i = 0; i < MAX_HASH; i++)
    {
        if (rev[i] == -1)
            continue;
        int k = rev[i];
        if (in_best[k])
        {
            printf("%c%c,", unhash(nodes[k]).text[0], unhash(nodes[k]).text[1]);
        }
    }
    printf("\n(remove last comma)\n");

    free(graph);
    free(nodes);
    free(rev);

    return (luint)best_len;
}

// ----------------------------------------------------------------

// readInput here flattens the answer, each line is 6 chars longs (keeping the '\n')
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