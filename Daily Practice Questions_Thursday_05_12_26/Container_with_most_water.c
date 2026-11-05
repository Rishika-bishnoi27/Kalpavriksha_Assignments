
#include <stdio.h>
#include <stdlib.h>

void readHeights(int *heights, int count) {
    printf("Enter heights:\n");

    for (int index = 0; index < count; index++) {
        scanf("%d", &heights[index]);
    }
}

int calculateMaxWater(int *heights, int count) {
    int left = 0;
    int right = count - 1;
    int maximum = 0;

    while (left < right) {
        int width = right - left;
        int height = heights[left] < heights[right] ? heights[left] : heights[right];
        int water = width * height;

        if (water > maximum) {
            maximum = water;
        }

        if (heights[left] < heights[right]) {
            left++;
        } else {
            right--;
        }
    }

    return maximum;
}

int main() {
    int count;

    printf("Enter number of lines: ");
    scanf("%d", &count);

    if (count < 2) {
        printf("At least two lines are required\n");
        return 1;
    }

    int *heights = malloc(count * sizeof(int));

    if (heights == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readHeights(heights, count);

    int maximum = calculateMaxWater(heights, count);

    printf("Maximum water: %d\n", maximum);

    free(heights);

    return 0;
}
