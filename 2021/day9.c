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
    while (!feof(f)) {
        (*size_y) ++;
        fgets(buffer, MAX_LEN, f);
    }
    // Le tableau est de la forme int[size_y][size_x]
    printf("%d %d\n", *size_x, *size_y);
    int **tab = malloc(sizeof(int *) * (*size_y));
    for (int i = 0; i < (*size_y); i++) {
        tab[i] = malloc(sizeof(int) * (*size_x));
        for (int j = 0; j < (*size_x); j++) { //security.
            tab[i][j] = 0;
        }
    }

    fsetpos(f, start);
    char c;
    int x = 0, y = 0;
    while (!feof(f)) {
        c = fgetc(f);
        if (c != '\n' && c != '\0' && c != EOF) {
            tab[y][x] = atoi(&c);
            x++;
        } else {
            y++;
            x = 0;
        }
    }
    return tab;
}

void freeInput(int **tab, int size) {
    for (int i = 0; i < size; i++) {
        free(tab[i]);
    }
    free(tab);
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
    for (int i = 0; i < size_y; i++) {
        for (int j = 0; j < size_x; j++) {
            printf("%d", inputs[i][j]);
        }
        printf("\n");
    }

    freeInput(inputs, size_y);

    fclose(f);
    clock_t end_time = clock();
    double time_spend = (double)(end_time - begin_time) / CLOCKS_PER_SEC;

    printf("It took %.5f seconds (cpu time) to compute.\n\n\n", time_spend);

    return 0;
}