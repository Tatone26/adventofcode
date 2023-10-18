#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define MAX_LEN 256

int **readInput(FILE *f, fpos_t *start, int *size_x, int *size_y)
{
    char buffer[MAX_LEN];
    fsetpos(f, start);
    fgets(buffer, MAX_LEN, f);
    *size_x = strlen(buffer) - 1;
    *size_y = 0; // WARNING : need to have an empty line after the tab
    while (!feof(f))
    {
        (*size_y)++;
        fgets(buffer, MAX_LEN, f);
    }
    // Le tableau est de la forme int[size_y][size_x]
    // printf("%d %d\n", *size_x, *size_y);
    int **tab = malloc(sizeof(int *) * (*size_y));
    for (int i = 0; i < (*size_y); i++)
    {
        tab[i] = malloc(sizeof(int) * (*size_x));
        for (int j = 0; j < (*size_x); j++)
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

Point *listOfLowPoints(int **inputs, int size_x, int size_y, int number_of_low_points)
{
    Point *liste = malloc(sizeof(Point) * number_of_low_points);
    int iter = 0;
    for (int i = 0; i < size_y; i++)
    {
        for (int j = 0; j < size_x; j++)
        {
            int height = inputs[i][j];
            if ((j - 1) >= 0)
            { // if not on the left rim
                if (inputs[i][j - 1] <= height)
                    continue;
                if (i - 1 >= 0 && inputs[i - 1][j - 1] <= height)
                    continue;
                if (i + 1 < size_y && inputs[i + 1][j - 1] <= height)
                    continue;
            }
            if ((j + 1) < size_x)
            { // if not on the right rim
                if (inputs[i][j + 1] < height)
                    continue;
                if (i - 1 >= 0 && inputs[i - 1][j + 1] <= height)
                    continue;
                if (i + 1 < size_y && inputs[i + 1][j + 1] <= height)
                    continue;
            }
            if (i - 1 >= 0 && inputs[i - 1][j] <= height)
                continue;
            if (i + 1 < size_y && inputs[i + 1][j] <= height)
                continue;
            liste[iter] = (Point){j, i};
            // printf("%d, %d\n", liste[iter].x, liste[iter].y);
            iter++;
            if (iter > number_of_low_points)
                break;
        }
        if (iter > number_of_low_points)
            break;
    }
    return liste;
}

int measureBasinFromLowPoint(int **inputs, int size_x, int size_y, Point lowPoint)
{
    int basinSize = 1;
    Point temp[size_y * size_x]; // Just to make sure.
    temp[0] = lowPoint;

    Point directions[4] = {(Point){0, 1}, (Point){0, -1}, (Point){1, 0}, (Point){-1, 0}};
    int i = 0;
    while (i < basinSize)
    { // Pour chaque point dans notre tableau de recherche. -> il faudrait optimiser en ne cherchant pas 2 fois sur le même point.
        for (int d = 0; d < 4; d++)
        { // Pour toutes les directions :
            int newX = temp[i].x + directions[d].x;
            int newY = temp[i].y + directions[d].y;
            if (0 > newX || size_x <= newX || 0 > newY || size_y <= newY)
                continue; // if sorti du tableau.
            if (inputs[newY][newX] == 9)
                continue; // on ne compte pas les 9.
            if (inputs[newY][newX] > inputs[temp[i].y][temp[i].x])
            {
                int alreadyThere = 0;
                for (int j = 0; j < basinSize; j++)
                {
                    if (temp[j].x == newX && temp[j].y == newY)
                    {
                        alreadyThere = 1;
                        break;
                    }
                }
                if (!alreadyThere)
                {
                    temp[basinSize] = (Point){newX, newY}; // Cela ne gère que les bassins avec un seul lowPooint, séparé des autres lowPoints par des 9.
                    basinSize++;
                }
            }
        }
        i++;
    }
    return basinSize;
}

int sumRiskLowPoint(int **inputs, int size_x, int size_y, int *number_of_low_points)
{
    (*number_of_low_points) = 0;
    int sum = 0;
    for (int i = 0; i < size_y; i++)
    {
        for (int j = 0; j < size_x; j++)
        {
            int height = inputs[i][j];
            if ((j - 1) >= 0)
            { // if not on the left rim
                if (inputs[i][j - 1] <= height)
                    continue;
                if (i - 1 >= 0 && inputs[i - 1][j - 1] <= height)
                    continue;
                if (i + 1 < size_y && inputs[i + 1][j - 1] <= height)
                    continue;
            }
            if ((j + 1) < size_x)
            { // if not on the right rim
                if (inputs[i][j + 1] < height)
                    continue;
                if (i - 1 >= 0 && inputs[i - 1][j + 1] <= height)
                    continue;
                if (i + 1 < size_y && inputs[i + 1][j + 1] <= height)
                    continue;
            }
            if (i - 1 >= 0 && inputs[i - 1][j] <= height)
                continue;
            if (i + 1 < size_y && inputs[i + 1][j] <= height)
                continue;
            sum += inputs[i][j] + 1;
            (*number_of_low_points)++;
        }
    }
    return sum;
}

int main()
{

    clock_t begin_time = clock();

    FILE *f = fopen("day9.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    int size_x, size_y;
    // int[size_y][size_x]
    int **inputs = readInput(f, &start, &size_x, &size_y);
    /* for (int i = 0; i < size_y; i++) {
        for (int j = 0; j < size_x; j++) {
            printf("%d", inputs[i][j]);
        }
        printf("\n");
    } */
    int number_of_low_points; // for part 2.
    printf("-- Day 9 --\nSum of risks levels of low points : %d\n", sumRiskLowPoint(inputs, size_x, size_y, &number_of_low_points));

    Point *listeLowPoints = listOfLowPoints(inputs, size_x, size_y, number_of_low_points);
    int max[3];
    max[0] = 0;
    max[1] = 1;
    max[2] = 2;
    for (int i = 0; i < number_of_low_points; i++)
    {
        int size = measureBasinFromLowPoint(inputs, size_x, size_y, listeLowPoints[i]);
        if (size >= max[0]) {
            max[2] = max[1];
            max[1] = max[0];
            max[0] = size;
        } else if (size >= max[1]){
            max[2] = max[1];
            max[1] = size;
        } else if (size >= max[2]) {
            max[2] = size;
        }
    }

    printf("Multiplication of three largests basins (%d, %d, %d) : %d\n", max[0], max[1], max[2], max[0]*max[1]*max[2]);

    free(listeLowPoints);
    freeInput(inputs, size_y);

    fclose(f);
    clock_t end_time = clock();
    double time_spend = (double)(end_time - begin_time) / CLOCKS_PER_SEC;

    printf("It took %.5f seconds (cpu time) to compute.\n\n\n", time_spend);

    return 0;
}