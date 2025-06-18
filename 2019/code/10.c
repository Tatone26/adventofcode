#include "runner.h"

typedef int Vec2[2];
typedef struct
{
    int dx, dy;
    double angle;
} DirAngle;

luint part1(void *input_v, void **args)
{
    bool **input = (bool **)input_v;
    int height = ((int *)args)[0];
    int width = ((int *)args)[1];

    int best_find = 0;

    bool *dirs = (bool *)malloc(height * width * sizeof(bool));
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            if (!input[y][x])
                continue;
            // For every asteroid, need to find which DIRECTIONS have an asteroid visible. DONT CARE about WHAT asteroid is visible.
            memset(dirs, 0, sizeof(bool) * height * width);
            int count = 0;
            for (int a = 0; a < height; a++)
                for (int b = 0; b < width; b++)
                {
                    // look at all other asteroids
                    if ((a == y && b == x) || !input[a][b])
                        continue;
                    // take the direction to this asteroid
                    int dx = b - x;
                    int dy = a - y;
                    int gcd = pgcd(dx, dy);
                    // here is the normalized direction (as small as possible)
                    dx = dx / gcd;
                    dy = dy / gcd;
                    // if we have already seen this, skip it.
                    bool seen = false;
                    for (int i = 0; i < count; i++)
                        if (dirs[(dy + y) * width + (dx + x)])
                        {
                            seen = true;
                            break;
                        }

                    if (!seen)
                    {
                        dirs[(dy + y) * width + (dx + x)] = 1;
                        count++;
                    }
                }
            if (count > best_find)
                best_find = count;
        }
    free(dirs);
    return best_find;
}

// -----------------------------------------------------------------

int cmp(const void *a, const void *b)
{
    double da = ((DirAngle *)a)->angle;
    double db = ((DirAngle *)b)->angle;
    return (da > db) - (da < db);
}

luint part2(void *input_v, void **args)
{
    bool **input = (bool **)input_v;
    int height = ((int *)args)[0];
    int width = ((int *)args)[1];

    int best_x = 0;
    int best_y = 0;
    int best_find = 0;

    bool *test_dirs = (bool *)malloc(sizeof(bool) * height * width);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            if (!input[y][x])
                continue;
            // For every asteroid, need to find which DIRECTIONS have an asteroid visible. DONT CARE about WHAT asteroid is visible.
            memset(test_dirs, 0, sizeof(bool) * height * width);
            int count = 0;
            for (int a = 0; a < height; a++)
                for (int b = 0; b < width; b++)
                {
                    // look at all other asteroids
                    if ((a == y && b == x) || !input[a][b])
                        continue;
                    // take the direction to this asteroid
                    int dx = b - x;
                    int dy = a - y;
                    int gcd = pgcd(dx, dy);
                    // here is the normalized direction (as small as possible)
                    dx = dx / gcd;
                    dy = dy / gcd;
                    // if we have already seen this, skip it.
                    bool seen = false;
                    for (int i = 0; i < count; i++)
                        if (test_dirs[(dy + y) * width + (dx + x)])
                        {
                            seen = true;
                            break;
                        }

                    if (!seen)
                    {
                        test_dirs[(dy + y) * width + (dx + x)] = 1;
                        count++;
                    }
                }
            if (count > best_find)
            {
                best_find = count;
                best_x = x;
                best_y = y;
            }
        }
    free(test_dirs);

    int nb_dirs = 0;
    DirAngle *dirs = (DirAngle *)malloc(sizeof(DirAngle) * width * height);
    // getting all possible directions
    for (int a = 0; a < height; a++)
        for (int b = 0; b < width; b++)
        {
            // look at all other asteroids
            if ((a == best_y && b == best_x) || !input[a][b])
                continue;
            // take the direction to this asteroid
            int dx = b - best_x;
            int dy = a - best_y;
            int gcd = pgcd(dx, dy);
            // here is the normalized direction (as small as possible)
            dx = dx / gcd;
            dy = dy / gcd;
            // if we have already seen this, skip it.
            bool seen = false;
            for (int i = 0; i < nb_dirs; i++)
                if (dirs[i].dx == dx && dirs[i].dy == dy)
                {
                    seen = true;
                    break;
                }

            if (!seen)
            {
                dirs[nb_dirs].dx = dx;
                dirs[nb_dirs].dy = dy;
                dirs[nb_dirs].angle = atan2(dx, -dy); // thanks chatgpt, apparently this gives all angles with "up" being 0.
                if (dirs[nb_dirs].angle < 0)
                    dirs[nb_dirs].angle += 2 * 3.14159265358979323846;
                nb_dirs++;
            }
        }

    if (!nb_dirs)
    {
        free(dirs);
        return 0;
    }

    // sorting directions now
    qsort(dirs, nb_dirs, sizeof(DirAngle), cmp);

    // now just get rid of asteroids for each direction one by one, 200 times
    // modifying input directly but hey who cares
    int destroyed = 0;
    Vec2 last_destroyed = {0, 0};
    for (int i = 0; destroyed < 200; i++)
    {
        DirAngle dir = dirs[i % nb_dirs];
        Vec2 to_destroy = {best_x + dir.dx, best_y + dir.dy};
        while (to_destroy[0] >= 0 && to_destroy[1] >= 0 && to_destroy[0] < width && to_destroy[1] < height)
        {
            if (input[to_destroy[1]][to_destroy[0]])
            {
                input[to_destroy[1]][to_destroy[0]] = 0;
                destroyed++;
                last_destroyed[0] = to_destroy[0];
                last_destroyed[1] = to_destroy[1];
                break;
            }
            to_destroy[0] += dir.dx;
            to_destroy[1] += dir.dy;
        }
    }

    free(dirs);

    return last_destroyed[0] * 100 + last_destroyed[1];
}

// ----------------------------------------------------------------

bool **readInput(char *filename, int *height, int *width)
{
    char buffer[MAX_LINE_LEN] = {0};

    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return 0;
    *height = fileSize(f);
    bool **input = (bool **)malloc(sizeof(bool *) * *height);

    fpos_t start;
    fgetpos(f, &start);
    if (!fgets(buffer, MAX_LINE_LEN, f))
    {
        free(input);
        fclose(f);
        return NULL;
    }
    *width = strlen(buffer) - 1;
    fsetpos(f, &start);

    int y = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
    {
        bool *line = (bool *)malloc(sizeof(bool) * *width);
        for (int i = 0; i < *width; i++)
            line[i] = (buffer[i] == '#');
        input[y++] = line;
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int height = 0, width = 0;
    bool **input = readInput(argv[1], &height, &width);
    int sizes[2] = {height, width};
    run(10, part1, part2, input, (void **)&sizes);
    for (int i = 0; i < height; i++)
        free(input[i]);
    free(input);
    return 0;
}