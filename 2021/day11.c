#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define SIZE_X 10
#define SIZE_Y 10
#define MAX_LEN 256

// There was like... no challenge :( in python it would take a lot less than 100 lines to code. It is VERY simple.

int **readInput(FILE *f, fpos_t *start)
{
    // Le tableau est de la forme int[size_y][size_x]
    // printf("%d %d\n", *size_x, *size_y);
    int **tab = (int **)malloc(sizeof(int *) * (SIZE_X));
    for (int i = 0; i < (SIZE_X); i++)
    {
        tab[i] = (int *)malloc(sizeof(int) * (SIZE_Y));
        for (int j = 0; j < (SIZE_Y); j++)
        { // security.
            tab[i][j] = 0;
        }
    }

    fsetpos(f, start);
    char c;
    int x = 0, y = 0;
    while (!feof(f))
    {
        c = fgetc(f);
        if (c != '\n' && c != '\0' && c != EOF)
        {
            tab[y][x] = atoi(&c);
            x++;
        }
        else
        {
            y++;
            x = 0;
        }
    }
    return tab;
}

void freeInput(int **tab, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(tab[i]);
    }
    free(tab);
}

typedef struct _point
{
    int x;
    int y;
} Point;

void flash(int **world, Point flashing)
{
    for (int i = (flashing.x == 0 ? flashing.x : flashing.x - 1); i <= (flashing.x == SIZE_X - 1 ? flashing.x : flashing.x + 1); i++)
    {
        for (int j = (flashing.y == 0 ? flashing.y : flashing.y - 1); j <= (flashing.y == SIZE_Y - 1 ? flashing.y : flashing.y + 1); j++)
        {
            if (i != flashing.x || j != flashing.y)
            {
                world[i][j]++;
            }
        }
    }
}

int isPointIn(Point tab[], Point p, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (tab[i].x == p.x && tab[i].y == p.y)
        {
            return 1;
        }
    }
    return 0;
}

int step(int **world)
{
    // First, increase everyone by one.
    for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            world[i][j]++;
        }
    }
    // Then flash until everyone flashed.
    Point flashed[SIZE_X * SIZE_Y];
    int nbFlashed = 0;
    int someoneFlashed = 1;
    while (someoneFlashed)
    {
        someoneFlashed = 0;
        for (int i = 0; i < SIZE_X; i++)
        {
            for (int j = 0; j < SIZE_Y; j++)
            {
                Point test = (Point){i, j};
                if ((world[i][j] > 9) && (!isPointIn(flashed, test, nbFlashed)))
                {
                    flash(world, test);
                    flashed[nbFlashed] = test; // Making sure we don't flash two times.
                    nbFlashed++;
                    someoneFlashed = 1; // Someone flashed, need to redo it
                }
            }
        }
    }
    // Then put back to 0
    for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (world[i][j] > 9)
            {
                world[i][j] = 0;
            }
        }
    }
    return nbFlashed;
}

int main()
{

    clock_t begin_time = clock();

    FILE *f = fopen("day11.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    int **world = readInput(f, &start);

    /* for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            printf("%d", world[i][j]);
        }
        printf("\n");
    } */

    int somme = 0;
    int nbOfSteps = 1;
    while (nbOfSteps <= 100)
    {
        somme += step(world);
        nbOfSteps++;
    }
    printf("-- Day 11 -- \nAfter 100 days, number of flashes : %d\n", somme);

    while (step(world) != SIZE_X * SIZE_Y)
    {
        nbOfSteps++;
    }
    printf("They all flash simultaneously at step %d.\n", nbOfSteps);

    freeInput(world, SIZE_X);

    fclose(f);
    clock_t end_time = clock();
    double time_spend = (double)(end_time - begin_time) / CLOCKS_PER_SEC;

    printf("It took %.5f seconds (cpu time) to compute.\n\n\n", time_spend);

    return 0;
}