#include "runner.h"

// Using a static list makes the 'neighbors' listing a lot easier (and faster)
// using Pos is totally overkill but I don't care
const Pos neighbors[8] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};

luint part1(void *input_v, void **args)
{
    char *input = (char *)input_v;
    int height = ((int *)args)[0];
    int width = ((int *)args)[1];

    luint count = 0;

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            if (input[INDEX(y, x)] != '@')
                continue;
            int neighbors_count = 0;
            for (int p = 0; p < 8; p++)
            {
                Pos n_pos = (Pos){x + neighbors[p].x, y + neighbors[p].y};
                if (INBOUNDS(n_pos, width, height) && input[INDEX_POS(n_pos)] == '@')
                    neighbors_count++;
            }
            if (neighbors_count < 4)
                count++;
        }

    return count;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    char *input_t = (char *)input_v;
    int height = ((int *)args)[0];
    int width = ((int *)args)[1];

    char *input = strndup(input_t, width * height);

    luint count = 0;
    int has_removed_something = 1;
    while (has_removed_something) // lol
    {
        has_removed_something = 0;
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
            {
                if (input[INDEX(y, x)] != '@')
                    continue;
                int neighbors_count = 0;
                for (int p = 0; p < 8; p++)
                {
                    Pos n_pos = (Pos){x + neighbors[p].x, y + neighbors[p].y};
                    if (INBOUNDS(n_pos, width, height) && input[INDEX_POS(n_pos)] == '@')
                        neighbors_count++;
                }
                if (neighbors_count < 4)
                {
                    has_removed_something = 1;
                    input[INDEX(y, x)] = '.';
                    count++;
                }
            }
    }
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
            printf("%c", input[INDEX(y, x)]);
        printf("\n");
    }
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