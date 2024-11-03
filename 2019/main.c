
#include "utils.c"
#include "day1.c"
#include "day2.c"
#include "day3.c"

#define DAYS_DONE 3

int main(int argc, char **argv)
{
    void (*functions[DAYS_DONE])(FILE *f) = {day1, day2, day3};
    char *nomFic[DAYS_DONE] = {"./inputs/day1.txt", "./inputs/day2.txt", "./inputs/day3.txt"};

    int todo[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            int nb = atoi(argv[i]);
            todo[nb - 1] = 1;
        }
    }
    else
    {
        for (int i = 0; i < 25; i++)
        {
            todo[i] = 1;
        }
    }

    for (int i = 0; i < DAYS_DONE; i++)
    {
        if (todo[i])
        {
            FILE *f = fopen(nomFic[i], "r");
            if (f == NULL)
                continue;
            functions[i](f);
            fclose(f);
        }
    }

    return 0;
}