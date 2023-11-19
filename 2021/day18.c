#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>

// Pfiou. What a long day in C. I'm never writing very memory-safe code like that after I finish that thing. 
// But still not really difficult, once you figure out what the two task need to do...
// Writing this sortof binary tree without enums like in Rust or any fonctional langages is really a pain !
// Should probably have gone with (value, depth) like everybody, or even string manipulation... But na it was fun I like suffering
// I'm gonna make it guys. Just seven problems left. And day12 (I don't want to do day12)

#define MAX_LEN 500

typedef struct _snailfish Snailfish;
struct _snailfish // some kind of binary tree, with a link to the parent too.
{
    Snailfish *parent;
    Snailfish *left_snail;
    Snailfish *right_snail;
    int number; // -1 if nothing there
};

void printSnailFish(Snailfish *snail)
{
    if (snail == NULL)
    {
        printf("(NULL)");
        return;
    }
    if (!(snail->left_snail) && !(snail->right_snail))
    {
        printf("%d", snail->number);
        return;
    }
    printf("[");
    printSnailFish(snail->left_snail);
    printf(",");
    printSnailFish(snail->right_snail);
    printf("]");
}

Snailfish *createSnailFish(Snailfish *parent, Snailfish *left, Snailfish *right, int number)
{
    Snailfish *result = malloc(sizeof(Snailfish));
    if (!result)
        return NULL;
    (*result) = (Snailfish){parent, left, right, number};
    return result;
}

void freeSnailFish(Snailfish *snail)
{
    if (!snail)
        return;
    if (snail->left_snail)
        freeSnailFish(snail->left_snail);
    if (snail->right_snail)
        freeSnailFish(snail->right_snail);
    free(snail);
}

Snailfish *readString(char *input)
{
    if (strlen(input) == 1)
    {
        Snailfish *result = createSnailFish(NULL, NULL, NULL, atoi(input));
        return result;
    }
    Snailfish *left_snail = NULL;
    Snailfish *right_snail = NULL;
    int i = 0;
    if (input[i] != '[')
        return NULL;
    int count_opened = 0;
    int count_closed = 0;
    int separation_pos = 0;
    while (input[i] != '\0' && input[i] != '\n')
    {
        i++;
        if (input[i] == '[')
            count_opened++;
        else if (input[i] == ']')
            count_closed++;
        else if (input[i] == ',' && count_closed == count_opened)
        {
            separation_pos = i;
            break;
        }
    }

    char *left = strndup(input + 1, separation_pos - 1);
    left_snail = readString(left);
    free(left);
    if (!left_snail)
        return NULL;

    char *right = strdup(input + separation_pos + 1);
    right[strlen(right) - 1] = '\0';
    right_snail = readString(right);
    free(right);
    if (!right_snail)
    {
        freeSnailFish(left_snail);
        return NULL;
    }

    Snailfish *result = createSnailFish(NULL, left_snail, right_snail, -1);
    if (!result)
    {
        freeSnailFish(left_snail);
        freeSnailFish(right_snail);
        return NULL;
    }
    left_snail->parent = result;
    right_snail->parent = result;
    return result;
}

Snailfish **readInput(FILE *f, fpos_t *start, int *size)
{
    *size = 0;
    fsetpos(f, start);
    char buffer[MAX_LEN];
    Snailfish **liste = malloc(sizeof(Snailfish *) * 1);
    if (!liste)
        return NULL;
    int i = 0;
    while (!feof(f)) // goes to the n line
    {
        if (fgets(buffer, MAX_LEN, f) == NULL || buffer[0] == '\n')
            break;
        liste = realloc(liste, sizeof(Snailfish *) * (i + 1));
        char *lineToTransform = strndup(buffer, strlen(buffer) - 1);
        liste[i] = readString(lineToTransform);
        free(lineToTransform);
        i++;
    }
    *size = i;
    return liste;
}

Snailfish *nextRightNumber(Snailfish *first)
{

    Snailfish *tempParent = first->parent;
    Snailfish *tempChild = first;
    while (tempParent && tempParent->right_snail == tempChild)
    {
        tempChild = tempParent;
        tempParent = tempParent->parent;
    }
    if (!tempParent)
        return NULL;
    Snailfish *tempBro = tempParent->right_snail;
    while (tempBro && tempBro->number == -1)
        tempBro = tempBro->left_snail;
    return tempBro;
}

Snailfish *nextLeftNumber(Snailfish *first)
{
    Snailfish *tempParent = first->parent;
    Snailfish *tempChild = first;
    while (tempParent && tempParent->left_snail == tempChild)
    {
        tempChild = tempParent;
        tempParent = tempParent->parent;
    }
    if (!tempParent)
        return NULL;
    Snailfish *tempBro = tempParent->left_snail;
    while (tempBro && tempBro->number == -1)
        tempBro = tempBro->right_snail;
    return tempBro;
}

