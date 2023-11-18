#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LEN 128

// MOCHE -> bug si il y a une ligne en plus vide dans le fichier...
// De plus ça n'a pas de sens comme code xD y a probablement des manières smart de le faire (avec du calcul binaire, des modulos notamment jsp)

int main()
{

    FILE *f = fopen("inputs/day3.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    char buffer[MAX_LEN];
    fgets(buffer, MAX_LEN, f);
    int size_x = strlen(buffer);

    int compteur[size_x - 1][2];
    for (int i = 0; i < size_x - 1; i++)
    {
        compteur[i][0] = 0;
        compteur[i][1] = 0;
    }

    int size_y = 0;

    fsetpos(f, &start);
    while (!feof(f))
    {
        fgets(buffer, MAX_LEN, f);
        for (int i = 0; i < size_x - 1; i++)
        {
            compteur[i][(int)(buffer[i] - '0')]++;
        }
        size_y++;
    }
    if (size_y <= 0)
        return 1;

    int result[size_x - 1];

    int epsilon = 0;
    int gamma = 0;

    for (int i = 0; i < size_x - 1; i++)
    {
        result[i] = (compteur[i][0] >= compteur[i][1]) ? 0 : 1;
        epsilon += (int)pow(2, size_x - 2 - i) * result[i];
        gamma += (int)pow(2, size_x - 2 - i) * abs(result[i] - 1);
    }

    printf("-- Day 3 -- \n");
    printf("Epsilon rate : %d ; Gamma rate : %d ; Result1 : %d\n", epsilon, gamma, epsilon * gamma);

    if (size_y <= 0)
        size_y = 1;
    int not_used_oxygen[size_y];
    int not_used_co2[size_y];
    int nb_to_go_oxygen = size_y;
    int nb_to_go_co2 = size_y;
    int number_to_check = 0;

    fsetpos(f, &start);
    int i = 0;
    while (!feof(f))
    {
        fgets(buffer, MAX_LEN, f);
        not_used_oxygen[i] = (result[number_to_check] != (int)(buffer[number_to_check] - '0'));
        not_used_co2[i] = (result[number_to_check] == (int)(buffer[number_to_check] - '0'));

        if (not_used_oxygen[i])
            nb_to_go_oxygen--;
        if (not_used_co2[i])
            nb_to_go_co2--;
        i++;
    }
    number_to_check++;
    if (number_to_check >= size_x - 1)
        return 1;
    while ((nb_to_go_oxygen > 1) || (nb_to_go_co2 > 1))
    {
        int compte_oxygen[size_x - 1][2];
        int compte_co2[size_x - 1][2];
        for (int i = 0; i < size_x - 1; i++)
        {
            compte_oxygen[i][0] = 0;
            compte_oxygen[i][1] = 0;
            compte_co2[i][0] = 0;
            compte_co2[i][1] = 0;
        }
        fsetpos(f, &start);
        int j = 0;
        while (!feof(f))
        {
            fgets(buffer, MAX_LEN, f);
            for (int i = 0; i < size_x - 1; i++)
            {
                int c = (int)(buffer[i] - '0');
                if (c != 0 && c != 1)
                    return 1;
                if (!not_used_oxygen[j])
                    compte_oxygen[i][c]++;
                if (!not_used_co2[j])
                    compte_co2[i][c]++;
            }
            j++;
        }
        int val_oxygen = (compte_oxygen[number_to_check][1] >= compte_oxygen[number_to_check][0]) ? 1 : 0;
        int val_co2 = (compte_co2[number_to_check][0] <= compte_co2[number_to_check][1]) ? 0 : 1;
        fsetpos(f, &start);
        i = 0;
        while (!feof(f))
        {
            fgets(buffer, MAX_LEN, f);
            if ((nb_to_go_oxygen > 1) && !not_used_oxygen[i])
            {
                not_used_oxygen[i] = (val_oxygen != (int)(buffer[number_to_check] - '0'));
                if (not_used_oxygen[i])
                    nb_to_go_oxygen--;
            }
            if ((nb_to_go_co2 > 1) && !not_used_co2[i])
            {
                not_used_co2[i] = (val_co2 != (int)(buffer[number_to_check] - '0'));
                if (not_used_co2[i])
                    nb_to_go_co2--;
            }
            i++;
        }
        number_to_check++;
    }
    fsetpos(f, &start);

    i = 0;

    int oxygen = 0;
    int co2 = 0;
    while (!feof(f))
    {
        fgets(buffer, MAX_LEN, f);
        if (!not_used_oxygen[i])
        {
            for (int i = 0; i < size_x - 1; i++)
            {
                oxygen += (int)pow(2, size_x - 2 - i) * (int)(buffer[i] - '0');
            }
        }
        if (!not_used_co2[i])
        {
            for (int i = 0; i < size_x - 1; i++)
            {
                co2 += (int)pow(2, size_x - 2 - i) * (int)(buffer[i] - '0');
            }
        }
        i++;
    }

    printf("Oxygen : %d ; CO2 : %d, Result2 : %d\n", oxygen, co2, oxygen * co2);

    fclose(f);

    return 0;
}