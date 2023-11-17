#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define MAX_LEN 128
#define BINGO_GRID 5

int *read_inputs(FILE *f, fpos_t *start, int *size, fpos_t *end)
{

    *size = 0;
    char c = '0';
    while (c != '\n')
    {
        c = fgetc(f);
        if (c == ',')
            (*size)++;
    }
    (*size)++; // Pour le dernier chiffre
    int *result = malloc(sizeof(int) * (*size));
    assert(result);
    for (int i = 0; i < *size; i++)
        result[i] = 0;

    fsetpos(f, start);
    c = '0';
    int i = 0;
    while (c != '\n')
    {
        c = fgetc(f);
        if ((c != ',') && (c != '\n'))
        {
            result[i] = result[i] * 10 + atoi(&c);
        }
        else
        {
            i++;
        }
    }
    fgetpos(f, end);
    return result;
}

int **read_boards(FILE *f, fpos_t *start, int *size)
{
    // Returns a list of all the lines. Not a list of list of list.
    *size = 0;

    char buffer[MAX_LEN];
    while (!feof(f))
    {
        fgets(buffer, MAX_LEN, f);
        if ((strlen(buffer) > 1) && (buffer[0] != '\n'))
            (*size)++;
    }
    if (*size <= 0)
        return NULL;
    int **result = malloc(sizeof(int *) * (*size));
    if (!result)
        return NULL;
    assert(result);
    for (int i = 0; i < (*size); i++)
    {
        result[i] = malloc(sizeof(int) * BINGO_GRID);
        for (int j = 0; j < BINGO_GRID; j++)
        {
            result[i][j] = 0;
        }
    }

    fsetpos(f, start);
    int a, b, c, d, e;
    int i = 0;
    while (!feof(f))
    {
        int test = fscanf(f, "%d %d %d %d %d", &a, &b, &c, &d, &e);
        if (test == 5)
        {
            result[i][0] = a;
            result[i][1] = b;
            result[i][2] = c;
            result[i][3] = d;
            result[i][4] = e;
            i++;
        }
    }

    return result;
}

void free_board(int **boards, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(boards[i]);
    }
    free(boards);
}

// bool
int check_line(int **boards, int line, int *inputs, int to_check)
{
    for (int i = 0; i < BINGO_GRID; i++)
    {
        int found = 0;
        for (int input = 0; input <= to_check; input++)
        {
            if (inputs[input] == boards[line][i])
            {
                found = 1;
            }
        }
        if (!found)
            return 0;
    }
    return 1;
}

int check_column(int **boards, int line_start, int column, int *inputs, int to_check)
{
    for (int i = line_start; i < BINGO_GRID + line_start; i++)
    {
        int found = 0;
        for (int input = 0; input <= to_check; input++)
        {
            if (inputs[input] == boards[i][column])
            {
                found = 1;
            }
        }
        if (!found)
            return 0;
    }
    return 1;
}

int score(int **boards, int winner_board, int *inputs, int to_check)
{
    int score = 0;
    for (int i = winner_board * BINGO_GRID; i < winner_board * BINGO_GRID + BINGO_GRID; i++)
    {
        for (int j = 0; j < BINGO_GRID; j++)
        {
            // printf("%d", boards[i][j]);
            int found = 0;
            for (int input = 0; input < to_check; input++)
            {
                if (inputs[input] == boards[i][j])
                {
                    found = 1;
                    // printf("*");
                }
            }
            // printf(" ");
            if (!found)
                score += boards[i][j];
        }
        // printf("\n");
    }
    return score;
}

int not_won(int *won, int size)
{
    int compte = 0;
    for (int i = 0; i < size; i++)
    {
        if (!won[i])
            compte++;
    }
    return compte;
}

int main()
{

    FILE *f = fopen("inputs/day4.txt", "r");
    fpos_t start;
    fgetpos(f, &start);
    if (!f)
    {
        printf("FNF");
        return 1;
    }

    int size;
    fpos_t end;
    int *inputs = read_inputs(f, &start, &size, &end);
    if (!inputs)
    {
        return 1;
    }
    /* for (int i = 0; i < size; i++){
        printf("%d\n", inputs[i]);
    } */

    int number_of_lines;
    int **boards = read_boards(f, &end, &number_of_lines);

    /* for (int i = 0; i < number_of_lines; i ++){
        for (int j = 0; j < BINGO_GRID; j ++){
            printf("%d ", boards[i][j]);
        }
        printf("\n");
        if (i % BINGO_GRID == BINGO_GRID - 1) printf("\n");
    } */

    int stop = 0;
    int winner_line = 0;
    int winner_number = 0;
    int numbers_drawn = 0;

    int number_of_grids = number_of_lines / BINGO_GRID;
    int won[number_of_grids];
    for (int i = 0; i < number_of_grids; i++)
    {
        won[i] = 0;
    }

    int last_win = -1;
    int last_win_numbers_drawn = -1;

    for (int input = 0; (input < size) && !stop; input++)
    {
        for (int i = 0; (i < number_of_lines) && !stop; i++)
        {
            if (won[i / BINGO_GRID])
                continue;
            if (check_line(boards, i, inputs, input))
            {
                // printf("found winner : line %d at input number %d, %d\n", i, input, inputs[input]);
                if (!numbers_drawn)
                {
                    winner_number = inputs[input];
                    winner_line = i;
                    numbers_drawn = input + 1;
                }
                won[i / BINGO_GRID] = 1;
                last_win = i / BINGO_GRID;
                last_win_numbers_drawn = input + 1;
            }
        }
        for (int i = 0; i < number_of_lines && !stop; i += BINGO_GRID)
        {
            if (won[i / BINGO_GRID])
                continue;
            for (int j = 0; (j < BINGO_GRID) && !stop; j++)
            {
                if (check_column(boards, i, j, inputs, input))
                {
                    // printf("found winner : line %d column %d at input number %d, %d\n", i, j, input, inputs[input]);
                    if (!numbers_drawn)
                    {
                        winner_number = inputs[input];
                        winner_line = i;
                        numbers_drawn = input + 1;
                    }
                    won[i / BINGO_GRID] = 1;
                    last_win = i / BINGO_GRID;
                    last_win_numbers_drawn = input + 1;
                }
            }
        }
    }

    int winner_board = winner_line / BINGO_GRID;
    int score_sum = score(boards, winner_board, inputs, numbers_drawn);
    printf("-- Day 4 --\nWinner board : %d, sum : %d, final score : %d\n", winner_board, score_sum, score_sum * winner_number);
    int score_sum_loser = score(boards, last_win, inputs, last_win_numbers_drawn);
    printf("Loser board : %d, sum : %d, final score : %d\n", last_win, score_sum_loser, score_sum_loser * inputs[last_win_numbers_drawn - 1]);

    free_board(boards, number_of_lines);
    free(inputs);
    fclose(f);

    return 0;
}
