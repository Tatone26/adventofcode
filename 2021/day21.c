#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>


#define MAX_POS 10 // The max position.
#define MIN_POS 1  // The min position.

#define WIN_SCORE 21 // Win score for part two. 
#define MAX_SCORE (WIN_SCORE - 1 + MAX_POS) // The maximum score anyone can get : 20 + 10. (part2)
#define MIN_SCORE 0

// An... EASY day ??? for the 21 ? I'm really suprised (I'm doing this in november 2023)
// Easy to get from 27 possibles outputs to only 7 (always the sum of three dice), but that was not really a trap.
// I guess you just needed to understand the rules to make a solution that runs in a few seconds max. 
// To get really low, just need to save every already-computed solutions #caching -> just around 186200 situations to compute. Which is instant. 

// Proud of my made-up "hash table", which I have been able to make thanks to the max values of every int
// (score <= 30 and pos <= 10), and I even made it dynamic if you want to change the parameters. (max win_score is 28 before long int)

void readInput(FILE *f, fpos_t *start, int *playerOne, int *playerTwo)
{
    fsetpos(f, start);
    *playerOne = -1;
    *playerTwo = -1;
    while (!feof(f))
    {
        fscanf(f, "Player 1 starting position: %d\n", playerOne);
        fscanf(f, "Player 2 starting position: %d\n", playerTwo);
    }
    if (*playerOne == -1 || *playerTwo == -1)
    {
        printf("ERROR READING INPUT\n");
    }
}

int deterministicDice(int lastRoll)
{
    return (lastRoll + 1 - MIN_POS) % 100 + MIN_POS;
}

int movePlayer(int lastPos, int roll)
{
    return (lastPos + roll - MIN_POS) % MAX_POS + MIN_POS;
}

int action(int startOne, int startTwo)
{
    int posOne = startOne;
    int posTwo = startTwo;
    int scoreOne = 0;
    int scoreTwo = 0;
    int lastRoll = 0;
    int numberOfRolls = 0;
    while (1)
    {
        int roll1 = deterministicDice(lastRoll);
        int roll2 = deterministicDice(roll1);
        int roll3 = deterministicDice(roll2);
        numberOfRolls += 3;
        lastRoll = roll3;
        posOne = movePlayer(posOne, roll1 + roll2 + roll3);
        scoreOne += posOne;
        if (scoreOne >= 1000)
        {
            return scoreTwo * numberOfRolls;
        }

        roll1 = deterministicDice(lastRoll);
        roll2 = deterministicDice(roll1);
        roll3 = deterministicDice(roll2);
        numberOfRolls += 3;
        lastRoll = roll3;
        posTwo = movePlayer(posTwo, roll1 + roll2 + roll3);
        scoreTwo += posTwo;
        if (scoreTwo >= 1000)
        {
            return scoreOne * numberOfRolls;
        }
    }
}

// Useful : 0 <= pos - 1 < 10 so can use (pos1 - 1) + 10 * (pos2 - 1) without any problems -> between 0 and 99
// 0 <= score <= 30 (extreme case) so (score1) + 30 * (score2) no problem
// that gets us to ((pos1 - 1) + 10 * (pos2 - 1)) + 10 * 10 * ((score1) + 30 * (score2)) -> no ambiguities -> between 0 and 93099.
// move all of that to the left once and set the last number as playerToMove, and we get something between 0 and 186198.
// And it works !
int hash(int pos1, int pos2, int score1, int score2, int playerToMove)
{
    return ((((pos1 - MIN_POS) + MAX_POS * (pos2 - MIN_POS)) + MAX_POS * MAX_POS * ((score1 - MIN_SCORE) + MAX_SCORE * (score2 - MIN_SCORE))) << 1) | playerToMove;
}

// Need to cache some stuff : some situations can be reached multiple ways.
long int result(int pos1, int pos2, int score1, int score2, int playerToMove, long int *cache)
{
    if (score1 >= WIN_SCORE)
        return 1;
    if (score2 >= WIN_SCORE)
        return 0;

    int h = hash(pos1, pos2, score1, score2, playerToMove); // The magiiiic
    if (cache[h] != -1)
        return cache[h];

    long int results[7];
    if (playerToMove)
    {
        int newPos = pos2 + 2;
        for (int i = 3 - 3; i <= 9 - 3; i++)
        {
            newPos = newPos % 10 + 1;
            results[i] = result(pos1, newPos, score1, score2 + newPos, 0, cache);
        }
    }
    else
    {
        int newPos = pos1 + 2; // It will move at least two steps, then a 3rd, 4th, 5th, 6th, 7th, 8th or 9th, but nothing else. 
        for (int i = 3 - 3; i <= 9 - 3; i++)
        {
            newPos = newPos % 10 + 1;
            results[i] = result(newPos, pos2, score1 + newPos, score2, 1, cache);
        }
    }
    // Simple probas, make a proba tree -> 27 possibles outputs, and their sums are what I used here. 
    long int THEresult = results[0] + 3 * results[1] + 6 * results[2] + 7 * results[3] + 6 * results[4] + 3 * results[5] + results[6];
    cache[h] = THEresult;
    return THEresult;
}

int main()
{
    FILE *f = fopen("inputs/day21.txt", "r");
    if (!f || f == NULL)
    {
        printf("FNF");
        return 1;
    }
    fpos_t start;
    fgetpos(f, &start);
    int startOne, startTwo;
    readInput(f, &start, &startOne, &startTwo);
    printf("-- Day 21 --\nTo 1000, deterministic : loser score * numbers of rolls : %d\n", action(startOne, startTwo));

    int maxHash = hash(MAX_POS, MAX_POS, MAX_SCORE, MAX_SCORE, 1);
    long int cache[maxHash];
    for (int i = 0; i < maxHash; i++)
    {
        cache[i] = -1;
    }
    long int part2 = result(startOne, startTwo, MIN_SCORE, MIN_SCORE, 0, cache);

    printf("QUANTUM DIE : First player win in %ld universes. (number of alternate situations maximum : %d)\n", part2, maxHash + 1);
    fclose(f);
    return 0;
}