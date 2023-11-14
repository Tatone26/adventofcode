#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

// At least an easy day for speed, but needs a lot of patience to setup right.
// I find my code pretty small and compact, but I know smart can make things so much smaller (but this is very clear in what it is doing.)
// Nothing really hard, if you remember to get a file reader buffer big enough :C
// Always interesting to learn the basics of things like bit manipulation !
// Oh, and ANY other language than C would make this so much easier... But I like to suffer I think (300 lines here for around 50-100 in python for exemple)

// Apparently, people don't like recursion but since there is a relatively small number of packets, I don't see why it would be a problem. 

#define VERSION_POS 0
#define TYPE_ID_POS 3
#define LENGTH_TYPE_ID_POS (VERSION_POS + TYPE_ID_POS + 3)

char *readInput(FILE *f, fpos_t *start)
{
    fsetpos(f, start);
    char buffer[3000];
    fgets(buffer, 3000, f);
    int i;
    for (i = 0; buffer[i] != '\n' && buffer[i] != '\0'; i++)
        ;
    buffer[i] = '\0';
    return strdup(buffer);
}

char transformChar(char a)
// Why... WHY ARE :;<=>?@ BETWEEN THE DIGITS ET THE LETTERS AAAAAA
{
    unsigned mask = (1 << 4) - 1;
    char b;
    if ('0' <= a && a <= '9')
    {
        b = a - '0';
    }
    else if ('A' <= a && a <= 'F')
    {
        b = a - 'A' + '9' + 1;
    }
    else
    {
        printf("Shouldn't be here.\n");
        assert(0);
        return 0; // Yeah.
    }
    return b & mask;
}
// Nothing possible without https://stackoverflow.com/a/8012148, and a lot of luck. 
int getNBit(char *input, int n)
{
    return (input[n / 4] & (1 << (3 - n % 4))) && 1; // yeah it works don't know why lol
}

int getIntFromBits(char *input, int startBit, int stopBit)
// Stopbit is excluded. Pretty proud of this fonction, for someone who never touched any bits operations before.
{
    int result = 0;
    for (int i = startBit; i < stopBit; i++)
    {
        result = result | (getNBit(input, i) << (stopBit - 1 - i));
    }
    return result;
}

int getVersion(char *input, int startBit)
{
    return getIntFromBits(input, VERSION_POS + startBit, VERSION_POS + 3 + startBit);
}

int getTypeID(char *input, int startBit)
{
    return getIntFromBits(input, TYPE_ID_POS + startBit, TYPE_ID_POS + 3 + startBit);
}

int getBitsFromGroup(char *input, int startBit, int groupNb, int *isLastGroup)
{
    int groupStartBit = startBit + 6 + groupNb * 5;
    *isLastGroup = !getNBit(input, groupStartBit);
    int result = getIntFromBits(input, groupStartBit + 1, groupStartBit + 5);
    /* for (int i = startBit + 1; i < startBit + 5; i++)
    {
        printf("%d", getNBit(input, i));
    }
    printf("\n"); */
    return result;
}

long int getLiteralValueFromPacket(char *input, int startBit, int *stopBit)
{
    if (getTypeID(input, startBit) != 4)
    {
        printf("No literal value.\n");
        assert(0);
        return 0;
    }
    int isLastGroup = 0;
    int i = 0;
    long int result = 0;
    while (!isLastGroup)
    {
        result = (result << 4) | (getBitsFromGroup(input, startBit, i, &isLastGroup));
        i++;
        *stopBit = startBit + i * 5 + 6;
    }
    return result;
}

int getLengthTypeID(char *input, int startBit)
{
    if (getTypeID(input, startBit) == 4)
    {
        printf("Not an operator packet !\n");
        assert(0);
        return 0;
    }
    int lengthTypeID = getNBit(input, startBit + LENGTH_TYPE_ID_POS);
    return lengthTypeID;
}

long int readValuePacket(char *input, int startBit, int *stopBit);
long int *getValuesFromOperator(char *input, int startBit, int *stopBit, int *size)
// Takes care of the two cases (Length Type Id 1 or 0) and returns a int tab with all the downward values. 
{
    if (getTypeID(input, startBit) == 4)
        return NULL;
    int lengthTypeID = getLengthTypeID(input, startBit);
    if (lengthTypeID == 1)
    {
        int numberOfSubPackets = getIntFromBits(input, startBit + LENGTH_TYPE_ID_POS + 1, startBit + LENGTH_TYPE_ID_POS + 1 + 11);
        long int *result = malloc(sizeof(long int) * numberOfSubPackets);
        // printf("Operator packet 1 with version %d, with %d subpackets\n", sommeVersions, numberOfSubPackets);
        int currentBit = startBit + LENGTH_TYPE_ID_POS + 1 + 11;
        for (int i = 0; i < numberOfSubPackets; i++)
        {
            result[i] = readValuePacket(input, currentBit, &currentBit);
        }
        *stopBit = currentBit;
        *size = numberOfSubPackets;
        return result;
    }
    else
    {
        int totalLengthInBits = getIntFromBits(input, startBit + LENGTH_TYPE_ID_POS + 1, startBit + LENGTH_TYPE_ID_POS + 1 + 15);
        // printf("Operator packet, typeID 0 with version %d, with %d bits of subpackets.\n", sommeVersions, totalLengthInBits);
        int currentBit = startBit + LENGTH_TYPE_ID_POS + 1 + 15;
        int stopBitSafe = currentBit + totalLengthInBits;
        int i = 0;
        long int *result = malloc(sizeof(long int) * i);
        while (currentBit < stopBitSafe)
        {
            i++;
            result = realloc(result, sizeof(long int) * i);
            result[i - 1] = readValuePacket(input, currentBit, &currentBit);
        }
        *size = i;
        *stopBit = currentBit;
        return result;
    }
}

