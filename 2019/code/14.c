#include "runner.h"

typedef struct
{
    char inputs[10][10]; // list of all the inputs
    char output[10];     // what is produced
    int quantities[11];  // 0-9 for inputs, 10 for output
} Reaction;

void update_table(HashTable table, Reaction *input, int size, char *todo, int multi)
{
    for (int i = 0; i < size; i++)
    {
        if (!strncmp(todo, input[i].output, strlen(todo)))
        {
            lint old = get_flag(table, NULL, input[i].output, 0);
            lint used = get_flag(table, NULL, input[i].output, 1);
            lint possible = input[i].quantities[10];
            lint need = multi;
            insert_hash(table, NULL, input[i].output, used + need, 1);

            if (used + need <= old)
                break;
            lint m;
            lint to_create = need - (old - used);
            if (to_create % possible == 0)
                m = to_create / possible;
            else
                m = to_create / possible + 1;
            lint created = m * possible;
            insert_hash(table, NULL, input[i].output, old + created, 0);

            for (int x = 0; x < 10 && input[i].quantities[x] != 0; x++)
            {
                if (!strncmp("ORE", input[i].inputs[x], strlen(input[i].inputs[x])))
                {
                    lint used_ore = get_flag(table, NULL, "ORE", 1);
                    lint possible_ore = input[i].quantities[x];
                    lint needed_ore = m * possible_ore;
                    insert_hash(table, NULL, "ORE", needed_ore + used_ore, 1);
                }
                else
                    update_table(table, input, size, input[i].inputs[x], m * input[i].quantities[x]);
            }
            break;
        }
    }
}

luint part1(void *input_v, void **args)
{
    Reaction *input = (Reaction *)input_v;
    int size = ((int *)args)[0];

    HashTable table = {NULL};

    /* for (int i = 0; i < size; i++)
    {
        printf("\nOutput : %d %s\n", input[i].quantities[10], input[i].output);
        for (int x = 0; x < 10 && input[i].quantities[x] != 0; x++)
            printf("Input : %d %s\n", input[i].quantities[x], input[i].inputs[x]);
    } */

    update_table(table, input, size, "FUEL", 1);
    return get_flag(table, NULL, "ORE", 1);
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    Reaction *input = (Reaction *)input_v;
    int size = ((int *)args)[0];

    if (size <= 0)
        return 0;

    HashTable table = {NULL};
    update_table(table, input, size, "FUEL", 1);
    luint per_fuel = (luint)get_flag(table, NULL, "ORE", 1);

    luint *needed = (luint *)malloc(sizeof(luint) * size);
    luint *excedent = (luint *)malloc(sizeof(luint) * size);
    for (int i = 0; i < size; i++)
    {
        needed[i] = 1;
        excedent[i] = 1;
    }

    luint biggest_ratio = 0;
    int biggest_ratio_output = 0;
    ;
    for (int i = 0; i < size; i++)
    {
        needed[i] = get_flag(table, NULL, input[i].output, 1);
        excedent[i] = get_flag(table, NULL, input[i].output, 0) - needed[i];
        luint ratio = excedent[i] > 0 ? needed[i] / excedent[i] : 0;
        if (ratio > biggest_ratio)
        {
            biggest_ratio = ratio;
            biggest_ratio_output = i;
        }
    }

    printf("For %s, excedent %lld and needed %lld\n", input[biggest_ratio_output].output, excedent[biggest_ratio_output], needed[biggest_ratio_output]);
    luint more_every = excedent[biggest_ratio_output] * needed[biggest_ratio_output];
    luint total_single_big_loop = more_every + excedent[biggest_ratio_output]; // that's wrong :
    printf("Getting more all %lld loops ; and total being %lld\n", more_every, total_single_big_loop);

    luint big_loop_consommation = per_fuel * more_every;
    printf("consommation : %lld\n", big_loop_consommation);
    luint number_of_big_loops = (1000000000000 / big_loop_consommation);
    printf("number of big loops : %lld\n", number_of_big_loops);
    luint total_with_bonus = total_single_big_loop * number_of_big_loops;
    printf("producing %lld\n", total_with_bonus);
    luint excess_ore = 1000000000000 - (number_of_big_loops * big_loop_consommation);
    printf("Ore left : %lld\n", excess_ore);

    luint total = total_with_bonus + excess_ore / per_fuel;

    free(needed);
    free(excedent);
    free_hash(table);

    // Ici, récupérer aussi cb il faut pour l'excès (soit le nombre d'ore en excès, soit le nombre pour chaque truc produit par l'ore)
    // ensuite, pour tester les loops, regarder si old - used == ce nombre par exemple -> alors la prochaine boucle est en fait double et on devrait
    // ah bah non parce que ça voudrait dire qu'à la suivante on a flag 1 == flag 0 et c'est jamais le cas pttnnnn
    // ptn wtfff

    return total;

    // 1333333333248
    // 1937984496
    // 150130974
    // 83418309
    // 82892292
    // 82892753
}

// ----------------------------------------------------------------

Reaction *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    *size = fileSize(f);
    Reaction *input = (Reaction *)malloc(sizeof(Reaction) * *size);

    int r = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        char inputs[100];
        char outputs[100];
        sscanf(buffer, "%99[^=]=> %99[^\n]", inputs, outputs);
        int q = 0;
        int start = 0;
        while (start < (int)strlen(inputs))
        {
            if (sscanf(inputs + start, "%d %99[^,]", &input[r].quantities[q], input[r].inputs[q]) != 2)
                printf("ERROR\n");
            ;
            start += nbOfDigits(input[r].quantities[q]) + 1 + strlen(input[r].inputs[q]) + 2;
            q++;
        }
        int length = strlen(input[r].inputs[q - 1]);
        input[r].inputs[q - 1][length - 1] = '\0';
        for (; q < 10; q++)
        {
            for (int x = 0; x < 10; x++)
                input[r].inputs[q][x] = '\0';
            input[r].quantities[q] = 0;
        }

        sscanf(outputs, "%d %s", &input[r].quantities[10], input[r].output);
        r++;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Reaction *input = readInput(argv[1], &size);
    run(14, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}