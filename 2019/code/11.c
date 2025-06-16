#include "runner.h"
#include "intcode.h"

luint part1(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    HashTable painted_cells = {NULL};

    bool next_color = 0;
    short direction = 0;
    Pos current_pos = (Pos){0, 0};

    while (!comp->finished)
    {
        give_input(comp, get_flag(painted_cells, &current_pos, NULL, 0));
        run_intcode(comp, true);
        next_color = comp->output;
        run_intcode(comp, true);
        int turn = comp->output;
        // paint current cell (will add or update)
        insert_hash(painted_cells, &current_pos, NULL, next_color, 0);
        // turn robot
        direction = (unsigned)(direction + (turn ? 1 : -1)) % 4;
        // move forward
        switch (direction)
        {
        case 0:
            current_pos.y--;
            break;
        case 1:
            current_pos.x++;
            break;
        case 2:
            current_pos.y++;
            break;
        case 3:
            current_pos.x--;
            break;
        }
    }

    luint res = (luint)hash_size(painted_cells);
    free_hash(painted_cells);
    free_computer(comp);
    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    HashTable painted_cells = {NULL};

    bool next_color = 0;
    short direction = 0;
    Pos current_pos = (Pos){0, 0};

    int min_pos_x = __INT32_MAX__;
    int min_pos_y = __INT32_MAX__;
    int max_pos_x = -__INT32_MAX__;
    int max_pos_y = -__INT32_MAX__;

    // inserting initial white cell
    insert_hash(painted_cells, &current_pos, NULL, 1, 0);

    while (!comp->finished)
    {
        give_input(comp, get_flag(painted_cells, &current_pos, NULL, 0));
        run_intcode(comp, true);
        next_color = comp->output;
        run_intcode(comp, true);
        int turn = comp->output;
        // paint current cell (will add or update)
        insert_hash(painted_cells, &current_pos, NULL, next_color, 0);
        // turn robot
        direction = (unsigned)(direction + (turn ? 1 : -1)) % 4;
        // move forward
        switch (direction)
        {
        case 0:
            current_pos.y--;
            if (current_pos.y < min_pos_y)
                min_pos_y = current_pos.y;
            break;
        case 1:
            current_pos.x++;
            if (current_pos.x > max_pos_x)
                max_pos_x = current_pos.x;
            break;
        case 2:
            current_pos.y++;
            if (current_pos.y > max_pos_y)
                max_pos_y = current_pos.y;
            break;
        case 3:
            current_pos.x--;
            if (current_pos.x < min_pos_x)
                min_pos_x = current_pos.x;
            break;
        }
    }
    printf("\nRegistration number : \n");
    for (int y = min_pos_y; y <= max_pos_y; y++)
    {
        for (int x = min_pos_x; x <= max_pos_x; x++)
        {
            if (get_flag(painted_cells, &(Pos){x, y}, NULL, 0))
                printf("#");
            else
                printf(" ");
        }
        printf("\n");
    }

    free_hash(painted_cells);
    free_computer(comp);
    return 0;
}

// ----------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    lint *input = read_intcode(argv[1], &size);
    run(11, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}