long int readValuePacket(char *input, int startBit, int *stopBit)
// Very clean and understable fonction. Refer to the problem page if doubts. 
{
    int typeID = getTypeID(input, startBit);
    if (typeID == 4)
        return getLiteralValueFromPacket(input, startBit, stopBit);
    int size;
    long int *listeOfValues = getValuesFromOperator(input, startBit, stopBit, &size);
    long int result = 0;
    // printf("Type ID of Operator : %d\n", typeID);
    switch (typeID)
    {
    case 0: // sum
    {
        long int somme = 0;
        for (int i = 0; i < size; i++)
        {
            somme += listeOfValues[i];
        }
        result = somme;
        break;
    };
    case 1: // multiplication, so need to start at 1 :)
    {
        long int facteur = 1;
        for (int i = 0; i < size; i++)
        {
            facteur *= listeOfValues[i];
        }
        result = facteur;
        break;
    };
    case 2: // minimum
    {
        long int minimum = 5000000;
        for (int i = 0; i < size; i++)
        {
            if (listeOfValues[i] < minimum)
            {
                minimum = listeOfValues[i];
            }
        }
        result = minimum;
        break;
    };
    case 3: // maximum
    {
        long int maximum = 0;
        for (int i = 0; i < size; i++)
        {
            if (listeOfValues[i] > maximum)
            {
                maximum = listeOfValues[i];
            }
        }
        result = maximum;
        break;
    };
    case 5: // greater than
    {
        result = listeOfValues[0] > listeOfValues[1];
        break;
    };
    case 6: // lower than
    {
        result = listeOfValues[0] < listeOfValues[1];
        break;
    };
    case 7: // equals
    {
        result = listeOfValues[0] == listeOfValues[1];
        break;
    };
    default:
        printf("Wrong TypeID : %d\n", typeID);
        assert(0);
    }
    free(listeOfValues);
    // printf("Result of operator : %ld\n", result);
    return result;
}

int readSumVersionPacket(char *input, int startBit, int *stopBit)
// stopBit is useful only in the operator with the recursion version 0 (number of bits) -> I'm not even sure about that but I don't care
{
    if (getTypeID(input, startBit) == 4)
    {
        getLiteralValueFromPacket(input, startBit, stopBit); // to make sure the stopBit is where it needs to be
        // printf("Literal value packet with version %d, representing %ld\n", getVersion(input, startBit), );
        return getVersion(input, startBit);
    }
    else
    {
        int lengthTypeID = getLengthTypeID(input, startBit);
        if (lengthTypeID == 1)
        {
            int sommeVersions = getVersion(input, startBit);
            int numberOfSubPackets = getIntFromBits(input, startBit + LENGTH_TYPE_ID_POS + 1, startBit + LENGTH_TYPE_ID_POS + 1 + 11);
            // printf("Operator packet 1 with version %d, with %d subpackets\n", sommeVersions, numberOfSubPackets);
            int currentBit = startBit + LENGTH_TYPE_ID_POS + 1 + 11;
            for (int i = 0; i < numberOfSubPackets; i++)
            {
                sommeVersions += readSumVersionPacket(input, currentBit, &currentBit);
            }
            *stopBit = currentBit;
            return sommeVersions;
        }
        else
        {
            int sommeVersions = getVersion(input, startBit);
            int totalLengthInBits = getIntFromBits(input, startBit + LENGTH_TYPE_ID_POS + 1, startBit + LENGTH_TYPE_ID_POS + 1 + 15);
            // printf("Operator packet, typeID 0 with version %d, with %d bits of subpackets.\n", sommeVersions, totalLengthInBits);
            int currentBit = startBit + LENGTH_TYPE_ID_POS + 1 + 15;
            int stopBitSafe = currentBit + totalLengthInBits;
            while (currentBit < stopBitSafe)
            {
                sommeVersions += readSumVersionPacket(input, currentBit, &currentBit);
            }
            *stopBit = currentBit;
            return sommeVersions;
        }
    }
}

int main()
{
    FILE *f = fopen("inputs/day16.txt", "r");
    if (!f || f == NULL)
    {
        printf("FNF");
        return 1;
    }
    fpos_t start;
    fgetpos(f, &start);
    char *input = readInput(f, &start);
    // printf("Input : %s --\n", input);
    int size = strlen(input);
    for (int i = 0; i < size; i++)
    {
        input[i] = transformChar(input[i]);
    }
    /* for (int i = 0; i < size * 4; i++)
    {
        printf("%d", getNBit(input, i));
    } */
    int stopBit;
    printf("-- Day 16 --\nSum of verion numbers of all packets : %d\n", readSumVersionPacket(input, 0, &stopBit));
    stopBit = 0;
    printf("Value of the BITS transmission : %ld\n", readValuePacket(input, 0, &stopBit));
    // Free everything
    free(input);
    fclose(f);
    return 0;
}