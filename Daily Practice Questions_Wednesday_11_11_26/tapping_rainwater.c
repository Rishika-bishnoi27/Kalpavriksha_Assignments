
#include <stdio.h>
#include <stdlib.h>

void readHeights(int *heights, int count) {
    printf("Enter bar heights:\n");

    for (int index = 0; index < count; index++) {
        scanf("%d", &heights[index]);
    }
}

long long calculateWater(int *heights, int count) {
    int left = 0;
    int right = count - 1;

    int leftMaximum = 0;
    int rightMaximum = 0;

    long long totalWater = 0;

    while (left < right) {
        if (heights[left] < heights[right]) {
            if (heights[left] >= leftMaximum) {
                leftMaximum = heights[left];
            } else {
                totalWater += leftMaximum - heights[left];
            }

            left++;
        } else {
            if (heights[right] >= rightMaximum) {
                rightMaximum = heights[right];
            } else {
                totalWater += rightMaximum - heights[right];
            }

            right--;
        }
    }

    return totalWater;
}

int main() {
    int count;

    printf("Enter number of bars: ");

    if (scanf("%d", &count) != 1 || count < 0) {
        printf("Invalid number of bars\n");
        return 1;
    }

    int *heights = malloc((count > 0 ? count : 1) * sizeof(int));

    if (heights == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readHeights(heights, count);

    long long result = calculateWater(heights, count);

    printf("Trapped rainwater: %lld\n", result);

    free(heights);

    return 0;
}
