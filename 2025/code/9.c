#include "runner.h"

#define abs(x) ((x) > 0 ? (x) : -(x))

luint part1(void *input_v, void **args)
{
    Pos *input = (Pos *)input_v;
    int size = ((int *)args)[0];

    luint max = 0;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
        {
            luint dx = (luint)abs(input[i].x - input[j].x);
            luint dy = (luint)abs(input[i].y - input[j].y);
            luint area = (dx + 1) * (dy + 1);
            if (area > max)
                max = area;
        }

    return max;
}

// -----------------------------------------------------------------

// Sad, because I had almost the right idea but was thinking inside out. IA put me back on track with a single answer...

typedef struct
{
    int min_seg_x;
    int max_seg_x;
    int min_seg_y;
    int max_seg_y;
} Segment;

luint part2(void *input_v, void **args)
{
    Pos *input = (Pos *)input_v;
    int size = ((int *)args)[0];

    // small precomputing
    Segment *segments = (Segment *)malloc(sizeof(Segment) * size);
    for (int k = 0; k < size; k++)
    {
        int l = (k + 1) % size;
        int seg_min_x = (input[k].x < input[l].x) ? input[k].x : input[l].x;
        int seg_max_x = (input[k].x > input[l].x) ? input[k].x : input[l].x;
        int seg_min_y = (input[k].y < input[l].y) ? input[k].y : input[l].y;
        int seg_max_y = (input[k].y > input[l].y) ? input[k].y : input[l].y;
        segments[k] = (Segment){seg_min_x, seg_max_x, seg_min_y, seg_max_y};
    }

    luint max = 0;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++) // actually skipping the last pair lol
        {
            luint dx = (luint)abs(input[i].x - input[j].x);
            luint dy = (luint)abs(input[i].y - input[j].y);
            luint area = (dx + 1) * (dy + 1);
            if (area > max)
            {
                Pos down_left = (Pos){input[i].x < input[j].x ? input[i].x : input[j].x, input[i].y < input[j].y ? input[i].y : input[j].y};
                Pos up_right = (Pos){input[i].x > input[j].x ? input[i].x : input[j].x, input[i].y > input[j].y ? input[i].y : input[j].y};

                // we check, for every edge of the polygon, if it intercepts any edge of our rectangle (PERPENDICULAR CROSSING ONLY)
                int is_valid = 1;
                for (int k = 0; k < size; k++)
                {
                    // will cut if horizontal segment
                    if (segments[k].min_seg_y > down_left.y && segments[k].max_seg_y < up_right.y) // is at the right height to cut our rectangle
                    {
                        if (segments[k].min_seg_x < up_right.x && segments[k].max_seg_x > down_left.x) // is cutting or at least a bit inside the rectangle
                        {
                            // which means the rectangle isn't entirely contained
                            is_valid = 0;
                            break;
                        }
                    }

                    // same logic for vertical segments
                    if (segments[k].min_seg_x > down_left.x && segments[k].max_seg_x < up_right.x)
                    {
                        if (segments[k].min_seg_y < up_right.y && segments[k].max_seg_y > down_left.y)
                        {
                            is_valid = 0;
                            break;
                        }
                    }
                }

                if (is_valid)
                    max = area;
            }
        }

    free(segments);
    return max;
}

// ----------------------------------------------------------------

Pos *readInput(char *filename, int *size)
{
    char buffer[MAX_LINE_LEN];

    FILE *f = fopen(filename, "r");
    if (!f)
        exit(2);
    *size = fileSize(f);
    Pos *input = (Pos *)malloc(sizeof(Pos) * (*size)); // WARNING : change size of alloc

    for (int i = 0; i < *size; i++)
    {
        fgets(buffer, MAX_LINE_LEN, f);
        sscanf(buffer, "%lld,%lld\n", &input[i].x, &input[i].y);
    }

    fclose(f);
    return input;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int size = 0;
    Pos *input = readInput(argv[1], &size);
    run(9, part1, part2, input, (void **)&size);
    free(input);
    return 0;
}