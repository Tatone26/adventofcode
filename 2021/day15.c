#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>

#define MAX_LEN 256
#define POS(X, Y, SIZEX) (Y) * SIZEX + X

// Increase this if everything explodes ! It may be because the smallest path is higher than this.
#define MAX_SIZE_BUCKET_QUEUE 5000

typedef struct _point
{
    int x;
    int y;
} Point;

typedef struct _linked_point
{
    Point p;
    struct _linked_point *next;
} LinkedPoint;

// It was HARD to find a fast solution. I had to go through a LOT of iterations with TOTALLY DIFFERENT data structures.
// I tried 2D array (~2s), LinkedList with sort at extraction (~1s) or insertion (~0.3s), BinaryLinkedList (~0.15s) and finally... BUCKET QUEUE ! (0.08s)
// As often, big array is the solution !
// I keep track of the index of the first non-null element, which represent the lowest score, so the one that have priority in the path finding algo.
// That way, I have a O(1) time to find the list of elements that have the lowest score.
// Just need to take care of the case where multiple points have the same score, and there we go !
// Only downside : need to have a list big enough, but it shouldn't be a problem since it's just a list of pointers...
typedef struct _bucket_queue
{
    LinkedPoint **array;
    int size;
    int smallest;
} BucketQueue;

char **readInput(FILE *f, fpos_t *start, int *size_x, int *size_y)
{
    // Le tableau est de la forme int[size_y][size_x]
    int sizex = 0, sizey = 0;
    char buffer[MAX_LEN];
    fgets(buffer, MAX_LEN, f);

    sizex = strlen(buffer) - 1;
    while (!feof(f))
    {
        sizey++;
        fgets(buffer, MAX_LEN, f);
    }
    *size_x = sizex;
    *size_y = sizey;
    char **tab = (char **)malloc(sizeof(char *) * (sizey));
    for (int i = 0; i < (sizey); i++)
    {
        tab[i] = (char *)calloc(sizex, sizeof(char));
    }
    fsetpos(f, start);
    for (int y = 0; y < sizey; y++)
    {
        fgets(buffer, MAX_LEN, f);
        for (int x = 0; x < sizex; x++)
        {
            char c = buffer[x];
            tab[y][x] = atoi(&c);
        }
    }
    return tab;
}

void freeInput(char **tab, const Point *size)
{
    for (int i = 0; i < size->y; i++)
    {
        free(tab[i]);
    }
    free(tab);
}

void freeLinkedPoint(LinkedPoint *queue)
{
    if (queue == NULL)
        return;
    freeLinkedPoint(queue->next);
    free(queue);
}

void freeBucketQueue(BucketQueue *activePoints)
{
    for (int i = 0; i < activePoints->size; i++)
    {
        freeLinkedPoint(activePoints->array[i]);
    }
    free(activePoints->array);
}

void insertNewPoint(BucketQueue *activePoints, Point point, int score)
// BucketQueue allows for blazingly fast insertion.
{
    LinkedPoint *new = malloc(sizeof(LinkedPoint));
    *new = (LinkedPoint){point, NULL};
    // Update the smallest score.
    if (score < activePoints->smallest)
    {
        activePoints->smallest = score;
    }
    // Last-in First-out is the best in this scenario, because if you are the last in, you may be already closer to the goal...
    // And it cut the time by more than half from first-in first-out, and I still have the right answer. Letsgoo.
    new->next = activePoints->array[score];
    activePoints->array[score] = new;
}

Point nextToMove2(BucketQueue *activePoints)
// But does it allow fast extraction as well ? -> HELL YEAH
{
    // Find the new point
    int smallestScore = activePoints->smallest;
    Point newPoint = activePoints->array[smallestScore]->p;

    // Update the bucketList (put the second one in this place)
    LinkedPoint *temp = activePoints->array[smallestScore];
    activePoints->array[smallestScore] = activePoints->array[smallestScore]->next;
    // Don't forget to get rid of the returned point
    free(temp);

    int j;
    for (j = smallestScore; activePoints->array[j] == NULL && j < activePoints->size; j++)
        ;
    activePoints->smallest = j; // worse case scenario : the smallest is now the size of the array, so any new points will take this place. Or problem in pathfinding.

    return newPoint;
}

int cost(int *score, int x, int y, const Point *size)
// Minimum has priority.
{
    return score[POS(x, y, size->x)];
}

