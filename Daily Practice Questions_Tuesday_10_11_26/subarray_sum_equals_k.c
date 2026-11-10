
#include <stdio.h>
#include <stdlib.h>

#define TABLESIZE 200003

typedef struct {
    long long key;
    int frequency;
    int occupied;
} HashEntry;

int hashFunction(long long key) {
    long long position = key % TABLESIZE;

    if (position < 0) {
        position += TABLESIZE;
    }

    return (int)position;
}

void insertHash(HashEntry *table, long long key) {
    int position = hashFunction(key);

    while (table[position].occupied && table[position].key != key) {
        position = (position + 1) % TABLESIZE;
    }

    if (!table[position].occupied) {
        table[position].key = key;
        table[position].frequency = 0;
        table[position].occupied = 1;
    }

    table[position].frequency++;
}

int getFrequency(HashEntry *table, long long key) {
    int position = hashFunction(key);

    while (table[position].occupied) {
        if (table[position].key == key) {
            return table[position].frequency;
        }

        position = (position + 1) % TABLESIZE;
    }

    return 0;
}

void readArray(int *numbers, int count) {
    printf("Enter array elements:\n");

    for (int index = 0; index < count; index++) {
        scanf("%d", &numbers[index]);
    }
}

long long countSubarrays(int *numbers, int count, int target) {
    HashEntry *table = calloc(TABLESIZE, sizeof(HashEntry));

    if (table == NULL) {
        return -1;
    }

    long long prefix = 0;
    long long total = 0;

    insertHash(table, 0);

    for (int index = 0; index < count; index++) {
        prefix += numbers[index];

        total += getFrequency(table, prefix - target);

        insertHash(table, prefix);
    }

    free(table);

    return total;
}

int main() {
    int count, target;

    printf("Enter array size: ");

    if (scanf("%d", &count) != 1 || count < 0) {
        printf("Invalid array size\n");
        return 1;
    }

    int *numbers = malloc((count > 0 ? count : 1) * sizeof(int));

    if (numbers == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readArray(numbers, count);

    printf("Enter target sum: ");

    if (scanf("%d", &target) != 1) {
        printf("Invalid target\n");
        free(numbers);
        return 1;
    }

    long long result = countSubarrays(numbers, count, target);

    if (result == -1) {
        printf("Memory allocation failed\n");
    } else {
        printf("Number of subarrays: %lld\n", result);
    }

    free(numbers);

    return 0;
}
