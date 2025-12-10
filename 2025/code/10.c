#include "runner.h"

// hell

typedef struct
{
    uint16_t objective;
    int nb_lights;
    // fast buttons representation using binary
    uint16_t *buttons;
    int nb_buttons;
    int joltage[10];
} Machine;

typedef struct
{
    uint16_t value;
    int nb_steps;
} PathfindingEntry;

// pretty happy with a brute force DFS for this part

luint part1(void *input_v, void **args)
{
    Machine *input = (Machine *)input_v;
    int size = ((int *)args)[0];

    luint tot = 0;

    for (int i = 0; i < size; i++)
    {
        // max around 1024 or 2048 in this input
        int nb_states = (1 << (input[i].nb_lights));
        // keeps the number of states min to get there (there being the binary representation of the lights)
        uint16_t *hash = (uint16_t *)malloc(sizeof(uint16_t) * (nb_states));
        for (int x = 0; x < nb_states; x++)
            hash[x] = UINT16_MAX;

        const int MAX_NB_OF_ENTRIES = 999999; // yes, magic number.
        PathfindingEntry *todos = (PathfindingEntry *)malloc(sizeof(PathfindingEntry) * MAX_NB_OF_ENTRIES);
        int nb_entries = 0;
        todos[nb_entries].nb_steps = 0;
        todos[nb_entries++].value = 0;
        hash[0] = 0;

        // DFS (using LIFO via an array)
        while (nb_entries > 0)
        {
            // take DFS node
            int next_nb_steps = todos[nb_entries - 1].nb_steps + 1;
            uint16_t current_value = todos[nb_entries - 1].value;
            nb_entries--;

            if (next_nb_steps >= hash[input[i].objective]) // stop if we already have better for the objective
                continue;

            for (int x = 0; x < input[i].nb_buttons; x++) // else try all the buttons
            {
                uint16_t next_value = (current_value ^ input[i].buttons[x]);
                assert(next_value <= nb_states);
                if (hash[next_value] > next_nb_steps)
                {
                    hash[next_value] = next_nb_steps;
                    todos[nb_entries].nb_steps = next_nb_steps;
                    todos[nb_entries].value = next_value;
                    nb_entries++;
                }
            }
        }

        tot += hash[input[i].objective];
        free(hash);
        free(todos);
    }
    return tot;
}

// -----------------------------------------------------------------

// I hate cheating like that, but I couldn't do it otherwise... and like they say, don't reinvent the wheel !
#include <glpk.h>

luint part2(void *input_v, void **args)
{
    Machine *input = (Machine *)input_v;
    int nb_machines = ((int *)args)[0];
    luint tot = 0;

    for (int i = 0; i < nb_machines; i++)
    {
        Machine mac = input[i];

        // glpk setup
        int *ia, *ja;
        double *ar;
        int size = mac.nb_buttons * mac.nb_lights;
        int dim = mac.nb_lights;
        glp_prob *lp = glp_create_prob();
        glp_set_prob_name(lp, "combination_solver");
        glp_set_obj_dir(lp, GLP_MIN);

        // adding lines and columns
        glp_add_rows(lp, dim);
        for (int i = 0; i < dim; i++)
        {
            // sum vectors = target[i]
            glp_set_row_bnds(lp, i + 1, GLP_FX, mac.joltage[i], mac.joltage[i]);
        }
        glp_add_cols(lp, mac.nb_buttons);
        for (int j = 0; j < mac.nb_buttons; j++)
        {
            glp_set_col_bnds(lp, j + 1, GLP_LO, 0.0, 0.0);
            glp_set_col_kind(lp, j + 1, GLP_IV);

            glp_set_obj_coef(lp, j + 1, 1.0);
        }

        // filling sparse matrix
        ia = (int *)malloc((size + 1) * sizeof(int));
        ja = (int *)malloc((size + 1) * sizeof(int));
        ar = (double *)malloc((size + 1) * sizeof(double));

        int k = 1; // number of non-null elements
        for (int j = 0; j < mac.nb_buttons; j++)
        { // for every column
            for (int i = 0; i < dim; i++)
            { // for every line (value of button)
                if ((mac.buttons[j] >> i) & 1)
                {
                    ia[k] = i + 1;                               // line number
                    ja[k] = j + 1;                               // column number
                    ar[k] = (double)((mac.buttons[j] >> i) & 1); // value = 0 or 1
                    k++;
                }
            }
        }
        glp_load_matrix(lp, k - 1, ia, ja, ar);

        // SOLVING!!!
        glp_iocp parm;
        glp_init_iocp(&parm);
        parm.presolve = GLP_ON;
        parm.msg_lev = GLP_MSG_ERR;

        // the magic stuff is happening here
        int err = glp_intopt(lp, &parm);

        // getting the result
        if (err == 0)
        {
            int stat = glp_mip_status(lp);
            if (stat == GLP_OPT)
            {
                double z = glp_mip_obj_val(lp);
                tot += z;
            }
        }
        else
            printf("GLK ERROR\n");

        glp_delete_prob(lp);
        free(ia);
        free(ja);
        free(ar);
    }
    glp_free_env();
    return tot;
}

// ----------------------------------------------------------------

Machine *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    *size = fileSize(f);
    Machine *input = (Machine *)malloc(sizeof(Machine) * (*size)); // WARNING : change size of alloc

    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        int split_indexes[100];
        // manual split at spaces
        int nb_splits = 0;
        for (int x = 0; x < (int)strlen(buffer); x++)
        {
            if (buffer[x] == ' ')
                split_indexes[nb_splits++] = x;
        }
        if (nb_splits < 3)
            continue;
        // objective
        input[i].nb_lights = split_indexes[0] - 2;
        input[i].objective = 0;
        for (int x = 0; x < input[i].nb_lights; x++)
        {
            if (buffer[1 + x] == '#')
                input[i].objective |= (1 << x);
        }
        // joltage
        int offset = 0;
        for (int k = 0; k < input[i].nb_lights; k++)
        {
            int temp = atoi(buffer + split_indexes[nb_splits - 1] + 2 + offset);
            input[i].joltage[k] = temp;
            offset += nbOfDigits(temp) + 1;
        }
        // buttons
        input[i].nb_buttons = nb_splits - 1;
        input[i].buttons = (uint16_t *)calloc(input[i].nb_buttons, sizeof(uint16_t));
        for (int k = 0; k < input[i].nb_buttons; k++)
        {
            int offset = split_indexes[k] + 2;
            while (offset < split_indexes[k + 1] - 1)
            {
                int a = atoi(buffer + offset);
                if (a <= input[i].nb_lights)
                    input[i].buttons[k] |= (1 << a);
                offset += nbOfDigits(a) + 1;
            }
        }
    }
    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Machine *input = readInput(argv[1], &size);
    run(10, part1, part2, input, (void **)&size);
    for (int i = 0; i < size; i++)
        free(input[i].buttons);
    free(input);
    return 0;
}