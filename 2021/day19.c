#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#define MAX_LEN 256

// Wait... how ? Gonna do day20 I guess. 

typedef struct _beacon
{
    int x;
    int y;
    int z;
} Beacon;

typedef struct _scanner
{
    Beacon *beacons;
    int numberOfBeacons;
    int original_x;
    int original_y;
    int original_z;
} Scanner;

Scanner *readInput(FILE *f, fpos_t *start, int *size)
{
    fsetpos(f, start);
    int scannerNb = 0;
    Scanner *liste = NULL;
    char buffer[MAX_LEN];
    while (!feof(f))
    {
        int x, y, z;
        fgets(buffer, MAX_LEN, f);
        if (sscanf(buffer, "--- scanner %d ---", &scannerNb) == 1)
        {
            liste = realloc(liste, sizeof(Scanner) * (scannerNb + 1));
            liste[scannerNb] = (Scanner){NULL, 0, 0, 0, 0};
        }
        else if (sscanf(buffer, "%d,%d,%d", &x, &y, &z) == 3 && liste)
        {
            if (liste[scannerNb].numberOfBeacons == 0 && scannerNb > 0)
            {
                liste[scannerNb].original_x = x;
                liste[scannerNb].original_y = y;
                liste[scannerNb].original_z = z;
            }
            liste[scannerNb].beacons = realloc(liste[scannerNb].beacons, sizeof(Beacon) * (liste[scannerNb].numberOfBeacons + 1));
            liste[scannerNb].beacons[liste[scannerNb].numberOfBeacons] = (Beacon){x - liste[scannerNb].original_x, y - liste[scannerNb].original_y, z - liste[scannerNb].original_z};
            liste[scannerNb].numberOfBeacons++;
        }
    }
    if (!liste)
        *size = 0;
    else
        *size = scannerNb + 1;
    return liste;
}

void freeInput(Scanner *liste, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(liste[i].beacons);
    }
    free(liste);
}

void printScanner(Scanner *scanner)
{
    printf("-- Scanner %d %d %d--\n", scanner->original_x, scanner->original_y, scanner->original_z);
    for (int i = 0; i < scanner->numberOfBeacons; i++)
    {
        Beacon current = scanner->beacons[i];
        printf("%d %d %d", current.x, current.y, current.z);
        printf(" (%d %d %d)\n", current.x + scanner->original_x, current.y + scanner->original_y, current.z + scanner->original_z);
    }
    printf("\n");
}

int main()
{
    FILE *f = fopen("inputs/foo.txt", "r");
    if (!f || f == NULL)
    {
        printf("FNF");
        return 1;
    }
    fpos_t start;
    fgetpos(f, &start);
    int size;
    Scanner *liste = readInput(f, &start, &size);
    for (int i = 0; i < size; i++)
    {
        printScanner(liste + i);
    }


    freeInput(liste, size);
    fclose(f);
    return 0;
}