#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>

// BRUTE FORCE, but with acceptable bounds
// -> Apparently, the max Y velocity is equal to -minY of the input. -> When it goes back down, at zero, the velocity is equal to the start velocity.
// That way, if you launch it with a higher velocity, when it goes back down to zero it will just overshot downwards. 

void readInput(FILE *f, fpos_t *start, int tab[4])
{
    fsetpos(f, start);
    if (fscanf(f, "target area: x=%d..%d, y=%d..%d", tab, tab + 1, tab + 2, tab + 3) != 4)
    {
        printf("Error reading input.");
        assert(0);
    }
}

typedef struct _point
{
    int x;
    int y;
} Point;

void nextStep(Point *pos, Point *velocity)
{
    pos->x += velocity->x;
    pos->y += velocity->y;
    (velocity->y)--;
    if (!velocity->x) 
        return;
    (velocity->x)--;
   /*  if (velocity->x < 0) // Will never happen because X alsways positive. (because of the input.)
        (velocity->x) = 0; */
}

int canGoIn(int input[4], const Point *pos, const Point *vel)
{
    return !(vel->y < 0 && pos->y < input[2]); // Can probably be optimised. But 0.050s with file reading and prints is already enough. 
}

int isIn(int input[4], const Point *pos)
{
    return (pos->y >= input[2] && pos->y <= input[3] && pos->x >= input[0] && pos->x <= input[1]);
}

int goesInZone(int input[4], Point initialVelocity, int *highestPoint)
{
    Point pos = (Point){0, 0};
    Point vel = initialVelocity;
    while (canGoIn(input, &pos, &vel))
    {
        nextStep(&pos, &vel);
        if (isIn(input, &pos))
        {
            int newHighestY = (initialVelocity.y * (initialVelocity.y + 1))/2; // Logic. Add initY + initY-1 + initY-2 + ... + 2 + 1 + 0 to find max height. 
            if (*highestPoint < newHighestY)
            {
                *highestPoint = newHighestY;
            }
            return 1;
        }
    }
    return 0;
}

int findBestVelocity(int input[4], int *countVelocities)
{
    *countVelocities = 0;
    int best_y = 0;
    for (int y = input[2]; y <= -input[2]; y++) // why... why is the velocity with the max y = - min alt ???
    {
        for (int x = 0; x <= input[1]; x++) // Because the input is in the X positive, and more will automatically overshot. 
        {
            if (goesInZone(input, (Point){x, y}, &best_y))
            {
                (*countVelocities)++;
                // printf("%d %d goes in at %d %d.\n", x, y, finalPoint.x, finalPoint.y);
            }
        }
    }
    return best_y;
}

int main()
{
    FILE *f = fopen("inputs/day17.txt", "r");
    if (!f || f == NULL)
    {
        printf("FNF");
        return 1;
    }
    fpos_t start;
    fgetpos(f, &start);
    int input[4];
    readInput(f, &start, input);
    /* for (int i = 0; i < 4; i++)
    {
        printf("%d\n", input[i]);
    }
    */
    int countVelocities = 0;
    int maxY = findBestVelocity(input, &countVelocities);
    printf("-- Day 17 --\nMax altitude while still going in : %d\nDistinct initial velocities : %d\n", maxY, countVelocities);

    fclose(f);
    return 0;
}