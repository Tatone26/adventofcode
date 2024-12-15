#include "runner.h"

// I didn't spend 5 hours before realising it was just a *very* simple equation solving thing, I don"t know what you are talking about

typedef struct
{
    Pos buttonA;
    Pos buttonB;
    Pos prise;
} Machine;

luint part1(void *input_v, void **args)
{

    Machine *input = (Machine *)input_v;
    int size = ((int *)args)[0];

    luint res = 0;

    for (int ma = 0; ma < size; ma++)
    {
        Machine m = input[ma];
        int det = m.buttonA.x * m.buttonB.y - m.buttonB.x * m.buttonA.y;
        if (det == 0)
            continue;
        int x = (m.prise.x * m.buttonB.y - m.prise.y * m.buttonB.x);
        int y = (m.prise.y * m.buttonA.x - m.prise.x * m.buttonA.y);

        if (x % det != 0 || y % det != 0)
            continue;
        x = x / det;
        y = y / det;
        if (x < 0 || y < 0 || x > 100 || y > 100)
            continue;
        res += x * 3 + y;
    }
    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    Machine *input = (Machine *)input_v;
    int size = ((int *)args)[0];

    luint res = 0;

    for (int ma = 0; ma < size; ma++)
    {
        Machine m = input[ma];
        m.prise.x += 10000000000000;
        m.prise.y += 10000000000000;
        int det = m.buttonA.x * m.buttonB.y - m.buttonB.x * m.buttonA.y;
        if (det == 0) // If system doesn't have any solution
            continue;
        long long x = (m.prise.x * m.buttonB.y - m.prise.y * m.buttonB.x);
        long long y = (m.prise.y * m.buttonA.x - m.prise.x * m.buttonA.y);
        if (x % det != 0 || y % det != 0)
            continue; // if solutions aren't integers
        x = x / det;
        y = y / det;
        if (x < 0 || y < 0) // if solutions are negative integers
            continue;
        // we keep only positive integer solutions !
        res += x * 3 + y;
    }
    return res;
}

// ----------------------------------------------------------------

Machine *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *size = fileSize(f) / 4 + 1;
    Machine *input = (Machine *)malloc(sizeof(Machine) * *size);
    for (int i = 0; i < *size; i++)
    {
        int a = -1, b = -1, c = -1, d = -1, e = -1, g = -1;
        // thats like... the worst way of doing it but it works
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "Button A: X+%d, Y+%d", &a, &b);
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "Button B: X+%d, Y+%d", &c, &d);
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "Prize: X=%d, Y=%d", &e, &g);
        fgets(buffer, MAX_LINE_LEN, f);
        if (b == -1 || d == -1 || g == -1)
            printf("WARNING : error reading input.\n");
        input[i] = (Machine){(Pos){a, b}, (Pos){c, d}, (Pos){e, g}};
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Machine *input = readInput(argv[1], &size);
    run(13, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}