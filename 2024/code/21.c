#include "runner.h"

const char NUMPAD[4][3] = {{'7', '8', '9'}, {'4', '5', '6'}, {'1', '2', '3'}, {0, '0', 'A'}};
const char DIRPAD[2][3] = {{0, '^', 'A'}, {'<', 'v', '>'}};

// 212708 too high

Pos getPosNumPad(char c)
{
    switch (c)
    {
    case 'A':
        return (Pos){2, 3};
    case '0':
        return (Pos){1, 3};
    case '1':
        return (Pos){0, 2};
    case '2':
        return (Pos){1, 2};
    case '3':
        return (Pos){2, 2};
    case '4':
        return (Pos){0, 1};
    case '5':
        return (Pos){1, 1};
    case '6':
        return (Pos){2, 1};
    case '7':
        return (Pos){0, 0};
    case '8':
        return (Pos){1, 0};
    case '9':
        return (Pos){2, 0};
    default:
        return (Pos){0, 3};
    }
}

Pos getPosDirPad(char c)
{
    switch (c)
    {
    case 'A':
        return (Pos){2, 0};
    case '^':
        return (Pos){1, 0};
    case '<':
        return (Pos){0, 1};
    case 'v':
        return (Pos){1, 1};
    case '>':
        return (Pos){2, 1};
    default:
        return (Pos){0, 0};
    }
}

typedef struct
{
    char input[5][5];
} Codes;

char *shortestNumPath(char *code, int codeSize)
{
    char *path = (char *)malloc(sizeof(char) * 2048);

    int offset = 0;
    Pos currentPos = getPosNumPad('A');
    Pos nonPos = getPosNumPad(0);
    for (int i = 0; i < codeSize; i++)
    {
        char temp[100] = {0};
        int n = 0;
        Pos next = getPosNumPad(code[i]);
        Pos dpos = (Pos){next.x - currentPos.x, next.y - currentPos.y};
        Pos cpos = currentPos;
        while (dpos.x != 0 && dpos.y != 0)
        {
            int r = rand() % 4;
            int a = (r + i) % 4;
            if (a == 0 && dpos.y < 0 && (cpos.y - 1 != nonPos.y || cpos.x != nonPos.x))
            {
                temp[n++] = '^';
                dpos.y++;
                cpos.y--;
                continue;
            }
            if (a == 1 && dpos.y > 0 && (cpos.y + 1 != nonPos.y || cpos.x != nonPos.x))
            {
                temp[n++] = 'v';
                dpos.y--;
                cpos.y++;
                continue;
            }
            if (a == 2 && dpos.x > 0 && (cpos.x + 1 != nonPos.x || cpos.y != nonPos.y))
            {
                temp[n++] = '>';
                dpos.x--;
                cpos.x++;
                continue;
            }
            if (a == 3 && dpos.x < 0 && (cpos.x - 1 != nonPos.x || cpos.y != nonPos.y))
            {
                temp[n++] = '<';
                dpos.x++;
                cpos.x--;
                continue;
            }
        }
        while (dpos.x != 0)
        {
            if (dpos.x > 0)
            {
                temp[n++] = '>';
                dpos.x--;
                cpos.x++;
                continue;
            }
            if (dpos.x < 0)
            {
                temp[n++] = '<';
                dpos.x++;
                cpos.x--;
                continue;
            }
        }
        while (dpos.y != 0)
        {
            if (dpos.y < 0)
            {
                temp[n++] = '^';
                dpos.y++;
                cpos.y--;
                continue;
            }
            if (dpos.y > 0)
            {
                temp[n++] = 'v';
                dpos.y--;
                cpos.y++;
                continue;
            }
        }
        temp[n++] = 'A';
        memcpy(path + offset, temp, sizeof(char) * n);
        offset += n;
        currentPos = next;
    }

    return path;
}

