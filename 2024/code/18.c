#include "runner.h"

#define WORLD_SIZE 71 // 7 for test, 71 for input

#define PARENT(i) ((i - 1) / 2)
#define LEFT(i) (2 * i + 1)
#define RIGHT(i) (2 * i + 2)

const Pos DIRECTIONS[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

// tried to use some kind of A* but it doesn't seem to change anything here ?
typedef struct
{
    int cost;
    int heuristic;
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
    if (l < heap->size && heap->nodes[l].heuristic < heap->nodes[i].heuristic)
        smallest = l;
    if (r < heap->size && heap->nodes[r].heuristic < heap->nodes[i].heuristic)
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
        return (Node){__INT_MAX__, __INT_MAX__, (Pos){0, 0}, 0};
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
    while (i != 0 && heap->nodes[PARENT(i)].heuristic > heap->nodes[i].heuristic)
    {
        swap(&heap->nodes[i], &heap->nodes[PARENT(i)]);
        i = PARENT(i);
    }
}

// -----------------------------------------------------------------

luint part1(void *input_v, void **args)
{
    Pos *input = (Pos *)input_v;
    int size = ((int *)args)[0];

    int width = WORLD_SIZE, height = WORLD_SIZE;

    char world[WORLD_SIZE * WORLD_SIZE] = {0};
    for (int i = 0; i < (size < 1024 ? 12 : 1024); i++)
        world[INDEX_POS(input[i])] = '#';

    Pos start = (Pos){0, 0};
    Pos end = (Pos){width - 1, height - 1};
    // used to know if we have already found the best way of going there
    int(*seen)[4] = (int(*)[4])malloc(width * height * sizeof(int[4]));
    memset(seen, 127, width * height * sizeof(int[4]));

    Heap heap = newHeap(width * height);
    Node s = {0, end.x + end.y, start, 0};
    insertNode(&heap, s);

    Node current = extractMin(&heap);
    while (current.position.x != end.x || current.position.y != end.y)
    {
        for (int d = 0; d < 4; d++)
        {
            int direction = (current.direction + d) % 4;
            Pos dir = DIRECTIONS[direction];
            Pos next = (Pos){current.position.x + dir.x, current.position.y + dir.y};
            int nextHeuristic = current.cost + 1 + (end.x - next.x) + (end.y - next.y);
            if (INBOUNDS(next, width, height) && world[INDEX_POS(next)] != '#' && nextHeuristic < seen[INDEX_POS(next)][direction])
            {
                Node new = {current.cost + 1, nextHeuristic, next, direction};
                insertNode(&heap, new);
                seen[INDEX_POS(next)][direction] = nextHeuristic;
            }
        }
        current = extractMin(&heap);
        if (current.cost == __INT_MAX__)
        {
            break;
        }
    }
    luint res = current.cost;

    free(heap.nodes);
    free(seen);
    return res;
}

// ---------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    Pos *input = (Pos *)input_v;
    int size = ((int *)args)[0];

    int width = WORLD_SIZE, height = WORLD_SIZE;

    char world[WORLD_SIZE * WORLD_SIZE] = {0};
    for (int i = 0; i < (size < 1024 ? 12 : 1024); i++) // for testing : only the first 12 bytes.
        world[INDEX_POS(input[i])] = '#';

    int(*seen)[4] = (int(*)[4])malloc(width * height * sizeof(int[4]));
    Heap heap = newHeap(width * height);

    int min = (size < 1024 ? 12 : 1024) - 1; // with Part 1, we know that the first 1024 bytes (or 12 for testing) are not blocking the

    int upBound = size - 1;
    int downBound = min;
    int currentTest = downBound + (upBound - downBound) / 2;
    for (int d = min; d <= currentTest; d++)
        world[INDEX_POS(input[d])] = '#';
    while (downBound <= upBound)
    {
        currentTest = downBound + (upBound - downBound) / 2;
        // Not the most efficient way of updating the world, but it works and it is still faster than part 1.
        for (int d = currentTest + 1; d < size; d++)
            world[INDEX_POS(input[d])] = 0;
        for (int d = min; d <= currentTest; d++)
            world[INDEX_POS(input[d])] = '#';
        bool foundExit = true;

        Pos start = (Pos){0, 0};
        Pos end = (Pos){width - 1, height - 1};
        // reset
        memset(seen, 127, width * height * sizeof(int[4]));
        heap.size = 0;

        Node s = {0, end.x + end.y, start, 0};
        insertNode(&heap, s);

        Node current = extractMin(&heap);
        while (current.position.x != end.x || current.position.y != end.y)
        {
            for (int d = 0; d < 4; d++)
            {
                int direction = (current.direction + d) % 4;
                Pos dir = DIRECTIONS[direction];
                Pos next = (Pos){current.position.x + dir.x, current.position.y + dir.y};
                // this heuristic serves no purpose apparently (and that would be logical)
                int nextHeuristic = current.cost + 1 + (end.x - next.x) + (end.y - next.y);
                if (INBOUNDS(next, width, height) && world[INDEX_POS(next)] != '#' && nextHeuristic < seen[INDEX_POS(next)][direction])
                {
                    Node new = {current.cost + 1, nextHeuristic, next, direction};
                    insertNode(&heap, new);
                    seen[INDEX_POS(next)][direction] = nextHeuristic;
                }
            }
            current = extractMin(&heap);
            if (current.cost == __INT_MAX__)
            {
                foundExit = false;
                break;
            }
        }
        if (!foundExit) //  current is too big
            upBound = currentTest - 1;
        else // current is too low
            downBound = currentTest + 1;
    }
    printf("Answer Part 2 : %lld,%lld\n", input[currentTest].x, input[currentTest].y);
    free(heap.nodes);
    free(seen);
    return currentTest;
}

// ----------------------------------------------------------------

Pos *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *size = fileSize(f);
    Pos *input = (Pos *)malloc(sizeof(Pos) * *size);

    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        int a, b;
        if (sscanf(buffer, "%d,%d", &a, &b) != 2)
            printf("ERROR READING INPUT\n");
        input[i] = (Pos){a, b};
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Pos *input = readInput(argv[1], &size);
    run(18, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}