#include <stdio.h>
#include <stdlib.h>

void readArray(int *numbers, int count) {
    for (int index = 0; index < count; index++) {
        scanf("%d", &numbers[index]);
    }
}

double findMedian(int *firstArray, int firstSize, int *secondArray, int secondSize) {
    if (firstSize > secondSize) {
        return findMedian(secondArray, secondSize, firstArray, firstSize);
    }

    int left = 0;
    int right = firstSize;

    while (left <= right) {
        int partitionFirst = left + (right - left) / 2;
        int partitionSecond = (firstSize + secondSize + 1) / 2 - partitionFirst;

        int firstLeft = (partitionFirst == 0) ? -2147483647 : firstArray[partitionFirst - 1];
        int firstRight = (partitionFirst == firstSize) ? 2147483647 : firstArray[partitionFirst];

        int secondLeft = (partitionSecond == 0) ? -2147483647 : secondArray[partitionSecond - 1];
        int secondRight = (partitionSecond == secondSize) ? 2147483647 : secondArray[partitionSecond];

        if (firstLeft <= secondRight && secondLeft <= firstRight) {
            if ((firstSize + secondSize) % 2 == 0) {
                int maximumLeft = firstLeft > secondLeft ? firstLeft : secondLeft;
                int minimumRight = firstRight < secondRight ? firstRight : secondRight;

                return ((double)maximumLeft + minimumRight) / 2.0;
            }

            return firstLeft > secondLeft ? firstLeft : secondLeft;
        }

        if (firstLeft > secondRight) {
            right = partitionFirst - 1;
        } else {
            left = partitionFirst + 1;
        }
    }

    return 0.0;
}

int main() {
    int firstSize, secondSize;

    printf("Enter size of first array: ");

    if (scanf("%d", &firstSize) != 1 || firstSize < 0) {
        printf("Invalid array size\n");
        return 1;
    }

    int *firstArray = malloc((firstSize > 0 ? firstSize : 1) * sizeof(int));

    if (firstArray == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    printf("Enter first sorted array:\n");
    readArray(firstArray, firstSize);

    printf("Enter size of second array: ");

    if (scanf("%d", &secondSize) != 1 || secondSize < 0) {
        printf("Invalid array size\n");
        free(firstArray);
        return 1;
    }

    if (firstSize + secondSize == 0) {
        printf("Both arrays cannot be empty\n");
        free(firstArray);
        return 1;
    }

    int *secondArray = malloc((secondSize > 0 ? secondSize : 1) * sizeof(int));

    if (secondArray == NULL) {
        printf("Memory allocation failed\n");
        free(firstArray);
        return 1;
    }

    printf("Enter second sorted array:\n");
    readArray(secondArray, secondSize);

    double median = findMedian(firstArray, firstSize, secondArray, secondSize);

    printf("Median: %.2f\n", median);

    free(firstArray);
    free(secondArray);

    return 0;
}
