#include "runner.h"
#include "intcode.h"

luint part1(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    // Using a hash map here is probably totally overkill, but hey it works
    HashTable points = {NULL};

    Pos current = (Pos){0, 0};
    while (!comp->finished)
    {
        run_intcode(comp, 1);
        char out = (char)comp->output;
        // printf("%c", out);
        switch (out)
        {
        case '\n':
            current.y++;
            current.x = 0;
            break;
        case '#':
        case '^':
        case 'v':
        case '<':
        case '>':
            insert_hash(points, &current, NULL, 1, 0);
            current.x++;
            break;
        case '.':
            current.x++;
            break;
        default:
            printf("Output not recognized : '%c'\n", out);
            break;
        }
    }

    luint res = 0;

    for (int _ = 0; _ < HASHTABLE_SIZE; _++)
    {
        HashNode *cur = points[_];
        while (cur)
        {
            if (get_flag(points, &(Pos){cur->value.x + 1, cur->value.y}, NULL, 0) && get_flag(points, &(Pos){cur->value.x - 1, cur->value.y}, NULL, 0) && get_flag(points, &(Pos){cur->value.x, cur->value.y + 1}, NULL, 0) && get_flag(points, &(Pos){cur->value.x, cur->value.y - 1}, NULL, 0))
                res += cur->value.x * cur->value.y;
            cur = cur->next;
        }
    }

    free_hash(points);
    free_computer(comp);

    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    HashTable points = {NULL};

    // Getting the map
    Pos current = (Pos){0, 0};
    Pos starting_pos = current;
    int starting_dir = 0;
    while (!comp->finished)
    {
        run_intcode(comp, 1);
        char out = (char)comp->output;
        // printf("%c", out);
        switch (out)
        {
        case '\n':
            current.y++;
            current.x = 0;
            break;
        case '#':
            insert_hash(points, &current, NULL, 1, 0);
            current.x++;
            break;
        case '^':
            insert_hash(points, &current, NULL, 1, 0);
            starting_pos = current;
            starting_dir = 0;
            current.x++;
            break;
        case 'v':
            insert_hash(points, &current, NULL, 1, 0);
            starting_pos = current;
            starting_dir = 1;
            current.x++;

            break;
        case '<':
            insert_hash(points, &current, NULL, 1, 0);
            starting_pos = current;
            starting_dir = 2;
            current.x++;

            break;
        case '>':
            insert_hash(points, &current, NULL, 1, 0);
            starting_pos = current;
            starting_dir = 2;
            current.x++;
            break;
        case '.':
            current.x++;
            break;
        default:
            printf("Output not recognized : '%c'\n", out);
            break;
        }
    }

    char path[MAX_LINE_LEN] = {'\0'};
    int path_offset = 0;
    Pos pos = starting_pos;
    int dir = starting_dir; // 0 : ^ - 1 : v - 2 : < - 3 : >
    int forward = 0;

    while (1)
    {
        Pos next = pos;
        switch (dir)
        {
        case 0:
            next.y--;
            break;
        case 1:
            next.y++;
            break;
        case 2:
            next.x--;
            break;
        case 3:
            next.x++;
            break;
        }
        if (get_flag(points, &next, NULL, 0))
        {
            // can move forward
            forward++;
            pos = next;
        }
        else
        {
            // can not move forward

            if (forward > 0)
            {
                // writing the forward movement
                char number[12] = {'\0'};
                sprintf(number, "%d", forward);
                strncpy(path + path_offset, number, strlen(number));
                path_offset += strlen(number);
                path[path_offset++] = ',';
                forward = 0;
            }

            Pos test = pos;
            int new_dir = dir;
            // trying to turn left
            switch (dir)
            {
            case 0: // was up, testing left
                test.x--;
                new_dir = 2;
                break;
            case 1: // was down, testing right
                test.x++;
                new_dir = 3;
                break;
            case 2: // was left, testing down
                test.y++;
                new_dir = 1;
                break;
            case 3: // was right, testing up
                test.y--;
                new_dir = 0;
                break;
            }
            if (get_flag(points, &test, NULL, 0))
            {
                // can turn left
                path[path_offset++] = 'L';
                path[path_offset++] = ',';
                dir = new_dir;
            }
            else
            {
                // trying to turn right
                test = pos;
                switch (dir)
                {
                case 0: // was up, testing right
                    test.x++;
                    new_dir = 3;
                    break;
                case 1: // was down, testing left
                    test.x--;
                    new_dir = 2;
                    break;
                case 2: // was left, testing up
                    test.y--;
                    new_dir = 0;
                    break;
                case 3: // was right, testing down
                    test.y++;
                    new_dir = 1;
                    break;
                }
                if (get_flag(points, &test, NULL, 0))
                {
                    // can move right
                    path[path_offset++] = 'R';
                    path[path_offset++] = ',';

                    dir = new_dir;
                }
                else
                {
                    // end of the path !
                    break;
                }
            }
        }
    }

    path[path_offset - 1] = '\0';
    // printf("%s\n", path);

    //  path is right, now we need to cut it in small parts...

    char A[21] = {'\0'};
    char B[21] = {'\0'};
    char C[21] = {'\0'};
    char MAIN[21] = {'\0'};
    // manually :
    // TODO : do that automatically ?
    strncpy(A, "L,10,L,10,R,6\n", 15);
    strncpy(B, "R,12,L,12,L,12\n", 16);
    strncpy(C, "L,6,L,10,R,12,R,12\n", 20);
    strncpy(MAIN, "A,A,B,B,C,B,C,B,C,A\n", 21);

    reset_computer(comp, input);
    comp->memory[0] = 2;
    // main
    for (int i = 0; i < 20; i++)
    {
        run_intcode(comp, 0);
        if (comp->want_input)
            give_input(comp, MAIN[i]);
    }
    // A
    for (int i = 0; i < 14; i++)
    {
        run_intcode(comp, 0);
        if (comp->want_input)
            give_input(comp, A[i]);
    }
    // B
    for (int i = 0; i < 15; i++)
    {
        run_intcode(comp, 0);
        if (comp->want_input)
            give_input(comp, B[i]);
    }
    // C
    for (int i = 0; i < 19; i++)
    {
        run_intcode(comp, 0);
        if (comp->want_input)
            give_input(comp, C[i]);
    }
    // video feed
    run_intcode(comp, 0);
    if (comp->want_input)
        give_input(comp, 'n');
    run_intcode(comp, 0);
    if (comp->want_input)
        give_input(comp, '\n');

    // end
    run_intcode(comp, 0);

    luint res = comp->output;
    free_computer(comp);
    free_hash(points);
    return res;
}

// ----------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    lint *input = read_intcode(argv[1], &size);
    run(17, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}