#include "runner.h"

typedef struct
{
    char name[4];
    int nb_next;
    int *next;
    int nb_prec;
} Node;

typedef struct
{
    int nb_nodes;
    Node *nodes;
} Graph;

luint nb_paths_between(Graph *input, char start_node[3], char end_node[3])
{
    int nb_nodes = input->nb_nodes;
    // finding the position of the start node
    int start = -1;
    for (int i = 0; i < nb_nodes; i++)
        if (!strncmp(input->nodes[i].name, start_node, 3))
        {
            start = i;
            break;
        }
    if (start == -1)
        return 0;

    // the secret is that during the BFS, we need to select a node only if all paths leading to it have been found.
    // for that, we need to count how many ACCESSIBLE nodes lead to any given point.
    // to count these, we just do a simple BFS.
    int *is_accessible = (int *)calloc(nb_nodes, sizeof(int));
    int *bfs_to_do = (int *)calloc(nb_nodes, sizeof(int));
    int *next_bfs_to_do = (int *)malloc(nb_nodes * sizeof(int));
    bfs_to_do[start] = 1;
    is_accessible[start] = 1;

    // this BFS is catastrophically not optimised, but it works well anyway for such small graphs !
    int again = 1;
    while (again)
    {
        again = 0;
        // reset next_bfs
        memset(next_bfs_to_do, 0, sizeof(int) * nb_nodes);
        // use all current nodes
        for (int k = 0; k < nb_nodes; k++)
        {
            if (!bfs_to_do[k])
                continue;
            for (int n = 0; n < input->nodes[k].nb_next; n++)
            {
                if (!is_accessible[input->nodes[k].next[n]])
                {
                    // recursion only if never tested before
                    next_bfs_to_do[input->nodes[k].next[n]] = 1;
                    is_accessible[input->nodes[k].next[n]] = 1;
                    again = 1;
                }
            }
        }
        // swap
        int *temp = bfs_to_do;
        bfs_to_do = next_bfs_to_do;
        next_bfs_to_do = temp;
    }
    free(bfs_to_do);
    free(next_bfs_to_do);

    // result (nb path to get to the node)
    int *nb_paths = (int *)calloc(nb_nodes, sizeof(int));

    // queue
    int *todo = (int *)malloc(sizeof(int) * nb_nodes);
    int nb_todo = 0;

    int *paths_to_find = (int *)calloc(nb_nodes, sizeof(int));

    // here, we use is_accessible to count the number of paths to find.
    // this loop could be placed directly inside the BFS, but it's clearer here.
    for (int k = 0; k < nb_nodes; k++)
    {
        if (!is_accessible[k])
            continue;
        for (int n = 0; n < input->nodes[k].nb_next; n++)
            paths_to_find[input->nodes[k].next[n]]++;
    }
    free(is_accessible);

    todo[nb_todo++] = start;
    nb_paths[start] = 1;
    paths_to_find[start] = 0;

    while (nb_todo > 0)
    {
        int current_node = todo[nb_todo - 1];
        int current_nb_paths = nb_paths[current_node];
        nb_todo--;

        if (!strncmp(input->nodes[current_node].name, end_node, 3))
            continue;

        for (int k = 0; k < input->nodes[current_node].nb_next; k++) // for all neighbors (forward)
        {
            // update the number of paths to get to the next node
            nb_paths[input->nodes[current_node].next[k]] += current_nb_paths;
            // we found a path !
            int left_paths = (--paths_to_find[input->nodes[current_node].next[k]]);

            if (!left_paths)
            // if we discovered all paths to there, we can use it next
            {
                assert(input->nodes[current_node].next[k] < nb_nodes);
                todo[nb_todo++] = input->nodes[current_node].next[k];
            }
        }
    }

    luint res = 0;
    for (int i = 0; i < nb_nodes; i++)
        if (!strncmp(input->nodes[i].name, end_node, 3))
        {
            res = nb_paths[i];
            break;
        }

    free(nb_paths);
    free(todo);
    free(paths_to_find);
    return res;
}

luint part1(void *input_v, void **args)
{
    Graph *input = (Graph *)input_v;
    (void)args;

    return nb_paths_between(input, "you", "out");
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    Graph *input = (Graph *)input_v;
    (void)args;

    luint res = 0;

    // only two possibilities : svr-dac-fft-out or svr-fft-dac-out

    res += (luint)nb_paths_between(input, "svr", "dac") *
           nb_paths_between(input, "dac", "fft") *
           nb_paths_between(input, "fft", "out");

    res += (luint)nb_paths_between(input, "svr", "fft") *
           nb_paths_between(input, "fft", "dac") *
           nb_paths_between(input, "dac", "out");

    return res;
}

// ----------------------------------------------------------------

Graph *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    *size = fileSize(f);
    Graph *input = (Graph *)malloc(sizeof(Graph));
    input->nodes = (Node *)malloc(sizeof(Node) * (*size + 1)); // adding OUT as a node
    input->nb_nodes = (*size + 1);

    fpos_t start;
    fgetpos(f, &start);

    for (int i = 0; i < *size; i++)
    {
        char buf[4];
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "%c%c%c:", &buf[0], &buf[1], &buf[2]);
        input->nodes[i].name[0] = buf[0];
        input->nodes[i].name[1] = buf[1];
        input->nodes[i].name[2] = buf[2];
        input->nodes[i].name[3] = '\0';
        input->nodes[i].nb_prec = 0;
    }

    input->nodes[input->nb_nodes - 1].name[0] = 'o';
    input->nodes[input->nb_nodes - 1].name[1] = 'u';
    input->nodes[input->nb_nodes - 1].name[2] = 't';
    input->nodes[input->nb_nodes - 1].name[3] = '\0';
    input->nodes[input->nb_nodes - 1].nb_next = 0;
    input->nodes[input->nb_nodes - 1].next = NULL;

    fsetpos(f, &start);

    for (int i = 0; i < input->nb_nodes - 1; i++)
    {
        int offset = 5; // 3 chars + ':' + ' '
        fgets(buffer, MAX_LINE_LEN, f);
        int nb_splits = 0;
        for (int x = 0; x < (int)strlen(buffer + offset); x++)
            if ((buffer + offset)[x] == ' ')
                nb_splits++;
        input->nodes[i].nb_next = nb_splits + 1;
        input->nodes[i].next = (int *)malloc(sizeof(int) * (input->nodes[i].nb_next));
        for (int j = 0; j < input->nodes[i].nb_next; j++)
        {
            char buf[4];
            // j * 4 is the offset for each name
            sscanf(buffer + offset + j * 4, "%c%c%c", &buf[0], &buf[1], &buf[2]);
            buf[3] = '\0';
            int place_in_array = -1;
            for (int k = 0; k < input->nb_nodes; k++)
            {
                if (!strncmp(input->nodes[k].name, buf, 3))
                {
                    place_in_array = k;
                    break;
                }
            }
            // printf("\n");
            if (place_in_array == -1)
                printf("NO NEIGHBORS FOR NODE - ERROR INPUT\n");
            input->nodes[i].next[j] = place_in_array;
            if (place_in_array >= 0)
                input->nodes[place_in_array].nb_prec++;
        }
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Graph *input = readInput(argv[1], &size);
    run(11, part1, part2, input, (void **)&size);
    for (int i = 0; i < size; i++)
        if (input->nodes[i].nb_next > 0)
            free(input->nodes[i].next);
    free(input->nodes);
    free(input);
    return 0;
}