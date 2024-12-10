#include "runner.h"

// I was using the same structures for part 1 but it is a lot faster to just read the input once and directly calculate cheksum.

/// @brief Calculate the checksum directly by reading the input, with carefully updated variables to know where we at.
/// @return The checksum
luint part1(va_list args)
{
    short *input = va_arg(args, short *);
    int size = va_arg(args, int);
    if (size == 0)
        return 0;

    luint checksum = 0;
    // calculate positions corresponding to a file in the input
    int *positions = (int *)malloc(sizeof(int) * size);
    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        positions[i] = sum;
        sum += input[i];
    }

    int file_to_move = size - size % 2; // which file we want to move -> the current ID is file_to_move / 2
    int free_space = 1;                 // where is the first free space
    int free_used = 0;                  // how much we used of this space

    // files we can move
    while (file_to_move > free_space && free_space < size)
    {
        for (int i = 0; i < input[file_to_move]; i++)
        {
            checksum += (file_to_move / 2) * (positions[free_space] + free_used);
            free_used++;
            if (free_used >= input[free_space])
            {
                // find next free space
                free_space += 2;
                while (input[free_space] == 0 && free_space < size)
                    free_space += 2;
                // that shouldn't ever arrive but you know
                if (free_space >= size)
                    break;
                // Here is for the edge case of the file that fills the last free space. Not sure about it, but works for example and my input.
                if (file_to_move - free_space < 10 && positions[free_space] >= positions[file_to_move] + i)
                {
                    for (int j = 0; j < input[file_to_move] - i - 1; j++)
                        checksum += (file_to_move / 2) * (positions[file_to_move] + j);
                    break;
                }
                free_used = 0;
            }
        }
        file_to_move -= 2;
    }
    // files we can't move need to be counted too, but that's easier
    while (file_to_move >= 0)
    {
        int size = input[file_to_move];
        checksum += (long)(file_to_move / 2) * ((positions[file_to_move] * size) + (size * (size - 1)) / 2);
        file_to_move -= 2;
    }
    free(positions);
    return checksum;
}

// ------------------------------------------------------------------------------------

// I'm using multiple structures to get a cleaner code, but the concept is storing free spaces in arrays by there size :
// to found the next free space of a given size, just needs to look into this array. Spaces of size x are in all arrays of size <= x.

typedef struct
{
    int position;
    int size;
    int id;
} File;

typedef struct
{
    int position;
    int size;
} Space;

/// @brief free_spaces[i] need to be allocated with 0
/// free_spaces[i] should refer to memory
typedef struct
{
    int indexes[10];
    Space **free_spaces[10];
    Space *memory;
    int nb_spaces;
} FreeSpaces;

/// @brief Updates first free indexes for each size in the given interval (borders included).
/// Looks for the next hole of the right size (in the array) or stops
void updateFreeIndexes(FreeSpaces *spaces, int from, int to)
{
    for (int s = from; s <= to; s++)
    {
        int temp = spaces->indexes[s];
        // if already no more spaces of this size
        if (temp == -1)
            continue;
        for (int f = temp; spaces->free_spaces[s][f]; f++)
            if (spaces->free_spaces[s][f]->size >= s)
            {
                spaces->indexes[s] = f;
                goto found;
            }
        spaces->indexes[s] = -1;
    found:
        continue;
    }
}

/// @brief Finds the first free space that the file can move to
/// @return NULL if there is none
Space *firstFreeSpace(FreeSpaces *spaces, File *file)
{
    int index = spaces->indexes[file->size];
    if (index == -1)
        return NULL;
    return spaces->free_spaces[file->size][index];
}

/// @brief "Moves" the files to the free empty places, and calculate checksum directly.
///
/// @returns the checksum
luint defragmentation(File *files, int nb_files, FreeSpaces spaces)
{
    luint checksum = 0;
    // first file to move is the last one
    for (int to_move = nb_files - 1; to_move >= 0; to_move--)
    {
        File *file = &files[to_move];
        Space *test = firstFreeSpace(&spaces, file);

        if (test && test->position < file->position)
        {
            file->position = test->position; // move file
            test->position += file->size;    // update free space
            test->size -= file->size;
            // need only to update indexes of size > test->size : if test->size == 0, need to look for a new 1.
            updateFreeIndexes(&spaces, test->size + 1, test->size + file->size);
        }
        // id * (i in 0..size : position * i) = id * (position * size + (size * (size - 1))/2)
        checksum += (long)file->id * (long)((file->position * file->size) + ((file->size) * (file->size - 1)) / 2);
    }
    return checksum;
}

// -----------------------------------------------------------------

luint part2(va_list args)
{
    short *input = va_arg(args, short *);
    int size = va_arg(args, int);
    if (size == 0)
        return 0;

    int max_size = size / 2 + 1; // we can't have more files or free spaces than that.
    // read the file system (1 block / short), sorted : files first and free spaces after
    File *files = (File *)calloc(max_size, sizeof(File));
    FreeSpaces spaces = {{-1}, {0}, 0, 0};
    spaces.memory = (Space *)malloc(max_size * sizeof(Space));
    for (int i = 1; i < 10; i++)
        spaces.free_spaces[i] = (Space **)calloc(max_size, sizeof(Space *)); // all free spaces will be in the free_spaces[1]

    // fill arrays (I know this part is like really hard to read sorry)
    int id = 0, position = 0, nb_files = 0;
    int count_free_spaces[10] = {0};
    int already_found = 0;
    for (int i = 0; i < size; i++)
    {
        if (i % 2 == 0)
            // If we found a file, it's easy
            files[nb_files++] = (File){position, input[i], id++};
        else
        {
            // if we found a space bigger than we ever have, then we can update the indexes.
            if (already_found < input[i])
            {
                for (int s = already_found + 1; s <= input[i]; s++)
                    spaces.indexes[s] = 0;
                already_found = input[i];
            }
            // Create the space
            spaces.memory[spaces.nb_spaces] = (Space){position, input[i]};
            // Add it to the corresponding inputs
            for (int s = input[i]; s >= 1; s--)
            {
                spaces.free_spaces[s][count_free_spaces[s]] = &spaces.memory[spaces.nb_spaces];
                count_free_spaces[s]++;
            }
            spaces.nb_spaces++;
        }

        position += input[i];
    }

    luint res = defragmentation(files, nb_files, spaces);

    free(files);
    for (int i = 1; i < 10; i++)
        free(spaces.free_spaces[i]);

    free(spaces.memory);
    return res;
}

// ----------------------------------------------------------------

short *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    fgets(buffer, MAX_LINE_LEN, f);
    *size = strlen(buffer);
    if (buffer[*size - 1] == '\n')
        (*size)--;
    short *input = (short *)malloc(sizeof(short) * *size);
    for (int i = 0; i < *size; i++)
        input[i] = buffer[i] - '0'; // atoi

    fclose(f);
    return input;
}

int main()
{
    int size = 0;
    short *input = readInput("input/9.txt", &size);
    run(9, part1, part2, 2, input, size);

    free(input);
    return 0;
}