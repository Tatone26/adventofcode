#include "runner.h"

typedef struct
{
    char inputs[10][10]; // list of all the inputs
    char output[10];     // what is produced
    int quantities[11];  // 0-9 for inputs, 10 for output
} Reaction;

// Potential (big) boost : using a queue to use ingredients only once
// When producing X * n, for each ingredient :
// - IF NOT ALREADY IN QUEUE, add ingredients in queue (with n being multiplied like in the recursion)
// - IF IN QUEUE, update n by adding to it
// Then, as long as queue is not empty :
// if thing is ORE, then add it to total
// if thing is something else, then update queue
// that's all
// That should work, at worst it will process things multiple times (if the order is bad, it may process the lower-level ones first but that's not a problem)
void update_table(HashTable table, Reaction *input, int size, char *todo, luint multi)
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

    // getting an upper bound for binary searching (doesn't cost much, but pretty efficient)
    HashTable table = {NULL};
    update_table(table, input, size, "FUEL", 1);
    luint upper_bound = 1000000000000 / (get_flag(table, NULL, "ORE", 1) / 2); // that's quite random but it feels good

    // lets try binary searching ! (I didn't think of it myself, but it's quite obvious :( )
    luint low = 1;
    luint high = upper_bound;
    while (low < high)
    {
        luint mid = (low + high + 1) / 2;
        HashTable table = {NULL};
        update_table(table, input, size, "FUEL", mid);
        luint tot = get_flag(table, NULL, "ORE", 1);
        if (tot > 1000000000000)
            high = mid - 1;
        else
            low = mid;
        // reset table
        free_hash(table);
    }
    return low;
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