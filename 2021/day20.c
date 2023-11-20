#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>

// My patience did not allow me to make a fast algo. Just create a big world and iter over everyone everytime.
// This is very bad, considering that all the oustide wolrd (that has nos been "touched" by the image) change the same. 
// Will modify later.

#define MAX_LEN 514 // for \n and \0 I guess
#define LIGHT_PIXEL '#'
#define DARK_PIXEL '.'

char *readAlgorithm(FILE *f, fpos_t *start)
{
    fsetpos(f, start);
    char result[MAX_LEN];
    fgets(result, MAX_LEN, f);
    return strndup(result, strlen(result) - 1);
}

char **readImage(FILE *f, fpos_t *start, int *size)
{
    char buffer[MAX_LEN];
    fsetpos(f, start);
    fgets(buffer, MAX_LEN, f);
    while (buffer[0] != '\n')
        fgets(buffer, MAX_LEN, f);

    char **result = NULL;
    int nb = 0;
    fgets(buffer, MAX_LEN, f);
    while (!feof(f) && buffer[0] != '\n')
    {
        nb++;
        result = realloc(result, sizeof(char *) * nb);
        result[nb - 1] = strndup(buffer, strlen(buffer) - 1);
        fgets(buffer, MAX_LEN, f);
    }
    *size = nb;
    return result;
}

void freeImage(char **image, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(image[i]);
    }
    free(image);
}

char **completeInput(char **image, int size, int toAdd)
{
    char **result = malloc(sizeof(char *) * (size + toAdd * 2)); // Two empty lines up, two empty lines down
    if (!result || size <= 0)
    {
        if (result)
            free(result);
        freeImage(image, size);
        return NULL;
    }
    for (int i = 0; i < size + toAdd * 2; i++)
    {
        result[i] = malloc((strlen(image[0]) + (toAdd * 2 + 1)) * sizeof(char)); // Two empty columns on each side too.
        if (!result[i])
        {
            for (int j = 0; j < i; j++)
                free(result[j]);
            free(result);
            freeImage(image, size);
            return NULL;
        }
        for (int j = 0; j < (int)strlen(image[0]) + (toAdd * 2); j++)
        {
            result[i][j] = DARK_PIXEL;
        }
        result[i][strlen(image[0]) + (toAdd * 2)] = '\0';
    }
    for (int i = 0; i < size; i++)
    {
        if (!result[i])
            continue;
        for (int j = 0; j < (int)strlen(image[i]); j++)
        {
            result[i + toAdd][j + toAdd] = image[i][j];
        }
    }
    freeImage(image, size);
    return result;
}

int getIndex(char **image, int size, int x, int y)
{
    if (x <= 0 || y <= 0 || x >= (int) strlen(image[0]) - 1 || y >= size - 1)
    {
        if (!image[x] || image[x][y] == DARK_PIXEL)
            return 0;
        else
            return 511;
    }
    char pixels[9];
    for (int i = 0; i < 3; i++)
    {
        pixels[i * 3 + 0] = image[y - 1 + i][x - 1];
        pixels[i * 3 + 1] = image[y - 1 + i][x];
        pixels[i * 3 + 2] = image[y - 1 + i][x + 1];
    }
    int index = 0;
    for (int i = 0; i < 9; i++)
    {
        if (pixels[8 - i] == LIGHT_PIXEL)
        {
            index = index | (1 << i);
        }
    }
    return index;
}

char **charDeepCopy(char **image, int size)
{
    char **result = malloc(sizeof(char *) * size);
    if (!result)
        return NULL;
    for (int i = 0; i < size; i++)
    {
        result[i] = strdup(image[i]);
        if (!result[i])
        {
            for (int x = 0; x < i; x++)
                free(result[i]);
            free(result);
            return NULL;
        }
    }
    return result;
}

char **enhanceImage(char *algo, char **image, int size)
{
    char **result = charDeepCopy(image, size);
    if (!result)
    {
        freeImage(image, size);
        return NULL;
    }
    for (int i = 0; i < size; i++)
    {
        if (!result[i])
            continue;
        for (int j = 0; j < (int)strlen(image[i]); j++)
        {
            int index = getIndex(image, size, j, i);
            /* if (index == 0 || index > MAX_LEN)
            {
                continue;
            } */
            char now = algo[index];
            result[i][j] = now;
        }
    }
    freeImage(image, size);
    return result;
}

int nbOfPixelsLit(char **image, int size)
{
    int count = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < (int)strlen(image[i]); j++)
        {
            if (image[i][j] == LIGHT_PIXEL)
                count++;
        }
    }
    return count;
}

int main()
{
    FILE *f = fopen("inputs/day20.txt", "r");
    if (!f || f == NULL)
    {
        printf("FNF");
        return 1;
    }
    fpos_t start;
    fgetpos(f, &start);

    char *algo = readAlgorithm(f, &start); // 512 chars long.
    printf("%s - \n\n", algo);

    int size;
    char **image = readImage(f, &start, &size);
    image = completeInput(image, size, 3);
    size = size + 6;
    if (!image)
    {
        free(algo);
        return 1;
    }

    /* for (int i = 0; i < size; i++)
    {
        printf("%s\n", image[i]);
    }
    printf("\n"); */

    for (int _ = 0; _ < 2; _++)
    {
        image = enhanceImage(algo, image, size);
    }
    printf("-- Day 20 --\nAfter 2 iterations : %d pixels lit.\n", nbOfPixelsLit(image, size));

    freeImage(image, size);

    image = readImage(f, &start, &size);
    image = completeInput(image, size, 51);
    if (!image)
    {
        free(algo);
        return 1;
    }
    size = size + 102;
    for (int _ = 0; _ < 50; _++)
    {
        image = enhanceImage(algo, image, size);
    }
    printf("After 50 iterations : %d pixels lit.\n", nbOfPixelsLit(image, size));

    freeImage(image, size);
    free(algo);
    fclose(f);
    return 0;
}

// Answer between 5971 and 5393 != 5495 != 5551 != 5124 != 5560 != 5569 != 5628
