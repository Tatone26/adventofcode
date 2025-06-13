#include "runner.h"

typedef struct
{
    int dx, dy;
    int n;
} Move;

luint part1(void *input_v, void **args)
{
    Move **input = (Move **)input_v;
    int size1 = ((int *)args)[0];
    int size2 = ((int *)args)[1];
    int sizes[2] = {size1, size2};

    Pos *firstwire = (Pos *)malloc(sizeof(Pos) * (size1 + 1));
    Pos *secondwire = (Pos *)malloc(sizeof(Pos) * (size2 + 1));
    Pos *wires[2] = {firstwire, secondwire};

    // getting all the positions of the turns
    for (int w = 0; w < 2; w++)
    {
        wires[w][0] = (Pos){0, 0};
        int x = 0, y = 0;
        for (int i = 0; i < sizes[w]; i++)
        {
            x += input[w][i].dx * input[w][i].n;
            y += input[w][i].dy * input[w][i].n;
            wires[w][i + 1] = (Pos){x, y};
        }
    }

    // finding the intersections (and keeping the best one)
    int best_intersect_score = __INT32_MAX__;
    for (int i = 0; i < size1; i++)
    {
        Pos start = firstwire[i];
        Pos end = firstwire[i + 1];
        for (int j = 0; j < size2; j++)
        {
            Pos start2 = secondwire[j];
            Pos end2 = secondwire[j + 1];
            // These two big conditions are doing the same thing as if I was to invert the two wires to test again.
            if (start.x == end.x && ((start.x <= start2.x && start.x >= end2.x) || (start.x >= start2.x && start.x <= end2.x)))
            // wire1 is potentially cut horizontally
            {
                if (start2.y == end2.y && ((start.y <= start2.y && end.y >= start2.y) || (start.y >= start2.y && end.y <= start2.y)))
                // found intersection
                {
                    int intersect_x = start.x;
                    int intersect_y = start2.y;
                    int score = abs((int)intersect_x) + abs((int)intersect_y);
                    if (best_intersect_score <= 0 || best_intersect_score > score)
                        best_intersect_score = score;
                }
            }
            if (start2.x == end2.x && ((start2.x <= start.x && start2.x >= end.x) || (start2.x >= start.x && start2.x <= end.x)))
            // wire2 is potentially cut horizontally
            {
                if (start.y == end.y && ((start2.y <= start.y && end2.y >= start.y) || (start2.y >= start.y && end2.y <= start.y)))
                // found intersection
                {
                    int intersect_x = start2.x;
                    int intersect_y = start.y;
                    int score = abs((int)intersect_x) + abs((int)intersect_y);
                    if (best_intersect_score <= 0 || best_intersect_score > score)
                        best_intersect_score = score;
                }
            }
        }
    }

    free(firstwire);
    free(secondwire);
    if (best_intersect_score == __INT32_MAX__ || best_intersect_score == 0)
    {
        printf("Didn't found any intersection.");
        return 0;
    }
    return best_intersect_score;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    Move **input = (Move **)input_v;
    int size1 = ((int *)args)[0];
    int size2 = ((int *)args)[1];
    int sizes[2] = {size1, size2};

    Pos *firstwire = (Pos *)malloc(sizeof(Pos) * (size1 + 1));
    int *firststeps = (int *)malloc(sizeof(int) * (size1 + 1));
    Pos *secondwire = (Pos *)malloc(sizeof(Pos) * (size2 + 1));
    int *secondsteps = (int *)malloc(sizeof(int) * (size2 + 1));
    Pos *wires[2] = {firstwire, secondwire};
    int *steps[2] = {firststeps, secondsteps};

    // getting all the positions of the turns
    for (int w = 0; w < 2; w++)
    {
        wires[w][0] = (Pos){0, 0};
        steps[w][0] = 0;
        int x = 0, y = 0, tn = 0;
        for (int i = 0; i < sizes[w]; i++)
        {
            x += input[w][i].dx * input[w][i].n;
            y += input[w][i].dy * input[w][i].n;
            tn += input[w][i].n;
            wires[w][i + 1] = (Pos){x, y};
            steps[w][i + 1] = tn;
        }
    }

    // finding the intersections (and keeping the best one)
    int best_intersect_score = __INT32_MAX__;
    for (int i = 0; i < size1; i++)
    {
        Pos start = firstwire[i];
        Pos end = firstwire[i + 1];
        for (int j = 0; j < size2; j++)
        {
            Pos start2 = secondwire[j];
            Pos end2 = secondwire[j + 1];
            // These two big conditions are doing the same thing as if I was to invert the two wires to test again.
            if (start.x == end.x && ((start.x <= start2.x && start.x >= end2.x) || (start.x >= start2.x && start.x <= end2.x)))
            // wire1 is potentially cut horizontally
            {
                if (start2.y == end2.y && ((start.y <= start2.y && end.y >= start2.y) || (start.y >= start2.y && end.y <= start2.y)))
                // found intersection
                {
                    int steps_for_first = firststeps[i];
                    int steps_for_second = secondsteps[j];
                    steps_for_first += abs((int)(start2.y - start.y));
                    steps_for_second += abs((int)(start.x - start2.x));
                    int score = steps_for_first + steps_for_second;
                    if (best_intersect_score <= 0 || best_intersect_score > score)
                        best_intersect_score = score;
                }
            }
            if (start2.x == end2.x && ((start2.x <= start.x && start2.x >= end.x) || (start2.x >= start.x && start2.x <= end.x)))
            // wire2 is potentially cut horizontally
            {
                if (start.y == end.y && ((start2.y <= start.y && end2.y >= start.y) || (start2.y >= start.y && end2.y <= start.y)))
                // found intersection
                {
                    int steps_for_first = secondsteps[j];
                    int steps_for_second = firststeps[i];
                    steps_for_first += abs((int)(start.y - start2.y));
                    steps_for_second += abs((int)(start2.x - start.x));
                    int score = steps_for_first + steps_for_second;
                    if (best_intersect_score <= 0 || best_intersect_score > score)
                        best_intersect_score = score;
                }
            }
        }
    }

    free(firstwire);
    free(secondwire);
    free(firststeps);
    free(secondsteps);
    if (best_intersect_score == __INT32_MAX__ || best_intersect_score == 0)
    {
        printf("Didn't found any intersection.");
        return 0;
    }
    return best_intersect_score;
}

