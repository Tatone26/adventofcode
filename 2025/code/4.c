#include "runner.h"

// Using a static list makes the 'neighbors' listing a lot easier (and faster)
// using Pos is totally overkill but I don't care
const Pos neighbors[8] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
const Pos opti_neighbors[4] = {{-1, 1}, {0, 1}, {1, 1}, {1, 0}};

luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int height = ((int *)args)[0];
    int width = ((int *)args)[1];

    luint count = 0;

    int *nb_neighbors = (int *)calloc(width * height, sizeof(int));

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            if (input[INDEX(y, x)] != '@')
                continue;
            // since we are iterating left to right and up to down, we can just iterate over the down and right neighbors,
            // if we update those accordingly
            for (int p = 0; p < 4; p++)
            {
                Pos n_pos = (Pos){x + opti_neighbors[p].x, y + opti_neighbors[p].y};
                if (INBOUNDS(n_pos, width, height) && input[INDEX_POS(n_pos)] == '@')
                {
                    nb_neighbors[INDEX(y, x)]++;
                    nb_neighbors[INDEX_POS(n_pos)]++;
                }
            }
            if (nb_neighbors[INDEX(y, x)] < 4)
                count++;
        }

    free(nb_neighbors);
    return count;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    char *input_t = (char *)input_v;
    int height = ((int *)args)[0];
    int width = ((int *)args)[1];

    if (width <= 0 || height <= 0)
        return 0;

    char *input = strndup(input_t, width * height);

    // using this list to know if we need to test a given point again or not.
    // a more optimised way in other langages would be a set containing all updated indexes.
    // In C, I suspect there would be too much overhead in creating such a set.
    // Time for now (gamemode) : 1.8/2ms
    // other ideas like counting neighbors like in part 1 have been slower.
    char *updated = (char *)calloc(width * height, sizeof(char));
    for (int c = 0; c < width * height; c++)
        if (input[c] == '@')
            updated[c] = 1;

    luint count = 0;
    int has_removed_something = 1;
    while (has_removed_something) // lol
    {
        has_removed_something = 0;
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
            {
                assert(width > 0 && height > 0); // makes linter happy
                if (!(updated[y * width + x]))
                    continue;

                int neighbors_count = 0;
                char n_update[8] = {0};
                for (int p = 0; p < 8 && neighbors_count < 4; p++) // counting all neighbors
                {
                    Pos n_pos = (Pos){x + neighbors[p].x, y + neighbors[p].y};
                    if (INBOUNDS(n_pos, width, height) && input[INDEX_POS(n_pos)] == '@')
                    {
                        neighbors_count++;
                        n_update[p] = 1; // keeping the info 'is the nth neighbor a roll of paper ?'
                    }
                }
                if (neighbors_count < 4)
                {
                    has_removed_something = 1;
                    input[INDEX(y, x)] = '.';
                    count++;
                    for (int p = 0; p < 8; p++)
                        if (n_update[p])
                            updated[INDEX((y + neighbors[p].y), (x + neighbors[p].x))] = 1;
                }
                updated[INDEX(y, x)] = 0;
            }
    }

    free(updated);
    free(input);
    return count;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *height, int *width)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    *height = fileSize(f);
    fgets(buffer, MAX_LINE_LEN, f);
    *width = strlen(buffer) - 1;
    char *input = (char *)malloc(sizeof(char) * (*width) * (*height)); // WARNING : change size of alloc

    int offset = 0;
    for (int i = 0; i < *height; i++)
    {
        strncpy(input + offset, buffer, *width); // -1 to remove \n
        offset += *width;
        fgets(buffer, MAX_LINE_LEN, f);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int height = 0, width = 0;
    char *input = readInput(argv[1], &height, &width);
    int args[] = {height, width};
    run(4, part1, part2, input, (void **)args);
    free(input);
    return 0;
}