#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>

#define MAX_LEN 256
#define POS(X, Y, SIZEX) (Y) * SIZEX + X

typedef struct _point
{
    int x;
    int y;
} Point;

typedef struct _list_of_points
{
    Point p;
    struct _list_of_points *next;
} pointsList;

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

void freeInput(int **tab, const Point *size)
{
    for (int i = 0; i < size->y; i++)
    {
        free(tab[i]);
    }
    free(tab);
}

Point nextToMove2(pointsList **toDo, int *score, const Point *size)
// second strategy : chained list.
{
    pointsList *temp = *toDo;
    Point minPoint;
    int minScore = INT_MAX;
    while (temp != NULL)
    {
        int newScore = score[POS(temp->p.x, temp->p.y, size->x)] + (temp->p.y + temp->p.x) * (temp->p.y + temp->p.x);
        if (newScore < minScore)
        {
            minPoint = temp->p;
            minScore = newScore;
        }
        temp = temp->next;
    }
    temp = *toDo;
    pointsList *temp_before = NULL;
    while (temp != NULL)
    {
        if (temp->p.x == minPoint.x && temp->p.y == minPoint.y)
        {
            if (temp_before == NULL)
            {
                // this means that temp is the first, so temp = *toDO
                temp_before = (*toDo)->next;
                free(*toDo);
                *toDo = temp_before;
                break;
            }
            else
            {
                temp_before->next = temp->next;
                free(temp);
                break;
            }
        }
        temp_before = temp;
        temp = temp->next;
    }

    return minPoint;
}

void moveOneTurn2(const char **input, pointsList **toDo, int *score, const Point *size)
{
    Point next = nextToMove2(toDo, score, size);
    int newScore;
    int oldScore = score[POS(next.x, next.y, size->x)];
    if (next.x == 0 && next.y == 0)
        oldScore = 0;
    if (next.x < size->x - 1)
    {
        newScore = oldScore + input[next.y][next.x + 1];
        if (newScore < score[POS(next.x + 1, next.y, size->x)])
        {
            score[POS(next.x + 1, next.y, size->x)] = newScore;
            pointsList *temp = malloc(sizeof(pointsList));
            *temp = (pointsList){(Point){next.x + 1, next.y}, *toDo};
            *toDo = temp;
        }
    }
    if (next.y < size->y - 1)
    {
        newScore = oldScore + input[next.y + 1][next.x];
        if (newScore < score[POS(next.x, next.y + 1, size->x)])
        {
            score[POS(next.x, next.y + 1, size->x)] = newScore;
            pointsList *temp = malloc(sizeof(pointsList));
            *temp = (pointsList){(Point){next.x, next.y + 1}, *toDo};
            *toDo = temp;
        }
    }
    if (next.y > 0)
    {
        newScore = oldScore + input[next.y - 1][next.x];
        if (newScore < score[POS(next.x, next.y - 1, size->x)])
        {
            score[POS(next.x, next.y - 1, size->x)] = newScore;
            pointsList *temp = malloc(sizeof(pointsList));
            *temp = (pointsList){(Point){next.x, next.y - 1}, *toDo};
            *toDo = temp;
        }
    }
    if (next.x > 0)
    {
        newScore = oldScore + input[next.y][next.x - 1];
        if (newScore < score[POS(next.x - 1, next.y, size->x)])
        {
            score[POS(next.x - 1, next.y, size->x)] = newScore;
            pointsList *temp = malloc(sizeof(pointsList));
            *temp = (pointsList){(Point){next.x - 1, next.y}, *toDo};
            *toDo = temp;
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
    pointsList *toDo2 = malloc(sizeof(pointsList));
    *toDo2 = (pointsList){(Point){0, 0}, NULL};
    // score[size.y - 1][size.x - 1] = 0;
    while (score[size.y - 1][size.x - 1] == INT_MAX) // here, it stops as soon as it finds a path. But maybe it is not the best... (wait, what ?)
    {
        moveOneTurn2(input, &toDo2, (int *)score, &size);
        /* for (int y = 0; y < sizey; y++) {
            for (int x = 0; x < sizex; x++) {
                printf("%d ", toDo[y][x]);
            }
            printf("\n");
        }
        printf("\n"); */
    }
    printf("-- Day 15 --\nLowest total risk : %d\n", score[size.y - 1][size.x - 1]);

    Point newSize = (Point){sizex * 5, sizey * 5};

    char **input_temp = malloc(sizeof(char *) * newSize.y);
    for (int y = 0; y < newSize.y; y++)
    {
        input_temp[y] = malloc(sizeof(char) * newSize.x);
        for (int x = 0; x < newSize.x; x++)
        {
            int decalage = y/sizey + x/sizex;
            input_temp[y][x] = (input[y%sizey][x%sizex] + decalage - 1) % 9 + 1 ;
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
    pointsList *toDo2_2 = malloc(sizeof(pointsList));
    *toDo2_2 = (pointsList){(Point){0, 0}, NULL};
    // score[size.y - 1][size.x - 1] = 0;
    while (score2[newSize.y - 1][newSize.x - 1] == INT_MAX) // here, it stops as soon as it finds a path. But maybe it is not the best... (wait, what ?)
    {
        moveOneTurn2(input2, &toDo2_2, (int *)score2, &newSize);
    }
    printf("Lowest total risk in 5*5 grid : %d\n", score2[newSize.y - 1][newSize.x - 1]);
    // Freeing everything
    freeInput((int **)input, &size);
    while (toDo2 != NULL)
    {
        pointsList *temp = toDo2->next;
        free(toDo2);
        toDo2 = temp;
    }
    freeInput((int **)input2, &newSize);
    while (toDo2_2 != NULL)
    {
        pointsList *temp = toDo2_2->next;
        free(toDo2_2);
        toDo2_2 = temp;
    }
    fclose(f);
    return 0;
}