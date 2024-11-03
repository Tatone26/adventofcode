#include "utils.h"

typedef struct
{
    Direction d;
    int move;
} Action;

Direction charToDir(char c)
{
    switch (c)
    {
    case 'R':
        return Right;
    case 'L':
        return Left;
    case 'U':
        return Up;
    case 'D':
        return Down;
    default:
        return None;
    }
}

Action *readLine(char *buffer, int *size)
{
    *size = sizeOfList(buffer, ',');
    Action *line = (Action *)malloc(sizeof(Action) * (*size));
    int pos = 0;
    for (int i = 0; i < *size; i++)
    {
        line[i] = (Action){charToDir(*(buffer + pos)),
                           atoi(buffer + pos + 1)};
        while (buffer[pos] != ',')
            pos++;
        pos++;
    }
    return line;
}

typedef struct _point
{
    int x;
    int y;
    struct _point *next;
} Point;

#define HASH_SIZE 350000

typedef struct _hash_points
{
    Point *tab[HASH_SIZE];
} HashPoints;

void freeHash(HashPoints *h)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        Point *temp = h->tab[i];
        while (temp)
        {
            Point *temp2 = temp->next;
            free(temp);
            temp = temp2;
        }
    }
    free(h);
}

int clef(int x, int y)
{
    return y + (x + y) * (x + y + 1) / 2;
}

/* Position dans la table de hachage */
int hachage(int x, int y)
{
    double ka = clef(x, y) * ((sqrt(5) - 1) / 2);
    return abs((int)(HASH_SIZE * (ka - (int)ka)));
}
/* int hachage(int x, int y)
{
    return abs(clef(x, y) % HASH_SIZE);
} */

void insertIntoHash(HashPoints *h, int x, int y)
{
    int hash = hachage(x, y);
    Point *p = (Point *)malloc(sizeof(Point));
    p->x = x;
    p->y = y;
    p->next = h->tab[hash];
    h->tab[hash] = p;
}

int isInHash(HashPoints *h, int x, int y)
{
    int hash = hachage(x, y);
    Point *temp = h->tab[hash];
    while (temp)
    {
        if (temp->x == x && temp->y == y)
            return 1;
        temp = temp->next;
    }
    return 0;
}

void day3(FILE *f)
{
    char buffer[5000];
    fgets(buffer, 5000, f);
    int sizeOne;
    Action *firstLine = readLine(buffer, &sizeOne);
    fgets(buffer, 5000, f);
    int sizeTwo;
    Action *secondLine = readLine(buffer, &sizeTwo);

    HashPoints *h = (HashPoints *)malloc(sizeof(HashPoints));
    for (int i = 0; i < HASH_SIZE; i++)
    {
        h->tab[i] = NULL;
    }

    int posx = 0, posy = 0;
    for (int i = 0; i < sizeOne; i++)
    {
        for (int j = 0; j < firstLine[i].move; j++)
        {
            switch (firstLine[i].d)
            {
            case Up:
                posy--;
                break;
            case Down:
                posy++;
                break;
            case Left:
                posx--;
                break;
            case Right:
                posx++;
                break;
            case None:
                break;
            }
            insertIntoHash(h, posx, posy);
        }
    }

    int distance = 0;

    posx = 0;
    posy = 0;
    for (int i = 0; i < sizeTwo; i++)
    {
        for (int j = 0; j < secondLine[i].move; j++)
        {
            switch (secondLine[i].d)
            {
            case Up:
                posy--;
                break;
            case Down:
                posy++;
                break;
            case Left:
                posx--;
                break;
            case Right:
                posx++;
                break;
            case None:
                break;
            }
            int d = abs(posx) + abs(posy);
            if ((distance == 0 || d < distance) && isInHash(h, posx, posy))
            {
                distance = d;
            }
        }
    }

    printf("Day 3 -- Part 1 : %d\n", distance);

    freeHash(h);
    free(firstLine);
    free(secondLine);
}