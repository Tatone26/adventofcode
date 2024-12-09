#include "runner.h"

typedef struct
{
    int position;
    int size;
    int id;
} File;

/// @brief Modifies in-place the two given arrays by moving the files where they can, starting from the last one.
///
/// Use size = 1 for all files to do first part.
///
/// @returns the checksum
luint defragmentation(File *files, int nb_files, int free_spaces)
{
    luint checksum = 0;
    int free_index = free_spaces; // first free space (indexed in the array)
    // first file to move is the last one
    for (int to_move = nb_files - 1; to_move >= 0; to_move--)
    {
        File *file = &files[to_move];
        // for part 1, this for loop will always stop at the first iteration
        // part 2 is slow because it needs to check every single free space... -> need a filter faster than a single if (size > s) to gain time
        for (int test = free_index; files[test].position < file->position; test++)
        {
            File *free_block = &files[test];
            if (free_block->size >= file->size)
            {
                file->position = free_block->position; // move file

                free_block->position += file->size; // update free space
                free_block->size -= file->size;
                break;
            }
        }
        while (files[free_index].size == 0) // update first free space
            free_index++;
        // id * (i in 0..size : position * i) = id * (position * size + (size * (size - 1))/2)
        checksum += (long)file->id * ((long)(file->position * file->size) + (long)((file->size) * (file->size - 1)) / 2);
    }
    return checksum;
}

luint part1(va_list args)
{
    short *input = va_arg(args, short *);
    int size = va_arg(args, int);
    // since we are breaking every file into small 1 block files, the worst case scenario is if every file is 9 blocks long
    int max_size = size * 9;
    // read the file system (1 block / short) sorted : files first, free spaces after
    File *files = (File *)malloc(max_size * sizeof(File));
    // fill arrays
    int id = 0, position = 0, nb_files = 0, nb_free_spaces = max_size / 2;
    for (int i = 0; i < size; i++)
    {
        if (i % 2 == 0)
        {
            for (int j = position; j < position + input[i]; j++)
                files[nb_files++] = (File){j, 1, id};
            id++;
        }
        else if (input[i] > 0)
            files[nb_free_spaces++] = (File){position, input[i], -1};

        position += input[i];
    }

    luint res = defragmentation(files, nb_files, max_size / 2);

    free(files);
    return res;
}

// -----------------------------------------------------------------

luint part2(va_list args)
{
    short *input = va_arg(args, short *);
    int size = va_arg(args, int);

    // read the file system (1 block / short), sorted : files first and free spaces after
    File *files = (File *)malloc(size * sizeof(File));
    // fill arrays
    int id = 0, position = 0, nb_files = 0, nb_free_spaces = size / 2 + 1;
    for (int i = 0; i < size; i++)
    {
        if (i % 2 == 0)
        {
            files[nb_files++] = (File){position, input[i], id};
            id++;
        }
        else if (input[i] > 0)
            files[nb_free_spaces++] = (File){position, input[i], -1};

        position += input[i];
    }

    luint res = defragmentation(files, nb_files, size / 2 + 1);

    free(files);
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