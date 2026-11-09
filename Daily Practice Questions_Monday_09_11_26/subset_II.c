#include <stdio.h>
#include <stdlib.h>

int compareNumbers(const void *first, const void *second) {
    int left = *(int *)first;
    int right = *(int *)second;

    return (left > right) - (left < right);
}

void displaySubset(int *subset, int length) {
    printf("[ ");

    for (int index = 0; index < length; index++) {
        printf("%d ", subset[index]);
    }

    printf("]\n");
}

void generateSubsets(int *numbers, int count, int start, int *subset, int length) {
    displaySubset(subset, length);

    for (int index = start; index < count; index++) {
        if (index > start && numbers[index] == numbers[index - 1]) {
            continue;
        }

        subset[length] = numbers[index];

        generateSubsets(numbers, count, index + 1, subset, length + 1);
    }
}

int main() {
    int count;

    printf("Enter array size: ");

    if (scanf("%d", &count) != 1 || count < 0) {
        printf("Invalid array size\n");
        return 1;
    }

    int *numbers = malloc((count > 0 ? count : 1) * sizeof(int));
    int *subset = malloc((count > 0 ? count : 1) * sizeof(int));

    if (numbers == NULL || subset == NULL) {
        printf("Memory allocation failed\n");
        free(numbers);
        free(subset);
        return 1;
    }

    printf("Enter array elements:\n");

    for (int index = 0; index < count; index++) {
        if (scanf("%d", &numbers[index]) != 1) {
            printf("Invalid input\n");
            free(numbers);
            free(subset);
            return 1;
        }
    }

    qsort(numbers, count, sizeof(int), compareNumbers);

    printf("Unique subsets:\n");

    generateSubsets(numbers, count, 0, subset, 0);

    free(numbers);
    free(subset);

    return 0;
}
