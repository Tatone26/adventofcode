#include "runner.h"

int comp(const void *a, const void *b)
{
    return (strlen(*(char **)a) - strlen(*(char **)b));
}

int compBis(const void *a, const void *b)
{
    return ((*(char **)a)[0]) - ((*(char **)b)[0]);
}

int charToInt(char c)
{
    switch (c)
    {
    case 'w':
        return 0;
    case 'u':
        return 1;
    case 'b':
        return 2;
    case 'g':
        return 3;
    default:
        return 4;
    }
}

int patternCheck(char *str, char *pat)
{
    int size;
    for (size = 0; pat[size] != '\0'; size++)
        if (str[size] == '\0' || str[size] != pat[size])
            return 0;
    return size;
}

bool isPossible(char **patterns, int nbPatterns, char *test, const int indexes[5])
{
    if (test[0] == '\0')
        return true;
    for (int i = indexes[charToInt(test[0])]; i < nbPatterns && patterns[i][0] == test[0]; i++)
    {
        int offset = patternCheck(test, patterns[i]);
        if (offset)
            if (isPossible(patterns, nbPatterns, test + offset, indexes))
                return true;
    }
    return false;
}

luint part1(void *input_v, void **args)
{
    char **patterns = (char **)input_v;
    int nbPatterns = *((int **)args)[0];
    char **designs = *((char ****)args)[1];
    int nbDesigns = *((int **)args)[2];
    // sorting the patterns by putting the smallest ones first seem to speed up immensely the process here
    char **temp = (char **)malloc(sizeof(char *) * nbPatterns);
    memcpy(temp, patterns, sizeof(char *) * nbPatterns);
    qsort(temp, nbPatterns, sizeof(char *), comp);
    // sorting the patterns by starting character and then getting the indexes (allows looking at a few patterns)
    qsort(temp, nbPatterns, sizeof(char *), compBis);
    int indexes[5] = {-1, -1, -1, -1, -1};
    for (int i = 0; i < nbPatterns; i++)
        if (indexes[charToInt(temp[i][0])] == -1)
            indexes[charToInt(temp[i][0])] = i;

    luint res = 0;

    for (int i = 0; i < nbDesigns; i++)
    {
        if (isPossible(temp, nbPatterns, designs[i], indexes))
            res++;
    }
    free(temp);
    return res;
}

// -----------------------------------------------------------------

#define CACHE_SIZE 65536

typedef struct
{
    long value;
    long hash;
} CacheNode;

/// @brief I have no idea why the unsigned - long - index conversion works. But it does with my input so :shrug:
long hash(char *test)
{
    unsigned x = 7;
    for (int i = 0; test[i] != '\0'; i++)
        x = x * 23 + test[i];
    return x;
}

long isPossibleBis(char **(patterns), int nbPatterns, char *test, CacheNode cache[CACHE_SIZE], const int indexes[5])
{
    if (test[0] == '\0')
        return 1;
    long h = hash(test);
    long hb = h % CACHE_SIZE;
    if (cache[hb].hash == h)
        return cache[hb].value;

    long total = 0;
    for (int i = indexes[charToInt(test[0])]; i < nbPatterns && patterns[i][0] == test[0]; i++)
    {
        int offset = patternCheck(test, patterns[i]);
        if (offset)
            total += isPossibleBis(patterns, nbPatterns, test + offset, cache, indexes);
    }
    cache[hb].hash = h;
    cache[hb].value = total;
    return total;
}

luint part2(void *input_v, void **args)
{
    char **patterns = (char **)input_v;
    int nbPatterns = *((int **)args)[0];
    char **designs = *((char ****)args)[1];
    int nbDesigns = *((int **)args)[2];

    char **temp = (char **)malloc(sizeof(char *) * nbPatterns);
    memcpy(temp, patterns, sizeof(char *) * nbPatterns);
    // qsort(temp, nbPatterns, sizeof(char *), comp);

    // could technically made even better by sorting completely the strings ? I guess ? First character is pretty good for now
    qsort(temp, nbPatterns, sizeof(char *), compBis);
    int indexes[5] = {-1, -1, -1, -1, -1};
    for (int i = 0; i < nbPatterns; i++)
        if (indexes[charToInt(temp[i][0])] == -1)
            indexes[charToInt(temp[i][0])] = i;

    CacheNode cache[CACHE_SIZE] = {0};

    luint res = 0;

    for (int i = 0; i < nbDesigns; i++)
    {
        long t = isPossibleBis(temp, nbPatterns, designs[i], cache, indexes);
        res += t;
    }
    free(temp);
    return res;
}

// ----------------------------------------------------------------

char **readInput(char *filename, int *nbPatterns, char ***designs, int *nbDesigns)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    fgets(buffer, MAX_LINE_LEN, f);

    *nbPatterns = 1;
    for (int c = 0; buffer[c] != '\n' && buffer[c] != '\0'; c++)
        if (buffer[c] == ',')
            (*nbPatterns)++;

    char **patterns = (char **)malloc(*nbPatterns * sizeof(char *));

    int offset = 0;
    for (int i = 0; i < (*nbPatterns) - 1; i++)
    {
        char test[100] = {0};
        if (sscanf(buffer + offset, "%s, ", test) != 1)
        {
            printf("yo there is a problem here\n");
        }
        offset += strlen(test) + 1;
        patterns[i] = strndup(test, strlen(test) - 1);
    }
    patterns[(*nbPatterns) - 1] = strndup(buffer + offset, strlen(buffer + offset) - 1);

    fgets(buffer, MAX_LINE_LEN, f); // empty line
    *nbDesigns = fileSize(f);
    char **des = (char **)malloc(*nbDesigns * sizeof(char *));

    for (int i = 0; i < *nbDesigns; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        des[i] = strndup(buffer, strlen(buffer) - 1);
    }
    *designs = des;

    fclose(f);
    return patterns;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int nbPatterns = 0, nbDesigns = 0;
    char **designs = NULL;
    char **patterns = readInput(argv[1], &nbPatterns, &designs, &nbDesigns);
    void *args[3] = {&nbPatterns, &designs, &nbDesigns};
    run(19, part1, part2, patterns, (void **)args);
    for (int i = 0; i < nbPatterns; i++)
        free(patterns[i]);
    free(patterns);
    for (int j = 0; j < nbDesigns; j++)
        free(designs[j]);
    free(designs);
    return 0;
}