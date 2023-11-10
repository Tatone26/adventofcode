#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define MAX_LEN 256

typedef struct _better_compte_char
{
    char first;
    int size;
    long int *tab;
} BetterCptChar;

typedef struct _inputs_read
{
    char first;
    char second;
    char to_add;
    char *after_10_steps;
    BetterCptChar *charCount10;
    BetterCptChar *charCount20;
    BetterCptChar *charCount30;
    struct _inputs_read *next;
} InputRead;

void addBetterChar(BetterCptChar *cptChar, char c, long int nb)
{
    /* assert (c <= cptChar->first + cptChar->size - 1);
    assert (c >= cptChar->first); */
    cptChar->tab[(int)(c - cptChar->first)] += nb;
}

BetterCptChar *initBetterCptChar(char *template, InputRead *inputRead, int readInputBool)
{
    char min = template[0], max = template[0];
    for (int i = 0; template[i] != '\n' && template[i] != '\0'; i++)
    {
        if (template[i] < min)
            min = template[i];
        else if (template[i] > max)
            max = template[i];
    }
    InputRead *temp = inputRead;
    while (temp != NULL)
    {
        if (temp->to_add < min)
            min = temp->to_add;
        else if (temp->to_add > max)
            max = temp->to_add;
        temp = temp->next;
    }
    BetterCptChar *result = malloc(sizeof(BetterCptChar));
    result->first = min;
    result->size = (int)(max - min + 1);
    result->tab = calloc(result->size, sizeof(long int));
    /* for (int i = 0; i < result->size; i++)
    {
        result->tab[i] = 0;
    } */
    if (readInputBool)
    {
        for (int i = 0; template[i] != '\n' && template[i] != '\0'; i++)
        {
            addBetterChar(result, template[i], 1);
        }
    }
    return result;
}

void combineBetterChar(BetterCptChar *base, BetterCptChar *toAdd)
{
    /* assert (base->first == toAdd->first);
    assert (base->size == toAdd->size); */
    for (int i = 0; i < base->size; i++)
    {
        base->tab[i] += toAdd->tab[i];
    }
}

InputRead *readInput(FILE *f, fpos_t *start, char **template)
{
    fsetpos(f, start);
    char buffer[MAX_LEN];
    fgets(buffer, MAX_LEN, f);

    *template = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(*template, buffer);

    fgets(buffer, MAX_LEN, f); // Empty line.

    InputRead *result = NULL;

    fgets(buffer, MAX_LEN, f);
    while (!feof(f))
    {
        char first, second, toAdd;
        if (sscanf(buffer, "%c%c -> %c", &first, &second, &toAdd) == 3)
        {
            InputRead *temp = result;
            result = malloc(sizeof(InputRead));
            *result = (InputRead) {first, second, toAdd, NULL, NULL, NULL, NULL, temp};
        }
        fgets(buffer, MAX_LEN, f);
    }
    return result;
}

char *getStringAfterNSteps(char *stringToCompute, InputRead *inputRead, int n)
{
    char *result = strdup(stringToCompute);
    if (n == 0)
        return result;
    int originalSize = strlen(stringToCompute); // + 1 for '\0'
    int offset = 0;
    for (int i = 0; stringToCompute[i] != '\0'; i++)
    {
        result[i + offset] = stringToCompute[i];
        InputRead *temp = inputRead;
        while (temp != NULL)
        {
            if (temp->first == stringToCompute[i] && temp->second == stringToCompute[i + 1])
            {
                offset++;
                result = realloc(result, sizeof(char) * (originalSize + offset + 1));
                result[i + offset] = temp->to_add;
                break;
            }
            temp = temp->next;
        }
    }
    result[originalSize + offset] = '\0'; // magic
    char *trueResult = getStringAfterNSteps(result, inputRead, n - 1);
    free(result);
    return trueResult;
}

