#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>

// Very slow and memory intensive, with a big chained list and everything, but don't care and get the job done in about 50s. 
// Might come back later, to try to do it with intervals instead of precise points.

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef struct _point {
    int x;
    int y;
    int nb;
    struct _point * suiv;
} Point;

Point *addNewPoint(Point *list, int x, int y) {
    Point *iter = list;
    while (iter) {
        if ((iter->x == x) && (iter->y == y)) {
            iter->nb ++;
            return list;
        }
        iter = iter->suiv;
    }
    Point *new = malloc(sizeof(Point));
    new->x = x;
    new->y = y;
    new->nb = 1;
    new->suiv = list;
    return new;
}

void freeListOfPoints(Point *list) {
    while(list) {
        Point *temp = list->suiv;
        free(list);
        list = temp;
    }
}

Point *readLines(FILE *f, fpos_t *start) {

    fsetpos(f, start);
    Point *list = NULL;
    int x1, y1, x2, y2;
    while (!feof(f)) {
        if(fscanf(f, "%d,%d -> %d,%d\n", &x1, &y1, &x2, &y2) == 4){
            if (x1 == x2) {
                for (int j = MIN(y1, y2); j <= MAX(y1, y2); j++) {
                    list = addNewPoint(list, x1, j);
                }
            } else if (y1 == y2) {
                for (int i = MIN(x1, x2); i <= MAX(x1, x2); i++) {
                    list = addNewPoint(list, i, y1);
                }
            }
        }
    }
    return list;
}


Point *readDiagonals(FILE *f, fpos_t *start, Point *list) {

    Point *newList = list;
    int x1, y1, x2, y2;
    fsetpos(f, start);
    while (!feof(f)) {
        if(fscanf(f, "%d,%d -> %d,%d\n", &x1, &y1, &x2, &y2) == 4){
            if ((x1 != x2) && (y1 != y2) && (abs(x1 - x2) == abs(y1 - y2))) {
                for (int i = 0; i <= abs(x1 - x2); i++){
                    int newX = (x1 < x2) ? x1 + i : x1 - i;
                    int newY = (y1 < y2) ? y1 + i : y1 - i;
                    newList = addNewPoint(newList, newX, newY);
                }
            }
        }
    }
    return newList;
}

int main() {

    clock_t begin = clock();

    FILE *f = fopen("day5.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f){
        printf("FNF");
        return 1;
    }

    Point *list = readLines(f, &start);

    int count = 0;
    Point *iter = list;
    while (iter) {
        if (iter->nb > 1) count++;
        iter = iter->suiv;
    }

    printf("-- Day 5 -- \nThere is %d points that overlap.\n", count);

    list = readDiagonals(f, &start, list);
    int count2 = 0;
    iter = list;
    while (iter) {
        if (iter->nb > 1) {
            count2++;
        }
        iter = iter->suiv;
    }

    printf("There is %d points that overlap, considering diagonals too.\n", count2);

    freeListOfPoints(list);

    clock_t end = clock();
    double time_spend = (double)(end - begin)/CLOCKS_PER_SEC;

    printf("It took %.5f seconds (cpu time) to compute.\n", time_spend);


    return 0;
}