// ----------------------------------------------------------------

Move **readInput(char *filename, int *size1, int *size2)
{

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;

    fpos_t start;
    fgetpos(f, &start);

    int *sizes[2] = {size1, size2};

    Move **input = (Move **)malloc(sizeof(Move *) * 2);
    for (int m = 0; m < 2; m++)
    {
        fsetpos(f, &start);
        char buffer[MAX_LINE_LEN] = {0};
        for (int x = 0; x <= m; x++)
            if (NULL == fgets(buffer, MAX_LINE_LEN, f))
            {
                if (m == 1)
                    free(input[0]);
                free(input);
                fclose(f);
                return NULL;
            }

        *sizes[m] = 1;
        for (int i = 0; buffer[i] != '\n' && buffer[i] != '\0'; i++)
            if (buffer[i] == ',')
                *sizes[m] += 1;
        Move *wire = (Move *)malloc(sizeof(Move) * *sizes[m]);
        int offset = 0;
        int start_offset = 0;
        for (int i = 0; i < *sizes[m]; i++)
        {
            while (buffer[offset] != '\n' && buffer[offset] != '\0' && buffer[offset] != ',')
                offset++;
            char number[100];
            strncpy(number, buffer + start_offset, offset - start_offset);
            number[offset - start_offset] = '\0';
            int n = atoi(number + 1);
            int dx, dy;
            switch (number[0])
            {
            case 'L':
                dx = -1;
                dy = 0;
                break;
            case 'R':
                dx = 1;
                dy = 0;
                break;
            case 'U':
                dx = 0;
                dy = -1;
                break;
            case 'D':
                dx = 0;
                dy = 1;
                break;
            default:
                dx = 0;
                dy = 0;
                break;
            }
            wire[i] = (Move){dx, dy, n};
            offset++;
            start_offset = offset;
        }
        input[m] = wire;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size1, size2;
    Move **input = readInput(argv[1], &size1, &size2);
    int sizes[2] = {size1, size2};
    run(3, part1, part2, input, (void **)&sizes);
    free(input[0]);
    free(input[1]);
    free(input);
    return 0;
}