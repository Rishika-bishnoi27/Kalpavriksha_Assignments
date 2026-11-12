#include <stdio.h>
#include <stdlib.h>

void readArray(int *numbers, int count) {
    printf("Enter array elements:\n");

    for (int index = 0; index < count; index++) {
        if (scanf("%d", &numbers[index]) != 1) {
            printf("Invalid input\n");
            exit(1);
        }
    }
}

void arrangeNumbers(int *numbers, int count) {
    int index = 0;

    while (index < count) {
        int position = numbers[index] - 1;

        if (numbers[index] > 0 &&
            numbers[index] <= count &&
            numbers[index] != numbers[position]) {

            int value = numbers[index];
            numbers[index] = numbers[position];
            numbers[position] = value;

        } else {
            index++;
        }
    }
}

int findMissingPositive(int *numbers, int count) {
    for (int index = 0; index < count; index++) {
        if (numbers[index] != index + 1) {
            return index + 1;
        }
    }

    return count + 1;
}

int main() {
    int count;

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

    arrangeNumbers(numbers, count);

    int result = findMissingPositive(numbers, count);

    printf("First missing positive: %d\n", result);

    free(numbers);

    return 0;
}
