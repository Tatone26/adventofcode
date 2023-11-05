#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define PATTERNS 10 // le nb de unique signal patterns before |
#define DIGITS 4    // nb de digits après |
#define MAX_LEN 256

char ***readInput(FILE *f, fpos_t *start, int *size)
{
    // Not the easiest read I might say.
    int nbOfEntries = 0;

    char buffer[MAX_LEN];
    fsetpos(f, start);
    fgets(buffer, MAX_LEN, f); // first line, so if empty file it is well cared for
    while (!feof(f))
    {
        nbOfEntries++;
        fgets(buffer, MAX_LEN, f);
    }

    *size = nbOfEntries;
    char ***entries = malloc(sizeof(char **) * nbOfEntries);
    assert(entries != NULL);

    fsetpos(f, start);
    for (int i = 0; i < nbOfEntries; i++)
    {
        entries[i] = malloc(sizeof(char *) * (PATTERNS + DIGITS));
        assert(entries[i] != NULL);
        fgets(buffer, MAX_LEN, f);
        // printf("%s\n", buffer);

        int word_size = 1;
        int iter = 0;
        int s = 0;
        int count = 0;
        while (buffer[iter] != '\0' && buffer[iter] != '\n')
        {
            iter++;
            // printf("%c", buffer[iter]);
            if (buffer[iter] == ' ' || buffer[iter] == '\0' || buffer[iter] == '\n')
            {
                if (word_size > 1)
                {
                    char *new_word = malloc(sizeof(char) * (word_size + 1));

                    new_word = strncpy(new_word, buffer + s, word_size + 1);
                    new_word[word_size] = '\0';

                    entries[i][count] = new_word;
                    s = iter + 1;
                    count++;
                }
                else
                {
                    s = iter + 1;
                }
                word_size = 0;
            }
            else
            {
                word_size++;
            }
        }
    }
    return entries;
}

void freeInputs(char ***inputs, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int x = 0; x < DIGITS + PATTERNS; x++)
        {
            free(inputs[i][x]);
        }
        free(inputs[i]);
    }
    free(inputs);
}

char *charactersForUniqueNumber(char **input, int n)
{
    long unsigned int l = 0;
    switch (n)
    {
    case 1:
        l = 2;
        break;
    case 4:
        l = 4;
        break;
    case 7:
        l = 3;
        break;
    case 8:
        l = 7;
        break;
    }
    char *result;
    for (int x = 0; x < PATTERNS; x++)
    {
        if (strlen(input[x]) == l)
        {
            result = input[x];
            break;
        }
    }
    return result;
}

int missingCharactersIn(char *test, char *fixed)
{
    // Returns the number of characters of fixed missing in test.
    int it = 0;
    int missing = 0;
    while (fixed[it] != '\0')
    {
        if (strchr(test, fixed[it]) == NULL)
            missing++;
        it++;
    }
    return missing;
}

int decryptLine(char **numbers)
{ // size is still DIGITS + PATTERNS

    int result = 0;
    for (int i = PATTERNS; i < PATTERNS + DIGITS; i++)
    {
        int l = strlen(numbers[i]);
        int n = 0;
        switch (l)
        {
        case 2:
            n = 1;
            break;
        case 4:
            n = 4;
            break;
        case 3:
            n = 7;
            break;
        case 7:
            n = 8;
            break;
        case 6:
        {
            // Finding which characters correspond to 4.
            char *four = charactersForUniqueNumber(numbers, 4);
            // Are all these characters in numbers[i] ? if yes then it's a 9 else it's a 6 or a 0
            if (!missingCharactersIn(numbers[i], four))
                n = 9;
            else
            {
                // If all of 7 char are in it, then it's a 0, else a 6
                char *seven = charactersForUniqueNumber(numbers, 7);
                n = !missingCharactersIn(numbers[i], seven) ? 0 : 6;
            }
        }
        break;
        case 5:
        { // 2, 3 or 5
            char *one = charactersForUniqueNumber(numbers, 1);
            // Are all these characters in numbers[i] ? if yes then it's a 3 else it's a 2 or 5
            if (!missingCharactersIn(numbers[i], one))
                n = 3;
            else
            { // is it a 2 or a 5 ?
                char *four = charactersForUniqueNumber(numbers, 4);
                // If there is ONE character that is not in 4 then it's a 5, is there is TWO not in four then it's a 4
                n = missingCharactersIn(numbers[i], four) >= 2 ? 2 : 5;
            }
        }
        break;
        }
        result = result * 10 + n;
    }
    return result;
}

int main()
{

    FILE *f = fopen("day8.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    int size;
    char ***inputs = readInput(f, &start, &size);

    int countResult = 0;
    for (int i = 0; i < size; i++)
    {
        for (int x = PATTERNS; x < DIGITS + PATTERNS; x++)
        {
            int l = strlen(inputs[i][x]);
            if (l == 2 || l == 4 || l == 3 || l == 7)
                countResult++; // si c'est un 1, un 4, un 7 ou un 8
        }
    }

    printf("-- Day 8 -- \nInstances of 'unique' digits : %d\n", countResult);

    int sumResults = 0;
    for (int i = 0; i < size; i++)
    {
        int s = decryptLine(inputs[i]);
        sumResults += s;
    }

    printf("Sum of all ouputs values : %d\n", sumResults);

    freeInputs(inputs, size);

    fclose(f);

    return 0;
}