
#include <stdio.h>
#include <stdlib.h>

void readArray(int *numbers, int count) {
    printf("Enter array elements:\n");

    for (int index = 0; index < count; index++) {
        scanf("%d", &numbers[index]);
    }
}

int searchTarget(int *numbers, int count, int target) {
    int left = 0;
    int right = count - 1;

    while (left <= right) {
        int middle = left + (right - left) / 2;

        if (numbers[middle] == target) {
            return middle;
        }

        if (numbers[left] <= numbers[middle]) {
            if (target >= numbers[left] && target < numbers[middle]) {
                right = middle - 1;
            } else {
                left = middle + 1;
            }
        } else {
            if (target > numbers[middle] && target <= numbers[right]) {
                left = middle + 1;
            } else {
                right = middle - 1;
            }
        }
    }

    return -1;
}

int main() {
    int count, target;

    printf("Enter array size: ");
    if (scanf("%d", &count) != 1 || count <= 0) {
        printf("Invalid array size\n");
        return 1;
    }

    int *numbers = malloc(count * sizeof(int));

    if (numbers == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readArray(numbers, count);

    printf("Enter target: ");
    if (scanf("%d", &target) != 1) {
        printf("Invalid target\n");
        free(numbers);
        return 1;
    }

    int result = searchTarget(numbers, count, target);

    printf("Target index: %d\n", result);

    free(numbers);

    return 0;
}