char *shortestDirPath(char *code, int codeSize)
{
    char *path = (char *)calloc(2048, sizeof(char));

    int offset = 0;
    Pos currentPos = getPosDirPad('A');
    Pos nonPos = getPosDirPad(0);
    for (int i = 0; i < codeSize; i++)
    {
        char temp[100] = {0};
        int n = 0;
        Pos next = getPosDirPad(code[i]);
        Pos dpos = (Pos){next.x - currentPos.x, next.y - currentPos.y};
        Pos cpos = currentPos;
        while (dpos.x != 0 && dpos.y != 0)
        {
            int r = rand() % 4;
            int a = (r + i) % 4;
            if (a == 0 && dpos.y < 0 && (cpos.y - 1 != nonPos.y || cpos.x != nonPos.x))
            {
                temp[n++] = '^';
                dpos.y++;
                cpos.y--;
                continue;
            }
            if (a == 1 && dpos.y > 0 && (cpos.y + 1 != nonPos.y || cpos.x != nonPos.x))
            {
                temp[n++] = 'v';
                dpos.y--;
                cpos.y++;
                continue;
            }
            if (a == 2 && dpos.x > 0 && (cpos.x + 1 != nonPos.x || cpos.y != nonPos.y))
            {
                temp[n++] = '>';
                dpos.x--;
                cpos.x++;
                continue;
            }
            if (a == 3 && dpos.x < 0 && (cpos.x - 1 != nonPos.x || cpos.y != nonPos.y))
            {
                temp[n++] = '<';
                dpos.x++;
                cpos.x--;
                continue;
            }
        }
        while (dpos.x != 0)
        {
            if (dpos.x > 0)
            {
                temp[n++] = '>';
                dpos.x--;
                cpos.x++;
                continue;
            }
            if (dpos.x < 0)
            {
                temp[n++] = '<';
                dpos.x++;
                cpos.x--;
                continue;
            }
        }
        while (dpos.y != 0)
        {
            if (dpos.y < 0)
            {
                temp[n++] = '^';
                dpos.y++;
                cpos.y--;
                continue;
            }
            if (dpos.y > 0)
            {
                temp[n++] = 'v';
                dpos.y--;
                cpos.y++;
                continue;
            }
        }
        temp[n++] = 'A';
        memcpy(path + offset, temp, sizeof(char) * n);
        offset += n;
        currentPos = next;
    }

    return path;
}

luint part1(void *input_v, void **args)
{
    Codes p = *(Codes *)input_v;
    int size = ((int *)args)[0];

    srand(time(NULL));

    luint total = 0;
    for (int i = 0; i < size; i++)
    {
        printf("a\n");
        int min = __INT_MAX__;
        for (int _ = 0; _ < 10000000; _++)
        {
            char *path = shortestNumPath(p.input[i], 4);
            // printf("%s : %s\n", p.input[i], path);
            char *path2 = shortestDirPath(path, strlen(path));
            // printf("%s\n", path2);
            char *path3 = shortestDirPath(path2, strlen(path2));
            /* printf("%s\n", path3);
            printf("%s -> size : %d, number %d\n", p.input[i], strlen(path3), atoi(p.input[i])); */
            int test = (strlen(path3)) * atoi(p.input[i]);
            if (test < min)
            {
                printf("%s : found %d\n", p.input[i], strlen(path3));
                min = test;
            }
            else
            {
                // printf("%d\n", strlen(path3));
            }
            free(path);
            free(path2);
            free(path3);
        }
        total += min;
    }
    return total;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    Codes p = *(Codes *)input_v;
    int size = ((int *)args)[0];

    return 0;
}

// ----------------------------------------------------------------

Codes readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    *size = 5;
    Codes input = {0};
    for (int i = 0; i < 5; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        memcpy(input.input[i], buffer, sizeof(char) * 4);
        input.input[i][4] = '\0';
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Codes input = readInput(argv[1], &size);
    run(21, part1, part2, &input, (void **)&size);
    return 0;
}