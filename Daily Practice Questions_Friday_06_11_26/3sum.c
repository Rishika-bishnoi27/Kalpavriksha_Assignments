
#include <stdio.h>
#include <stdlib.h>

int compareNumbers(const void *first, const void *second) {
    int left = *(int *)first;
    int right = *(int *)second;

    return (left > right) - (left < right);
}

void readArray(int *numbers, int count) {
    printf("Enter array elements:\n");

    for (int index = 0; index < count; index++) {
        scanf("%d", &numbers[index]);
    }
}

void findTriplets(int *numbers, int count) {
    qsort(numbers, count, sizeof(int), compareNumbers);

    int found = 0;

    for (int first = 0; first < count - 2; first++) {
        if (first > 0 && numbers[first] == numbers[first - 1]) {
            continue;
        }

        int left = first + 1;
        int right = count - 1;

        while (left < right) {
            long long total = (long long)numbers[first] + numbers[left] + numbers[right];

            if (total == 0) {
                printf("[%d, %d, %d]\n",
                       numbers[first], numbers[left], numbers[right]);

                found = 1;
                left++;
                right--;

                while (left < right && numbers[left] == numbers[left - 1]) {
                    left++;
                }

                while (left < right && numbers[right] == numbers[right + 1]) {
                    right--;
                }
            } 
            else if (total < 0) {
                left++;
            } 
            else {
                right--;
            }
        }
    }

    if (!found) {
        printf("No triplets found\n");
    }
}

int main() {
    int count;

    printf("Enter array size: ");
    if (scanf("%d", &count) != 1 || count < 0) {
        printf("Invalid array size\n");
        return 1;
    }

    if (count < 3) {
        printf("No triplets found\n");
        return 0;
    }

    int *numbers = malloc(count * sizeof(int));

    if (numbers == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readArray(numbers, count);
    findTriplets(numbers, count);

    free(numbers);

    return 0;
}