void moveOneTurn2(const char **input, BucketQueue *activePoints, int *score, const Point *size)
{
    Point next = nextToMove2(activePoints);
    int newScore;
    int oldScore = score[POS(next.x, next.y, size->x)];
    if (next.x == 0 && next.y == 0) // We don't want to count the entrance score. 
        oldScore = 0;
    // For the order of the next few things : since we want to go to the bottom right corner, the down and right movements should be prioritized.
    // Because of the bucket list last-in first-out implementation, putting those one AFTER the up and left ones gives priority to down and right. 
    // Probably a really small bonus. 
    if (next.y > 0)
    {
        newScore = oldScore + input[next.y - 1][next.x];
        if (newScore < score[POS(next.x, next.y - 1, size->x)])
        {
            score[POS(next.x, next.y - 1, size->x)] = newScore;
            insertNewPoint(activePoints, (Point){next.x, next.y - 1}, cost(score, next.x, next.y - 1, size));
        }
    }
    if (next.x > 0)
    {
        newScore = oldScore + input[next.y][next.x - 1];
        if (newScore < score[POS(next.x - 1, next.y, size->x)])
        {
            score[POS(next.x - 1, next.y, size->x)] = newScore;
            insertNewPoint(activePoints, (Point){next.x - 1, next.y}, cost(score, next.x - 1, next.y, size));
        }
    }
    if (next.y < size->y - 1)
    {
        newScore = oldScore + input[next.y + 1][next.x];
        if (newScore < score[POS(next.x, next.y + 1, size->x)])
        {
            score[POS(next.x, next.y + 1, size->x)] = newScore;
            insertNewPoint(activePoints, (Point){next.x, next.y + 1}, cost(score, next.x, next.y + 1, size));
        }
    }
    if (next.x < size->x - 1)
    {
        newScore = oldScore + input[next.y][next.x + 1];
        if (newScore < score[POS(next.x + 1, next.y, size->x)])
        {
            score[POS(next.x + 1, next.y, size->x)] = newScore;
            insertNewPoint(activePoints, (Point){next.x + 1, next.y}, cost(score, next.x + 1, next.y, size));
        }
    }
}

int main()
{
    FILE *f = fopen("inputs/day15.txt", "r");
    if (!f || f == NULL)
    {
        printf("FNF");
        return 1;
    }
    fpos_t start;
    fgetpos(f, &start);

    int sizex, sizey;
    const char **input = (const char **)readInput(f, &start, &sizex, &sizey);
    Point size = (Point){sizex, sizey};
    /* for (int y = 0; y < sizey; y++)
    {
        for (int x = 0; x < sizex; x++)
        {
            printf("%d", input[y][x]);
        }
        printf("\n");
    } */
    int score[sizey][sizex];
    for (int y = 0; y < sizey; y++)
    {
        for (int x = 0; x < sizex; x++)
        {
            score[y][x] = INT_MAX;
        }
    }
    score[0][0] = 0;
    LinkedPoint **array1 = calloc(MAX_SIZE_BUCKET_QUEUE, sizeof(LinkedPoint));
    BucketQueue activePoints = (BucketQueue){array1, MAX_SIZE_BUCKET_QUEUE, 0};
    insertNewPoint(&activePoints, (Point){0, 0}, 0);
    // score[size.y - 1][size.x - 1] = 0;
    while (score[size.y - 1][size.x - 1] == INT_MAX) // here, it stops as soon as it finds a path. But maybe it is not the best... (wait, what ?)
    {
        moveOneTurn2(input, &activePoints, (int *)score, &size);
        /* for (int y = 0; y < sizey; y++) {
            for (int x = 0; x < sizex; x++) {
                printf("%d ", activePoints[y][x]);
            }
            printf("\n");
        }
        printf("\n"); */
    }
    printf("-- Day 15 --\nLowest total risk : %d\n", score[size.y - 1][size.x - 1]);


    // Lets create the new input. m
    Point newSize = (Point){sizex * 5, sizey * 5};

    char **input_temp = malloc(sizeof(char *) * newSize.y);
    for (int y = 0; y < newSize.y; y++)
    {
        input_temp[y] = malloc(sizeof(char) * newSize.x);
        for (int x = 0; x < newSize.x; x++)
        {
            int decalage = y / sizey + x / sizex;
            input_temp[y][x] = (input[y % sizey][x % sizex] + decalage - 1) % 9 + 1;
        }
    }
    /* for (int i = 0; i < newSize.y; i++) {
        for (int j = 0; j < newSize.x; j++) {
            printf("%d", input_temp[i][j]);
        }
        printf("\n");
    } */
    const char **input2 = (const char **)input_temp; // nope
    int score2[newSize.y][newSize.x];
    for (int y = 0; y < newSize.y; y++)
    {
        for (int x = 0; x < newSize.x; x++)
        {
            score2[y][x] = INT_MAX;
        }
    }
    score2[0][0] = 0;
    LinkedPoint **array2 = calloc(MAX_SIZE_BUCKET_QUEUE, sizeof(LinkedPoint));
    BucketQueue activePoints2 = (BucketQueue){array2, MAX_SIZE_BUCKET_QUEUE, 0};
    insertNewPoint(&activePoints2, (Point){0, 0}, 0);
    // score[size.y - 1][size.x - 1] = 0;
    while (score2[newSize.y - 1][newSize.x - 1] == INT_MAX) // here, it stops as soon as it finds a path. But maybe it is not the best... (wait, what ?)
    {
        moveOneTurn2(input2, &activePoints2, (int *)score2, &newSize);
    }
    printf("Lowest total risk in 5*5 grid : %d\n", score2[newSize.y - 1][newSize.x - 1]);
    // Freeing everything
    freeInput((char **)input, &size);
    freeBucketQueue(&activePoints);
    freeBucketQueue(&activePoints2);
    freeInput((char **)input2, &newSize);
    fclose(f);
    return 0;
}