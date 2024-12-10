#include "runner.h"

#define MAX_IDENTICAL_ANTENNAS 10
#define MAX_ANTENNA_INDEX 62

// 0 - 61 inclus
int charIndex(char c)
{
    if (48 <= c && 57 >= c)
        return c - 48;
    else if (65 <= c && 90 >= c)
        return c - 65 + 10;
    else if (97 <= c && 122 >= c)
        return c - 97 + 10 + 26;
    else
        return -1;
}

/// @brief Processes the input into a list of list of positions
/// @param table Needs to be initialised with {-1, -1} everywhere.
void processInput(Pos table[MAX_ANTENNA_INDEX][MAX_IDENTICAL_ANTENNAS], char *input, int width, int height)
{
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            char c = input[INDEX(i, j)];
            if (c == '.')
                continue;
            int index_c = charIndex(c);
            int d;
            for (d = 0; table[index_c][d].x != -1; d++)
                ;
            table[index_c][d] = (Pos){j, i};
        }
}

luint part1(va_list args)
{
    char *input = va_arg(args, char *);
    int width = va_arg(args, int);
    int height = va_arg(args, int);
    // one line / character, max 100 positions for each
    Pos table[MAX_ANTENNA_INDEX][MAX_IDENTICAL_ANTENNAS];
    for (int i = 0; i < MAX_ANTENNA_INDEX; i++)
        for (int j = 0; j < MAX_IDENTICAL_ANTENNAS; j++)
            table[i][j] = (Pos){-1, -1};
    processInput(table, input, width, height);

    luint res = 0;
    bool *antinodes = (bool *)calloc(sizeof(bool), width * height);

    // for every type of antenna
    for (int type = 0; type < MAX_ANTENNA_INDEX; type++)
        // for every antenna of this type
        for (int ant = 0; table[type][ant].x != -1; ant++)
        {
            Pos antenna = table[type][ant];
            // for every next antenna in the list
            for (int next = ant + 1; table[type][next].x != -1; next++)
            {
                Pos dPos = {antenna.x - table[type][next].x,
                            antenna.y - table[type][next].y};
                Pos testPos = (Pos){antenna.x + dPos.x, antenna.y + dPos.y};
                if (INBOUNDS(testPos, width, height) && !antinodes[INDEX_POS(testPos)])
                {
                    res++;
                    antinodes[INDEX_POS(testPos)] = true;
                }
                // testing the other side
                testPos = (Pos){antenna.x - 2 * dPos.x, antenna.y - 2 * dPos.y};
                if (INBOUNDS(testPos, width, height) && !antinodes[INDEX_POS(testPos)])
                {
                    res++;
                    antinodes[INDEX_POS(testPos)] = true;
                }
            }
        }

    free(antinodes);
    return res;
}

// -----------------------------------------------------------------

luint part2(va_list args)
{
    char *input = va_arg(args, char *);
    int width = va_arg(args, int);
    int height = va_arg(args, int);
    // one line / character, max 100 positions for each
    Pos table[MAX_ANTENNA_INDEX][MAX_IDENTICAL_ANTENNAS];
    for (int i = 0; i < MAX_ANTENNA_INDEX; i++)
        for (int j = 0; j < MAX_IDENTICAL_ANTENNAS; j++)
            table[i][j] = (Pos){-1, -1};
    processInput(table, input, width, height);

    luint res = 0;
    bool *antinodes = (bool *)calloc(sizeof(bool), width * height);

    // for every type of antenna
    for (int type = 0; type < MAX_ANTENNA_INDEX; type++)
        // for every antenna of this type
        for (int ant = 0; table[type][ant].x != -1; ant++)
        {
            Pos antenna = table[type][ant];
            // for every next antenna
            for (int next = ant + 1; table[type][next].x != -1; next++)
            {
                Pos dPos = (Pos){antenna.x - table[type][next].x, antenna.y - table[type][next].y};
                Pos testPos = (Pos){antenna.x, antenna.y};
                // Find the smallest in-bounds position
                while (INBOUNDS(testPos, width, height))
                    testPos = (Pos){testPos.x -= dPos.x, testPos.y -= dPos.y};
                testPos = (Pos){testPos.x += dPos.x, testPos.y += dPos.y}; // stay in-bounds
                // go back up to the other side of the map while testing for the new positions
                while (INBOUNDS(testPos, width, height))
                {
                    if (!antinodes[INDEX_POS(testPos)])
                    {
                        res++;
                        antinodes[INDEX_POS(testPos)] = true;
                    }
                    testPos = (Pos){testPos.x += dPos.x, testPos.y += dPos.y};
                }
            }
        }

    free(antinodes);
    return res;
}

// ----------------------------------------------------------------

char *readInput(char *filename, int *width, int *height)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *height = fileSize(f);
    fpos_t start;
    fgetpos(f, &start);

    fgets(buffer, MAX_LINE_LEN, f);
    *width = strlen(buffer) - 1;

    char *input = (char *)malloc(sizeof(char) * *width * *height);

    fsetpos(f, &start);
    for (int j = 0; j < *height; j++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        strncpy(input + (j * *width), buffer, *width);
    }

    fclose(f);
    return input;
}

int main()
{
    int width = 0, height = 0;
    char *input = readInput("input/8.txt", &width, &height);
    run(8, part1, part2, 3, input, width, height);
    free(input);
    return 0;
}