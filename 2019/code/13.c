#include "runner.h"
#include "intcode.h"

#include <termios.h>

luint part1(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    HashTable tiles = {NULL};
    Pos current = (Pos){0, 0};
    int tile_id = 0;
    while (!comp->finished)
    {
        run_intcode(comp, 1);
        current.x = comp->output;
        run_intcode(comp, 1);
        current.y = comp->output;
        run_intcode(comp, 1);
        tile_id = comp->output;
        insert_pos(tiles, current, tile_id);
    }

    luint res = (luint)count_flags(tiles, 2);
    free_computer(comp);
    free_hash(tiles);
    return res;
}

// -----------------------------------------------------------------

#define Y 25
#define X 45

typedef struct
{
    int map[Y][X];
    int score;
} Screen;

void show(Screen screen, bool slow, int paddle, int ball)
{
    printf("\033[1;1H\033[2J"); // clear screen

    printf("Score : %d\n", screen.score);
    for (int y = 0; y < Y; y++)
    {
        for (int x = 0; x < X; x++)
        {
            switch (screen.map[y][x])
            {
            case 1:
                printf("#");
                break;
            case 2:
                printf("&");
                break;
            case 3:
                printf("^");
                break;
            case 4:
                printf("o");
                break;
            default:
                printf(".");
            }
        }
        printf("\n");
    }
    printf("p : %d ; b : %d ; slow : %d\n", paddle, ball, slow);

    // remove commentary below, put there because intellicode not happy
    // just to slow it down to a watchable speed
    /* struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    if (slow)
        sleepTime.tv_nsec = 1000 * 1000 * 5;
    else
        sleepTime.tv_nsec = 0;
    nanosleep(&sleepTime, NULL); */
}

luint part2(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);
    comp->memory[0] = 2;

    Screen screen;
    for (int y = 0; y < Y; y++)
        for (int x = 0; x < X; x++)
            screen.map[y][x] = 0;
    screen.score = 0;

    // we always put the paddle below the ball !
    int paddle_x = -1;
    int ball_x = -1;

    Pos current = (Pos){0, 0};
    int tile_id = 0;
    while (!comp->finished)
    {
        // show(screen, ball_x != -1, paddle_x, ball_x);
        if (comp->want_input)
        {
            if (paddle_x < ball_x)
                give_input(comp, 1);
            else if (paddle_x > ball_x)
                give_input(comp, -1);
            else
                give_input(comp, 0);
        }
        // if there is an output, there should be 3, but if there isn't that means it is waiting for input
        run_intcode(comp, 1);
        if (comp->has_output)
        {
            current.x = comp->output;
            run_intcode(comp, 1);
            if (comp->has_output)
                current.y = comp->output;
            run_intcode(comp, 1);
            if (comp->has_output)
                tile_id = comp->output;
            if (current.x == -1 && current.y == 0) // update score
                screen.score = tile_id;
            else
            {
                if (tile_id == 3) // move paddle
                    paddle_x = current.x;
                else if (tile_id == 4) // move ball
                    ball_x = current.x;
                if (0 <= current.y && 0 <= current.x && current.y < Y && current.x < X)
                    screen.map[current.y][current.x] = tile_id;
            }
        }
    }

    free_computer(comp);
    return screen.score;
}

// ----------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    lint *input = read_intcode(argv[1], &size);
    run(13, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}