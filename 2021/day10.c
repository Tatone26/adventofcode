#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define MAX_LEN 256

// Int overflow. Really. Not nice. 

int score(char c) {
    switch (c) {
        case ')' : return 3;
        case ']' : return 57;
        case '}' : return 1197;
        case '>' : return 25137;
        default : return 0;
    }
}

int isOpenOrClose(char c) {
    // 1 for open, -1 for close, 0 for illegal.
    switch (c) {
        case '(' : return 1;
        case '{' : return 1;
        case '[' : return 1;
        case '<' : return 1;
        case ')' : return -1;
        case '}' : return -1;
        case ']' : return -1;
        case '>' : return -1;
        default : return 0;
    }
}

char variant(char c) {
    switch (c) {
        case ')' : return '(';
        case ']' : return '[';
        case '}' : return '{';
        case '>' : return '<';
        case '(' : return ')';
        case '[' : return ']';
        case '{' : return '}';
        case '<' : return '>';
        default : return 0;
    }
}

char firstIllegalClosing(char *line) {
    int size = strlen(line);
    char openedCharacters[size];
    int opened = 0;
    int iter = 0;
    while (line[iter] != '\n') {
        char c = line[iter];
        int typeOfBracket = isOpenOrClose(c);
        if (!typeOfBracket) {
            printf("Found illegal caracter in %s at pos %d ! \n", line, iter);
            return 0;
        }
        if (typeOfBracket == 1) {
            openedCharacters[opened] = c;
            opened++;        
        } else if (typeOfBracket == -1) {
            if (variant(c) != openedCharacters[opened - 1]) {
                // printf("First illegal character in %s : %c at pos %d.\n", line, c, iter);
                return c;
            } else {
                openedCharacters[opened - 1] = '\0';
                opened--;
            }
        }
        iter++;
    }
    return 0;
}

int scorePart2(char c){
    switch (c) {
        case ')' : return 1;
        case ']' : return 2;
        case '}' : return 3;
        case '>' : return 4;
        default : return 0;
    }
}

unsigned long int scoreOfCompletingCharacters(char *characters, int size) {
    unsigned long int result = 0;
    for (int i = size - 1; i >= 0; i--) {
        result = 5ULL * result ;
        result = result + (unsigned long int) scorePart2(variant(characters[i]));
    }
   // printf("\n");
    return result;
}

unsigned long int completedLineScore(char *line) {
    // We suppose the line doesn't have any bad characters.
    int size = strlen(line);
    char openedCharacters[size];
    int opened = 0;
    int iter = 0;
    while (line[iter] != '\n') {
        char c = line[iter];
        int typeOfBracket = isOpenOrClose(c);
        if (!typeOfBracket) { // Not necessary since it will never happen but doesn't care.
            printf("Found illegal caracter in %s at pos %d ! \n", line, iter);
            return 0;
        }
        if (typeOfBracket == 1) {
            openedCharacters[opened] = c;
            opened++;        
        } else if (typeOfBracket == -1) {
            openedCharacters[opened - 1] = '\0';
            opened--;
        }
        iter++;
    }
    return scoreOfCompletingCharacters(openedCharacters, opened);
}

int compare( const void* a, const void* b)
{
     unsigned long int int_a = * ( (unsigned long int*) a );
     unsigned long int int_b = * ( (unsigned long int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

int main()
{

    clock_t begin_time = clock();

    FILE *f = fopen("day10.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    char buffer[MAX_LEN];


    int fileSize = 0; // Don't really care, just to make a big array.
    while (!feof(f)) {
        fgets(buffer, MAX_LEN, f);
        fileSize++;
    }

    int somme = 0;
    unsigned long int resultsArray[fileSize];
    int nbOfIncompleteLines = 0;

    fsetpos(f, &start);
    fgets(buffer, MAX_LEN, f);
    do {
        char c = firstIllegalClosing(buffer);
        if (c) {
            somme += score(c);
        } else {
            resultsArray[nbOfIncompleteLines] = completedLineScore(buffer);
            nbOfIncompleteLines++;
        }
        fgets(buffer, MAX_LEN, f);
    } while (!feof(f));
    printf("-- Day 10 --\nSyntax error score : %d\n", somme);

    qsort(resultsArray, nbOfIncompleteLines, sizeof(unsigned long int), compare);
    int middle = (nbOfIncompleteLines/2) + 1;
    printf("Middle score of incomplete lines : %lu, middle : %d de %d\n", resultsArray[middle - 1], middle, nbOfIncompleteLines);
    /* for (int i = 0; i < nbOfIncompleteLines; i++) {
        printf("%lu\n", resultsArray[i]);
    } */

    fclose(f);
    clock_t end_time = clock();
    double time_spend = (double)(end_time - begin_time) / CLOCKS_PER_SEC;

    printf("It took %.5f seconds (cpu time) to compute.\n\n\n", time_spend);

    return 0;
}