#include <stdlib.h>
#include <stdio.h>

int main()
{

    FILE *f = fopen("inputs/day1.txt", "r");
    if (!f)
    {
        printf("Erreur à l'ouverture du fichier.\n");
        return 1;
    }

    int compteur = 0;
    int last = 0;

    int compteur2 = 0;
    int last_three[3] = {0, 0, 0};

    while (!feof(f))
    {
        int nb;
        int test = fscanf(f, "%d\n", &nb);
        if (test != 1)
        {
            printf("Echec lors de la lecture.\n");
            return 1;
        }
        if (last && (nb > last))
            compteur++;
        last = nb;

        if (last_three[0] && last_three[1] && last_three[2] && ((nb + last_three[1] + last_three[2]) > (last_three[0] + last_three[1] + last_three[2])))
            compteur2++;
        last_three[0] = last_three[1];
        last_three[1] = last_three[2];
        last_three[2] = nb;
    }
    printf("-- Day 1 -- \n");
    printf("Resultat : %d\n", compteur);
    printf("Resultat 2 : %d\n", compteur2);

    fclose(f);

    return 0;
}