void updateCptCharWithString(char *stringInput, InputRead *inputRead, BetterCptChar *countChars, int stepsLeft)
{
    // Steps left is multiple of 10.
    for (int i = 0; stringInput[i] != '\0' && stringInput[i] != '\n'; i++)
    {
        InputRead *inputIter = inputRead;
        while (inputIter != NULL)
        {
            if (inputIter->second == stringInput[i + 1] && inputIter->first == stringInput[i])
            {
                if (stepsLeft == 1)
                {
                    combineBetterChar(countChars, inputIter->charCount10);
                }
                else if (stepsLeft == 2 && inputIter->charCount20 != NULL)
                {
                    combineBetterChar(countChars, inputIter->charCount20);
                    addBetterChar(countChars, inputIter->first, -1);
                    addBetterChar(countChars, inputIter->second, -1);
                }
                else if (stepsLeft == 3 && inputIter->charCount30 != NULL)
                {
                    combineBetterChar(countChars, inputIter->charCount30);
                    addBetterChar(countChars, inputIter->first, -1);
                    addBetterChar(countChars, inputIter->second, -1);
                }
                else
                {
                    updateCptCharWithString(inputIter->after_10_steps, inputRead, countChars, stepsLeft - 1);
                    addBetterChar(countChars, inputIter->first, -1);
                    addBetterChar(countChars, inputIter->second, -1);
                }
                break;
            }
            inputIter = inputIter->next;
        }
        addBetterChar(countChars, stringInput[i], 1);
    }
}

void completeInputRead(InputRead *inputRead)
{
    InputRead *temp = inputRead;
    while (temp != NULL)
    {
        char littleString[3] = {temp->first, temp->second, '\0'};
        temp->after_10_steps = getStringAfterNSteps(littleString, inputRead, 10);
        BetterCptChar *charCount = initBetterCptChar(temp->after_10_steps, inputRead, 1);
        addBetterChar(charCount, temp->first, -1);
        addBetterChar(charCount, temp->second, -1);
        temp->charCount10 = charCount;
        temp = temp->next;
    }
    // the inputRead now has after_10_steps, so can compute after_20
    temp = inputRead;
    while (temp != NULL)
    {
        char littleString[3] = {temp->first, temp->second, '\0'};
        BetterCptChar *after20Cpt = initBetterCptChar(littleString, inputRead, 0);
        updateCptCharWithString(littleString, inputRead, after20Cpt, 2);
        temp->charCount20 = after20Cpt;
        temp = temp->next;
    }
    // And after_30, with the help of after_20
    temp = inputRead;
    while (temp != NULL)
    {
        char littleString[3] = {temp->first, temp->second, '\0'};
        BetterCptChar *after30Cpt = initBetterCptChar(littleString, inputRead, 0);
        updateCptCharWithString(littleString, inputRead, after30Cpt, 3);
        temp->charCount30 = after30Cpt;
        temp = temp->next;
    }
    // And after_40 is done in the main directly.
}

void calculateResult(char *template, InputRead *inputRead, int steps)
{
    BetterCptChar *charCount = initBetterCptChar(template, inputRead, 0); // do not count the template, done in the recursion
    updateCptCharWithString(template, inputRead, charCount, steps / 10);
    printf("After %d steps, most - least = ", steps);
    int min = 0, max = 0; // position in the array.
    long int minC = charCount->tab[0], maxC = charCount->tab[0];
    for (int i = 1; i < charCount->size; i++)
    {
        if (charCount->tab[i] <= 0)
            continue;
        if (charCount->tab[i] < minC)
        {
            minC = charCount->tab[i];
            min = i;
        }
        if (charCount->tab[i] > maxC)
        {
            maxC = charCount->tab[i];
            max = i;
        }
        // printf("%c : %ld\n", (char)(charCount->first + i), charCount->tab[i]);
    }
    printf("%ld, with %c and %c (%ld and %ld)\n", maxC - minC, charCount->first + min, charCount->first + max, charCount->tab[min], charCount->tab[max]);
    free(charCount->tab);
    free(charCount);
}

int main()
{
    FILE *f = fopen("inputs/day14.txt", "r");
    if (!f || f == NULL)
    {
        printf("FNF");
        return 1;
    }
    fpos_t start;
    fgetpos(f, &start);

    char *template;
    InputRead *inputRead = readInput(f, &start, &template);
    completeInputRead(inputRead);
    // printf("%s\n", template);
    // InputRead *temp = inputRead;
    /* while (temp != NULL)
    {
        printf("%c%c -> %c\n", temp->first, temp->second, temp->to_add);
        temp = temp->next;
    } */
    printf("-- Day 14 --\n");
    calculateResult(template, inputRead, 10);
    calculateResult(template, inputRead, 40);

    // Freeing everything
    free(template);
    while (inputRead != NULL)
    {
        InputRead *temp = inputRead->next;
        free(inputRead->after_10_steps);
        free(inputRead->charCount10->tab);
        free(inputRead->charCount20->tab);
        free(inputRead->charCount30->tab);
        free(inputRead->charCount30);
        free(inputRead->charCount20);
        free(inputRead->charCount10);
        free(inputRead);
        inputRead = temp;
    }
    fclose(f);
    return 0;
}