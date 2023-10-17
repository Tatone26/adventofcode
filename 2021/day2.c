#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main()
{

    clock_t begin = clock();

    FILE *f = fopen("day2.txt", "r");
    if (!f)
    {
        printf("Erreur à l'ouverture du fichier.\n");
        return 1;
    }

    int x = 0;
    int y = 0;

    int aim = 0;
    int y2 = 0;

    char direction[8];
    int steps;

    while (!feof(f))
    {

        if (fscanf(f, "%s %d\n", direction, &steps) != 2)
        {
            printf("Erreur de lecture.\n");
            return 1;
        }

        switch (direction[0])
        {
        case 'f':
            x += steps;

            y2 += aim * steps;
            break;
        case 'u':
            y -= steps;

            aim -= steps;
            break;
        case 'd':
            y += steps;

            aim += steps;
            break;
        default:
            printf("Mauvaise lecture.\n");
        }
    }

    printf("-- Day 2 -- \n");
    printf("Arrivée : %d %d, résultat 1 : %d\n", x, y, x * y);
    printf("Arrivée 2 : %d %d, résultat 2 : %d\n", x, y2, x * y2);

    fclose(f);

    clock_t end = clock();
    double time_spend = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("It took %.5f seconds (cpu time) to compute.\n\n\n", time_spend);

    return 0;
}