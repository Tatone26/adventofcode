#include "runner.h"

typedef struct rule_
{
    short first;
    short second;
} rule;

long part1(int count, va_list args)
{
    if (count != 4)
    {
        printf("ERROR WITH ARGUMENTS\n");
        return -1;
    }
    rule *rules = va_arg(args, rule *);
    int nb_rules = va_arg(args, int);
    short **updates = va_arg(args, short **);
    int nb_updates = va_arg(args, int);

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

    int result = 0;

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
// tested : comb (bubble) sort
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

long part2(int count, va_list args)
{
    if (count != 4)
    {
        printf("ERROR WITH ARGUMENTS\n");
        return -1;
    }
    rule *rules = va_arg(args, rule *);
    int nb_rules = va_arg(args, int);
    short **updates = va_arg(args, short **);
    int nb_updates = va_arg(args, int);

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

    int result = 0;

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

// A few input perks : only 2 digits number and rules are positioned by blocks, but those can be at multiple spaces
rule *readInput(char *filename, int *nb_rules, int *nb_updates, short ***updates)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    fpos_t start;
    fgetpos(f, &start);
    int number_of_rules = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f) && buffer[0] != '\n' && buffer[2] == '|')
        number_of_rules++;
    if (number_of_rules == 0)
    {
        printf("Error reading input.\n");
        return 0;
    }
    *nb_rules = number_of_rules;
    rule *rules = (rule *)malloc(sizeof(rule) * *nb_rules);

    fsetpos(f, &start);
    for (int i = 0; i < *nb_rules; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        int a, b;
        if (sscanf(buffer, "%d|%d\n", &a, &b) != 2)
        {
            printf("error\n");
            free(rules);
            return 0;
        }
        rules[i] = (rule){a, b};
    }

    fgets(buffer, MAX_LINE_LEN, f);
    fpos_t second_start;
    fgetpos(f, &second_start);

    int number_of_updates = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
        number_of_updates++;
    fsetpos(f, &second_start);
    if (number_of_updates == 0)
    {
        free(rules);
        printf("error 2\n");
        return 0;
    }
    *nb_updates = number_of_updates;

    *updates = (short **)(malloc(sizeof(short *) * number_of_updates));
    for (int i = 0; i < number_of_updates; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
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

int main()
{
    int nb_rules = 0;
    int nb_updates = 0;
    short **updates = 0;
    rule *rules = readInput("input/5.txt", &nb_rules, &nb_updates, &updates);
    if (!updates || !rules)
    {
        printf("ERROR INPUT READING\n");
        return 1;
    }

    run(5, part1, part2, 4, rules, nb_rules, updates, nb_updates);

    for (int i = 0; i < nb_updates; i++)
        free(updates[i]);
    free(updates);

    free(rules);
    return 0;
}