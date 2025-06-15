#include "runner.h"

#define COLOR_OFF "\033[m"
#define BOLD "\033[1m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define UNDERLINE "\033[4m"
#define DIM "\033[2m"

/// @brief Runs a "day", given a function / part.
/// @param dayNum
/// @param part1
/// @param part2
/// @param count
/// @param parameters The parameters to pass to the functions.
void run(int dayNum, luint (*part1)(void *input, void **args), luint (*part2)(void *input, void **args), void *input, void **args)
{
    printf(BOLD GREEN "\n  -- Day %d --  " COLOR_OFF "\n\n", dayNum);

    clock_t startPart1 = clock();           // Timer
    luint res1 = (luint)part1(input, args); // First part
    clock_t endPart1 = clock();

    printf(UNDERLINE "Part 1" COLOR_OFF " : %llu\n", res1);

    clock_t startPart2 = clock();           // Timer
    luint res2 = (luint)part2(input, args); // Second part
    clock_t endPart2 = clock();

    printf(UNDERLINE "Part 2" COLOR_OFF " : %llu\n\n", res2);
    // Timing calculations
    float timePart1 = (float)(endPart1 - startPart1) / CLOCKS_PER_MS;
    float timePart2 = (float)(endPart2 - startPart2) / CLOCKS_PER_MS;
    // Pretty printing time
    const char *colorPart1 = (timePart1 <= 25.0 ? GREEN : RED);
    const char *colorPart2 = (timePart2 <= 50.0 ? GREEN : RED);
    const char *colorTotal = (timePart1 + timePart2 <= 80.0 ? GREEN : RED);
    printf(DIM "Part 1 :%s %.3fms\n" COLOR_OFF, colorPart1, timePart1);
    printf(DIM "Part 2 :%s %.3fms\n" COLOR_OFF, colorPart2, timePart2);
    printf(DIM "Total Time :%s %.3fms\n" COLOR_OFF, colorTotal, timePart1 + timePart2);
}

/// @brief Counts the number of digits of the given number in base 10, by divisions
/// @param n The number
/// @return The number of digits
int nbOfDigits(luint n)
{
    if (n == 0)
        return 1;
    int res = 1;
    while (n /= 10)
        res++;
    return res;
}

/// @brief Counts the number of lines that are not empty (ignores the last one)
/// @param f an open file, set to the desired position
/// @return The number of interesting lines (size of input)
int fileSize(FILE *f)
{
    fpos_t start;
    fgetpos(f, &start);
    char buffer[MAX_LINE_LEN];
    int n = 0;
    while (!feof(f) && fgets(buffer, MAX_LINE_LEN, f))
        n++;
    fsetpos(f, &start);
    return n;
}

/// @brief Uses euclide's algorithm to calculate the pgcd and the two Bezout's coefficient
/// @param a
/// @param b
/// @param u First Bezout coeff (for a) (CAN BE NEGATIVE)
/// @param v Second Bezout coeff (for b) (CAN BE NEGATIVE)
/// @return pgcd(a, b)
luint euclide(luint a, luint b, long long *u, long long *v)
{
    long long r1 = a, u1 = 1, v1 = 0, r2 = b, u2 = 0, v2 = 1;
    while (r2)
    {
        long long q = r1 / r2;
        long long rs = r1;
        long long us = u1;
        long long vs = v1;
        r1 = r2;
        u1 = u2;
        v1 = v2;
        r2 = rs - q * r2;
        u2 = us - q * u2;
        v2 = vs - q * v2;
    }
    *u = u1;
    *v = v1;
    return r1;
}

/// @brief Can be used to update the flag too.
/// @param table
/// @param value
/// @param flag
void insert_pos(HashTable table, Pos value, int flag)
{
    int h = pos_hash(value);
    HashNode *cur = table[h];
    // checking if it is already there...
    while (cur)
    {
        if (cur->value.x == value.x && cur->value.y == value.y)
        {
            cur->flag = flag;
            return;
        }
        cur = cur->next;
    }
    HashNode *new = (HashNode *)malloc(sizeof(HashNode));
    new->value = value;
    new->next = table[h];
    new->flag = flag;
    table[h] = new;
}

void remove_pos(HashTable table, Pos value)
{
    int h = pos_hash(value);
    HashNode *cur = table[h];
    HashNode *prec = NULL;
    while (cur)
    {
        if (cur->value.x == value.x && cur->value.y == value.y)
        {
            if (prec)
                prec->next = cur->next;
            else
                table[h] = cur->next;

            free(cur);
            return;
        }
        prec = cur;
        cur = cur->next;
    }
}

bool is_in_hash(HashTable table, Pos value)
{
    int h = pos_hash(value);
    HashNode *cur = table[h];
    while (cur)
    {
        if (cur->value.x == value.x && cur->value.y == value.y)
            return true;
        cur = cur->next;
    }
    return false;
}

/// @brief Returns false if not found.
/// @param table
/// @param value
/// @return
int get_flag(HashTable table, Pos value)
{
    int h = pos_hash(value);
    HashNode *cur = table[h];
    while (cur)
    {
        if (cur->value.x == value.x && cur->value.y == value.y)
            return cur->flag;
        cur = cur->next;
    }
    return false;
}

int pos_hash(Pos value)
{
    luint combined = (luint)value.x * 2654435761 + (luint)value.y;
    combined = combined ^ (combined >> 32);
    combined = combined * 2654435761;
    combined = combined ^ (combined >> 32);

    // Ensure the result is within the bounds of the hash table
    return combined % HASHTABLE_SIZE;
}

/// @brief Returns the number of element in the hashtable
/// @param table
/// @return
int hash_size(HashTable table)
{
    int total = 0;
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        HashNode *cur = table[i];
        while (cur != NULL)
        {
            total++;
            cur = cur->next;
        }
    }
    return total;
}

int count_flags(HashTable table, int flag)
{
    int total = 0;
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        HashNode *cur = table[i];
        while (cur != NULL)
        {
            if (cur->flag == flag)
                total++;
            cur = cur->next;
        }
    }
    return total;
}

void free_hash(HashTable table)
{
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        HashNode *cur = table[i];
        while (cur != NULL)
        {
            HashNode *next = cur->next;
            free(cur);

            cur = next;
        }
    }
}