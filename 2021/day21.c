#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>

// An... EASY day ??? for the 21 ? I'm really suprised (I'm doing this in november 2023)
// Easy to get from 27 possibles outputs to only 7 (always the sum of three dice), but that was not really a trap.
// I guess you just needed to understant the rules. And they are not hard. Or maybe there was a really smart mathematical answer. Check the reddit. 
// Fun to optimize ! (I like that)
// I like the fact that the example output takes almost twice as long as my input before optimisations.
// I guess there is one more recursion cycle. 

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
    if (lastRoll >= 100)
        return 1;
    return lastRoll + 1;
}

int movePlayer(int lastPos, int roll)
{
    return (lastPos + roll - 1) % 10 + 1;
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

// Now freaking optimise this ! I like that. 
long int result(int pos1, int pos2, int score1, int score2, int playerToMove)
{
    if (score1 >= 21)
        return 1;
    if (score2 >= 21)
        return 0;
    if (playerToMove)
    {
        long int one = result(pos1, movePlayer(pos2, 3), score1, score2 + movePlayer(pos2, 3), 0);
        long int two = result(pos1, movePlayer(pos2, 4), score1, score2 + movePlayer(pos2, 4), 0);
        long int three = result(pos1, movePlayer(pos2, 5), score1, score2 + movePlayer(pos2, 5), 0);
        long int four = result(pos1, movePlayer(pos2, 6), score1, score2 + movePlayer(pos2, 6), 0);
        long int five = result(pos1, movePlayer(pos2, 7), score1, score2 + movePlayer(pos2, 7), 0);
        long int six = result(pos1, movePlayer(pos2, 8), score1, score2 + movePlayer(pos2, 8), 0);
        long int seven = result(pos1, movePlayer(pos2, 9), score1, score2 + movePlayer(pos2, 9), 0);
        return one + 3*two + 6*three + 7*four + 6*five + 3 *six + seven;
    } else 
    {
        long int one = result(movePlayer(pos1, 3), pos2, score1 + movePlayer(pos1, 3), score2, 1);
        long int two = result(movePlayer(pos1, 4), pos2, score1 + movePlayer(pos1, 4), score2, 1);
        long int three = result(movePlayer(pos1, 5), pos2, score1 + movePlayer(pos1, 5), score2, 1);
        long int four = result(movePlayer(pos1, 6), pos2, score1 + movePlayer(pos1, 6), score2, 1);
        long int five = result(movePlayer(pos1, 7), pos2, score1 + movePlayer(pos1, 7), score2, 1);
        long int six = result(movePlayer(pos1, 8), pos2, score1 + movePlayer(pos1, 8), score2, 1);
        long int seven = result(movePlayer(pos1, 9), pos2, score1 + movePlayer(pos1, 9), score2, 1);
        return one + 3*two + 6*three + 7*four + 6*five + 3 *six + seven;
    }

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
    printf("-- Day 21 --\nTo 1000 : loser score * numbers of rolls : %d\n", action(startOne, startTwo));

    printf("QUANTUM DIE : First player win in %ld universes.\n", result(startOne, startTwo, 0, 0, 0));
    fclose(f);
    return 0;
}