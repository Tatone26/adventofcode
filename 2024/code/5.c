#include "runner.h"

typedef struct rule_
{
    short first;
    short second;
} rule;

luint part1(void *input_v, void **args)
{
    rule *rules = (rule *)input_v;
    int nb_rules = *(int *)args[0];
    short **updates = (short **)args[1];
    int nb_updates = *(int *)args[2];

    // Reading the rules (only two digits number allow making a fixed size array), inverted (at array x you have all y|x)
    short *read_rules[100] = {0};
    for (int i = 10; i < 100; i++)
        read_rules[i] = (short *)calloc(nb_rules / 2, sizeof(short));

    for (int i = 0; i < nb_rules; i++)
    {
        rule r = rules[i];
        int j;
        for (j = 0; read_rules[r.second][j] != 0; j++)
            ;
        read_rules[r.second][j] = r.first;
    }

    luint result = 0;

    for (int r = 0; r < nb_updates; r++)
    {
        short *update = updates[r];
        for (int i = 0; update[i + 1] != -1; i++)
        {
            short *current_rules = read_rules[update[i]];
            for (int next = i + 1; update[next] != -1; next++)
            {
                for (int j = 0; current_rules[j] != 0; j++)
                {
                    if (current_rules[j] == update[next])
                        goto not_good;
                }
            }
        }
        int update_size = 0;
        while (update[update_size] != -1)
            update_size++;
        result += update[(update_size / 2)];
    not_good:
        continue;
    }

    for (int i = 10; i < 100; i++)
        free(read_rules[i]);

    return result;
}

// -----------------------------------------------------------------

// Couldn't use the built-in qsort function because I need the extra parameter. I know about qsort_r, but it is an external library and I don't like it :)
// (also I hate myself)
// tested : comb (bubble) sort, slower
void quicksort(short *update, int first, int last, short **read_rules)
{
    if (first >= last)
        return;
    // the pivot is considered to be the last point.
    int pivot_value = update[last];
    // for every point in the array (between first and last)
    int j = first; // this is the position of the last point smaller than the pivot
    for (int i = first; i < last; i++)
    {
        // get the rules of this point
        short *current_rules = read_rules[update[i]];
        for (int r = 0; current_rules[r] != 0; r++)
            // if update[i] "<" update[last]
            if (current_rules[r] == pivot_value)
            {
                // put update[i] before the pivot
                short temp = update[i];
                update[i] = update[j];
                update[j] = temp;
                j++;
            }
        // else do nothing
    }
    // put the pivot at its place : above the elements smaller than it
    update[last] = update[j];
    update[j] = pivot_value;
    int pivot = j;
    // rec
    quicksort(update, first, pivot - 1, read_rules);
    quicksort(update, pivot + 1, last, read_rules);
}

// I allowed sorting the input in-line in part two, because I didn't want to copy every line when it won't be used again.
luint part2(void *input_v, void **args)
{
    rule *rules = (rule *)input_v;
    int nb_rules = *(int *)args[0];
    short **updates = (short **)args[1];
    int nb_updates = *(int *)args[2];

    // Reading the rules (only two digits number allow making a fixed size array), inverted : at array x, you have the y that needs to be BEFORE (so y|x)
    short *read_rules[100] = {0};
    for (int i = 10; i < 100; i++)
        read_rules[i] = (short *)calloc(nb_rules / 2, sizeof(short));

    for (int i = 0; i < nb_rules; i++)
    {
        rule r = rules[i];
        int j;
        for (j = 0; read_rules[r.second][j] != 0; j++)
            ;
        read_rules[r.second][j] = r.first;
    }

    luint result = 0;

    for (int r = 0; r < nb_updates; r++)
    {
        short *update = updates[r];
        for (int i = 0; update[i] != -1; i++)
        {
            short *current_rules = read_rules[update[i]];
            for (int next = i + 1; update[next] != -1; next++)
            {
                for (int j = 0; current_rules[j] != 0; j++)
                    if (current_rules[j] == update[next])
                        goto not_good_part2;
            }
        }
        continue;
    not_good_part2:
    {
        int update_size = 0;
        while (update[update_size] != -1)
            update_size++;

        quicksort(update, 0, update_size - 1, read_rules);

        result += update[update_size / 2];
    }
    }

    for (int i = 10; i < 100; i++)
        free(read_rules[i]);

    return result;
}

// ----------------------------------------------------------------

rule *readInput(char *filename, int *nb_rules, int *nb_updates, short ***updates)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;
    fpos_t start;
    fgetpos(f, &start);
    int number_of_rules = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f) && buffer[0] != '\n' && buffer[2] == '|')
        number_of_rules++;
    if (number_of_rules == 0)
    {
        printf("Error reading input.\n");
        fclose(f);
        return NULL;
    }
    *nb_rules = number_of_rules;
    rule *rules = (rule *)malloc(sizeof(rule) * *nb_rules);

    fsetpos(f, &start);
    for (int i = 0; i < *nb_rules; i++)
    {
        if (!fgets(buffer, MAX_LINE_LEN, f))
        {
            free(rules);
            fclose(f);
            return NULL;
        }
        int a, b;
        if (sscanf(buffer, "%d|%d\n", &a, &b) != 2)
        {
            printf("Error reading input bis.\n");
            free(rules);
            fclose(f);
            return 0;
        }
        rules[i] = (rule){a, b};
    }

    if (!fgets(buffer, MAX_LINE_LEN, f))
    {
        free(rules);
        fclose(f);
        return NULL;
    }
    *nb_updates = fileSize(f);
    if (*nb_updates == 0)
    {
        free(rules);
        fclose(f);
        printf("Error reading input ter.\n");
        return NULL;
    }

    *updates = (short **)(malloc(sizeof(short *) * *nb_updates));
    for (int i = 0; i < *nb_updates; i++)
    {
        if (!fgets(buffer, MAX_LINE_LEN, f))
        {
            for (int y = 0; y < i; y++)
            {
                free((*updates)[y]);
                free(*updates);
                free(rules);
                *updates = NULL;
                return NULL;
            }
        }
        int size = 0;
        int offset = 0;
        int a;
        while (sscanf(buffer + offset, "%d", &a) == 1)
        {
            size++;
            offset += 3;
        }

        (*updates)[i] = (short *)malloc(sizeof(short) * (size + 1));
        offset = 0;
        int j = 0;
        while (sscanf(buffer + offset, "%d", &a) == 1)
        {
            (*updates)[i][j] = a;
            j++;
            offset += 3;
        }
        (*updates)[i][size] = -1;
    }

    fclose(f);
    return rules;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int nb_rules = 0, nb_updates = 0;
    short **updates = 0;
    rule *rules = readInput(argv[1], &nb_rules, &nb_updates, &updates);
    if (!updates || !rules)
    {
        if (rules)
            free(rules);
        if (updates)
        {
            for (int i = 0; i < nb_updates; i++)
                free(updates[i]);
            free(updates);
        }
        printf("ERROR INPUT READING\n");
        return 1;
    }
    void *args[3] = {&nb_rules, updates, &nb_updates};

    run(5, part1, part2, rules, args);

    for (int i = 0; i < nb_updates; i++)
        free(updates[i]);
    free(updates);

    free(rules);
    return 0;
}