#include "runner.h"
#include "intcode.h"

typedef struct _pos_fifo_node
{
    Pos pos;
    struct _pos_fifo_node *next;
} FifoNode;

typedef struct
{
    FifoNode *head;
    FifoNode *tail;
    int size;
} FifoStruct;

void insert_fifo(FifoStruct *fifo, Pos pos)
{
    FifoNode *new = (FifoNode *)malloc(sizeof(FifoNode));
    new->pos = pos;
    if (fifo->size == 0 || !fifo->head || !fifo->tail)
    {
        fifo->head = new;
        fifo->tail = new;
        fifo->size = 1;
        new->next = NULL;
    }
    else
    {
        fifo->tail->next = new;
        fifo->tail = new;
        fifo->size = fifo->size + 1;
    }
}

Pos remove_fifo(FifoStruct *fifo)
{
    if (!fifo->head)
    {
        printf("Nothing in Fifo.\n");
        exit(0);
        return (Pos){0, 0};
    }
    Pos pos = fifo->head->pos;
    if (fifo->size == 1)
    {
        free(fifo->head);
        fifo->head = NULL;
        fifo->tail = NULL;
        fifo->size = 0;
    }
    else
    {
        FifoNode *temp = fifo->head->next;
        free(fifo->head);
        fifo->head = temp;
        fifo->size = fifo->size - 1;
    }

    return pos;
}

void free_fifo(FifoStruct *fifo)
{
    FifoNode *cur = fifo->head;
    while (fifo->size > 0)
    {
        FifoNode *temp = cur->next;
        free(cur);
        fifo->size = fifo->size - 1;
        cur = temp;
    }
    fifo->tail = NULL;
}

luint part1(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    HashTable points = {NULL};
    Pos current = (Pos){0, 0};
    insert_hash(points, &current, NULL, 1, 0); // inserting an empty space at starting pos
    int direction = 3;
    int loop = 0;
    bool start = true;
    while (start || (current.x != 0 || current.y != 0 || direction != 3))
    {
        start = false;
        // lets try to figure out the entire labyrinth
        give_input(comp, direction);
        run_intcode(comp, 1);
        if (!comp->has_output) // error
        {
            printf("NO OUTPUT\n");
            break;
        }
        Pos wall_pos = current;
        switch (comp->output)
        {
        case 0: // hit a wall
            switch (direction)
            {
            case 1:
                wall_pos.y--;
                break;
            case 2:
                wall_pos.y++;
                break;
            case 3:
                wall_pos.x--;
                break;
            case 4:
                wall_pos.x++;
                break;
            }
            insert_hash(points, &wall_pos, NULL, 0, 0);
            // wall -> turn left
            switch (direction)
            {
            case 1:
                direction = 3;
                break;
            case 2:
                direction = 4;
                break;
            case 3:
                direction = 2;
                break;
            case 4:
                direction = 1;
                break;
            }
            break;
        case 1: // took one step
        case 2: // step + found end
            switch (direction)
            {
            case 1:
                current.y--;
                break;
            case 2:
                current.y++;
                break;
            case 3:
                current.x--;
                break;
            case 4:
                current.x++;
                break;
            }
            insert_hash(points, &current, NULL, comp->output, 0);
            // no wall -> turn right
            switch (direction)
            {
            case 1:
                direction = 4;
                break;
            case 2:
                direction = 3;
                break;
            case 3:
                direction = 1;
                break;
            case 4:
                direction = 2;
                break;
            }
            break;
        }
        // printf("Pos : %lld %lld, direction %d\n", current.x, current.y, direction);
        loop++;
    }

    // here, the labyrinth is entirely mapped. Just need to BFS now !
    FifoStruct fifo = {NULL, NULL, 0};
    insert_fifo(&fifo, (Pos){0, 0});
    Pos position = remove_fifo(&fifo);
    insert_hash(points, &position, NULL, 1, 1);       // starting the BFS at 1 for first step
    int steps = get_flag(points, &position, NULL, 1); // the value of the BFS on current step
    while (get_flag(points, &position, NULL, 0) != 2)
    {
        // just do BFS : add a flag to the current position that is steps,
        // then for all 4 directions if the flag of the next block is still 0, then add it to fifo
        // if it is lower than steps then don't
        // will end up finding the end        insert_hash()
        for (int i = 1; i < 5; i++)
        {
            Pos next = position;
            switch (i)
            {
            case 1:
                next.y--;
                break;
            case 2:
                next.y++;
                break;
            case 3:
                next.x--;
                break;
            case 4:
                next.x++;
                break;
            }
            if (get_flag(points, &next, NULL, 0)) // if can move there
            {
                if (get_flag(points, &next, NULL, 1) == 0)
                { // if not seen yet
                    insert_hash(points, &next, NULL, steps + 1, 1);
                    insert_fifo(&fifo, next);
                }
            }
        }

        // move to next
        position = remove_fifo(&fifo);
        steps = get_flag(points, &position, NULL, 1);
    }

    // WARNING : started at 1, so may need -1 !!

    lint res = get_flag(points, &position, NULL, 1) - 1;

    free_fifo(&fifo);
    free_computer(comp);
    free_hash(points);
    return res;
}