int explodes(Snailfish *input, int depth)
{
    if (!input)
        return 0;
    if (depth == 4 && input->number != -1)
        return 0;
    else if (depth < 4)
    {
        if (input->number != -1)
            return 0;
        return explodes(input->left_snail, depth + 1) || explodes(input->right_snail, depth + 1);
    }
    if (depth == 4 && input->number == -1 && input->left_snail->number != -1 && input->right_snail->number != -1)
    {
        // boum

        Snailfish *nextRight = nextRightNumber(input);
        Snailfish *nextLeft = nextLeftNumber(input);
        if (nextRight) // else : not found so do nothing
            nextRight->number += input->right_snail->number;
        if (nextLeft)
            nextLeft->number += input->left_snail->number;

        Snailfish *newSnail = createSnailFish(input->parent, NULL, NULL, 0);
        if (input->parent->left_snail == input)
            input->parent->left_snail = newSnail;
        else if (input->parent->right_snail == input)
            input->parent->right_snail = newSnail;
        else
            freeSnailFish(newSnail);
        freeSnailFish(input);
        return 1;
    }
    return 0;
}

int splits(Snailfish *input)
{
    if (!input)
        return 0;
    if (input->number >= 10)
    {
        int leftNumber = input->number / 2;
        int rightNumber = input->number / 2 + input->number % 2;
        assert(input->number == leftNumber + rightNumber);
        Snailfish *newSnail = createSnailFish(input->parent, NULL, NULL, -1);
        newSnail->left_snail = createSnailFish(newSnail, NULL, NULL, leftNumber);
        newSnail->right_snail = createSnailFish(newSnail, NULL, NULL, rightNumber);
        if (input->parent->left_snail == input)
            input->parent->left_snail = newSnail;
        else if (input->parent->right_snail == input)
            input->parent->right_snail = newSnail;
        free(input);
        return 1;
    }
    return splits(input->left_snail) || splits(input->right_snail);
}

Snailfish *copySnail(Snailfish *input)
{
    Snailfish *result = createSnailFish(NULL, NULL, NULL, input->number);
    if (input->left_snail)
    {
        result->left_snail = copySnail(input->left_snail);
        result->left_snail->parent = result;
    }
    if (input->right_snail)
    {
        result->right_snail = copySnail(input->right_snail);
        result->right_snail->parent = result;
    }
    return result;
}

/// @brief
/// @param old is modified
/// @param new is copied
/// @return
Snailfish *addSnails(Snailfish *old, Snailfish *new)
{
    if (!old)
    {
        return copySnail(new);
    }
    Snailfish *result = createSnailFish(NULL, old, copySnail(new), -1);
    if (!result)
        return NULL;
    result->left_snail->parent = result;
    result->right_snail->parent = result;
loop:
{
    if (explodes(result, 0))
        goto loop;
    if (splits(result))
        goto loop;
}
    return result;
}

int magnitude(Snailfish *input)
{
    if (!input)
        return 0;
    if (input->number != -1)
        return input->number;
    else
        return 3 * magnitude(input->left_snail) + 2 * magnitude(input->right_snail);
}

int main()
{
    FILE *f = fopen("inputs/day18.txt", "r");
    if (!f || f == NULL)
    {
        printf("FNF");
        return 1;
    }
    fpos_t start;
    fgetpos(f, &start);

    int size = 0;
    Snailfish **listOfInputs = readInput(f, &start, &size);

    /*  for (int i = 0; i < size; i++)
     {
         printSnailFish(listOfInputs[i]);
         printf("\n");
     } */
    Snailfish *result = NULL;
    for (int i = 0; i < size; i++)
    {
        result = addSnails(result, listOfInputs[i]);
    }

    printf("-- Day 18 --\nMagnitude of the sum of all snailfish numbers : %d\n", magnitude(result));
    freeSnailFish(result);

    int maxMagnitude = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i != j)
            {
                Snailfish *binome = addSnails(copySnail(listOfInputs[i]), listOfInputs[j]);
                int magn = magnitude(binome);
                if (magn > maxMagnitude)
                    maxMagnitude = magn;
                freeSnailFish(binome);
            }
        }
    }

    printf("Max magnitude with only two numbers : %d\n", maxMagnitude);

    for (int i = 0; i < size; i++)
    {
        freeSnailFish(listOfInputs[i]);
    }
    free(listOfInputs);
    fclose(f);
    return 0;
}