#include "runner.h"

#define PARENT(i) ((i - 1) / 2)
#define LEFT(i) (2 * i + 1)
#define RIGHT(i) (2 * i + 2)

const Pos DIRECTIONS[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

typedef struct
{
    int cost;
    Pos position;
    int direction;
} Node;

typedef struct
{
    int size;
    int capacity;
    int nb_insertions;
    Node *nodes;
} Heap;

Heap newHeap(int capacity)
{
    Heap heap;
    heap.size = 0;
    heap.capacity = capacity;
    heap.nb_insertions = 0;
    heap.nodes = (Node *)malloc(sizeof(Node) * capacity);
    return heap;
}

void swap(Node *n, Node *m)
{
    Node temp = *n;
    *n = *m;
    *m = temp;
}

void updateHeap(Heap *heap, int i)
{
    int l = LEFT(i);
    int r = RIGHT(i);
    int smallest = i;
    if (l < heap->size && heap->nodes[l].cost < heap->nodes[i].cost)
        smallest = l;
    if (r < heap->size && heap->nodes[r].cost < heap->nodes[i].cost)
        smallest = r;
    if (smallest != i)
    {
        swap(&heap->nodes[i], &heap->nodes[smallest]);
        updateHeap(heap, smallest);
    }
}

Node extractMin(Heap *heap)
{
    if (heap->size == 0)
        return (Node){__INT_MAX__, (Pos){0, 0}, 0};
    if (heap->size == 1)
    {
        heap->size--;
        return heap->nodes[0];
    }
    Node root = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    updateHeap(heap, 0);
    return root;
}

void insertNode(Heap *heap, Node node)
{
    if (heap->size == heap->capacity)
    {
        printf("overflow heap\n");
        return;
    }
    heap->nb_insertions++;
    int i = heap->size++;
    heap->nodes[i] = node;
    while (i != 0 && heap->nodes[PARENT(i)].cost > heap->nodes[i].cost)
    {
        swap(&heap->nodes[i], &heap->nodes[PARENT(i)]);
        i = PARENT(i);
    }
}

luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    Pos start = (Pos){1, height - 2};
    Pos end = (Pos){width - 2, 1};
    // used to know if we have already found the best way of going there
    int(*seen)[4] = (int(*)[4])malloc(width * height * sizeof(int[4]));
    memset(seen, 127, width * height * sizeof(int[4]));

    Heap heap = newHeap(width * height);
    Node s = {0, start, 0};
    insertNode(&heap, s);

    int toTest[3] = {0, 1, 3};
    int valToTest[3] = {0, 1000, 1000};

    Node current = extractMin(&heap);
    while (current.position.x != end.x || current.position.y != end.y)
    {
        for (int d = 0; d < 3; d++)
        {
            int direction = (current.direction + toTest[d]) % 4;
            Pos dir = DIRECTIONS[direction];
            Pos next = (Pos){current.position.x + dir.x, current.position.y + dir.y};
            if (input[INDEX_POS(next)] != '#' && current.cost + valToTest[d] < seen[INDEX_POS(next)][direction] - (d == 0 ? 1 : 0))
            {
                Node new = {current.cost + valToTest[d] + 1, next, direction};
                insertNode(&heap, new);
                seen[INDEX_POS(next)][direction] = current.cost + valToTest[d] + 1;
            }
        }
        current = extractMin(&heap);
        if (current.cost == __INT_MAX__)
        {
            break;
        }
    }

    // printf("nb insertions : %d\n", heap.nb_insertions);

    free(heap.nodes);
    free(seen);
    return current.cost;
}

// -----------------------------------------------------------------

int updatePath(int (*seen)[4], int width, int height, Pos current, Pos start, bool *counted, int direction)
{
    if (counted[INDEX_POS(current)])
        return 0;
    counted[INDEX_POS(current)] = true;
    if (current.x == start.x && current.y == start.y)
    {
        // printf("end\n");
        return 1;
    }
    int min = seen[0][0];
    for (int d = 0; d < 4; d++)
    {
        if (seen[INDEX_POS(current)][d] < min)
            min = seen[INDEX_POS(current)][d];
    }
    if (min == seen[0][0])
        return 0;
    int total = 1;
    for (int d = 0; d < 4; d++)
    {
        Pos next = (Pos){current.x + DIRECTIONS[(d + 2) % 4].x, current.y + DIRECTIONS[(d + 2) % 4].y};
        if (seen[INDEX_POS(current)][d] == min || (d % 2 == direction % 2 && seen[INDEX_POS(current)][d] == min + 1000))
        {
            total += updatePath(seen, width, height, next, start, counted, d);
        }
    }
    return total;
}

luint part2(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int width = ((int *)args)[0];
    int height = ((int *)args)[1];

    Pos start = (Pos){1, height - 2};
    Pos end = (Pos){width - 2, 1};
    // used to know if we have already found the best way of going there
    int(*seen)[4] = (int(*)[4])malloc(width * height * sizeof(int[4]));
    memset(seen, 127, width * height * sizeof(int[4]));

    Heap heap = newHeap(width * height);
    Node s = {0, start, 0};
    insertNode(&heap, s);

    int toTest[3] = {0, 1, 3};
    int valToTest[3] = {0, 1000, 1000};

    Node current = extractMin(&heap);
    while (true)
    {
        for (int d = 0; d < 3; d++)
        {
            int direction = (current.direction + toTest[d]) % 4;
            Pos dir = DIRECTIONS[direction];
            Pos next = (Pos){current.position.x + dir.x, current.position.y + dir.y};
            if (input[INDEX_POS(next)] != '#' && current.cost + valToTest[d] < seen[INDEX_POS(next)][direction] - (d == 0 ? 1 : 0))
            {
                Node new = {current.cost + valToTest[d] + 1, next, direction};
                insertNode(&heap, new);
                seen[INDEX_POS(next)][direction] = current.cost + valToTest[d] + 1;
            }
        }
        current = extractMin(&heap);
        if (current.cost == __INT_MAX__)
        {
            break;
        }
    }
    // printf("nb insertions (%d %d) : %d\n", width, height, heap.nb_insertions);
    bool *counted = (bool *)calloc(width * height, sizeof(bool));
    luint res = updatePath(seen, width, height, end, start, counted, true);

    free(counted);
    free(heap.nodes);
    free(seen);
    return res;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *width, int *height)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *height = fileSize(f);
    fpos_t start;
    fgetpos(f, &start);
    fgets(buffer, MAX_LINE_LEN, f);
    *width = strlen(buffer) - 1;
    fsetpos(f, &start);
    char *input = (char *)malloc(sizeof(char) * *width * *height);

    for (int i = 0; i < *height; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        strncpy(input + i * *width, buffer, *width);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int sizes[2] = {0, 0};
    char *input = readInput(argv[1], &sizes[0], &sizes[1]);
    run(16, part1, part2, input, (void **)&sizes);
    free(input);
    return 0;
}