// -----------------------------------------------------------------

luint part2(void *input_v, void **args)
{
    lint *input = (lint *)input_v;
    int size = ((int *)args)[0];

    IntcodeComputer *comp = init_computer(input, size);

    HashTable points = {NULL};
    Pos current = (Pos){0, 0};
    insert_hash(points, &current, NULL, 1, 0); // inserting an empty space at starting pos
    int direction = 3;
    int loop = 0;
    bool start = true;
    Pos oxygen_pos = (Pos){0, 0};
    while (start || (current.x != 0 || current.y != 0 || direction != 3))
    {
        start = false;
        // lets try to figure out the entire labyrinth
        give_input(comp, direction);
        run_intcode(comp, 1);
        if (!comp->has_output) // error
        {
            printf("NO OUTPUT\n");
            break;
        }
        Pos wall_pos = current;
        switch (comp->output)
        {
        case 0: // hit a wall
            switch (direction)
            {
            case 1:
                wall_pos.y--;
                break;
            case 2:
                wall_pos.y++;
                break;
            case 3:
                wall_pos.x--;
                break;
            case 4:
                wall_pos.x++;
                break;
            }
            insert_hash(points, &wall_pos, NULL, 0, 0);
            // wall -> turn left
            switch (direction)
            {
            case 1:
                direction = 3;
                break;
            case 2:
                direction = 4;
                break;
            case 3:
                direction = 2;
                break;
            case 4:
                direction = 1;
                break;
            }
            break;
        case 1: // took one step
        case 2: // step + found end
            switch (direction)
            {
            case 1:
                current.y--;
                break;
            case 2:
                current.y++;
                break;
            case 3:
                current.x--;
                break;
            case 4:
                current.x++;
                break;
            }
            insert_hash(points, &current, NULL, comp->output, 0);
            // no wall -> turn right
            switch (direction)
            {
            case 1:
                direction = 4;
                break;
            case 2:
                direction = 3;
                break;
            case 3:
                direction = 1;
                break;
            case 4:
                direction = 2;
                break;
            }
            if (comp->output == 2)
            {
                oxygen_pos.x = current.x;
                oxygen_pos.y = current.y;
            }
            break;
        }
        // printf("Pos : %lld %lld, direction %d\n", current.x, current.y, direction);
        loop++;
    }

    // here, the labyrinth is entirely mapped. Just need to BFS now !
    FifoStruct fifo = {NULL, NULL, 0};
    insert_fifo(&fifo, oxygen_pos);
    Pos position = remove_fifo(&fifo);
    insert_hash(points, &position, NULL, 1, 1);       // starting the BFS at 1 for first step
    int steps = get_flag(points, &position, NULL, 1); // the value of the BFS on current step

    while (1)
    {
        insert_hash(points, &position, NULL, 1, 2);
        for (int i = 1; i < 5; i++)
        {
            Pos next = position;
            switch (i)
            {
            case 1:
                next.y--;
                break;
            case 2:
                next.y++;
                break;
            case 3:
                next.x--;
                break;
            case 4:
                next.x++;
                break;
            }
            if (get_flag(points, &next, NULL, 0)) // if can move there
            {
                if (get_flag(points, &next, NULL, 1) == 0)
                { // if not seen yet
                    insert_hash(points, &next, NULL, steps + 1, 1);
                    insert_fifo(&fifo, next);
                }
            }
        }

        if (fifo.size != 0)
        {
            // move to next
            position = remove_fifo(&fifo);
            steps = get_flag(points, &position, NULL, 1);
        }
        else
            break; // fifo empty means seen all
    }

    // WARNING : started at 1, so need -1 !!
    lint res = steps - 1;

    free_fifo(&fifo);
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
    run(15, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}