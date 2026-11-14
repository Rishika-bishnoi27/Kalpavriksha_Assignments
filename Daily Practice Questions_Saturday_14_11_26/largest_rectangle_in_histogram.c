#include <stdio.h>
#include <stdlib.h>

void readHeights(int *heights, int count) {
    printf("Enter histogram heights:\n");

    for (int index = 0; index < count; index++) {
        if (scanf("%d", &heights[index]) != 1 || heights[index] < 0) {
            printf("Invalid height\n");
            exit(1);
        }
    }
}

long long findLargestRectangle(int *heights, int count) {
    int *stack = malloc((count + 1) * sizeof(int));

    if (stack == NULL) {
        return -1;
    }

    int peak = -1;
    long long maximumArea = 0;

    for (int index = 0; index <= count; index++) {
        int currentHeight = (index == count) ? 0 : heights[index];

        while (peak != -1 && heights[stack[peak]] > currentHeight) {
            int height = heights[stack[peak--]];

            int width = (peak == -1) ? index : index - stack[peak] - 1;

            long long area = (long long)height * width;

            if (area > maximumArea) {
                maximumArea = area;
            }
        }

        stack[++peak] = index;
    }

    free(stack);

    return maximumArea;
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

    long long result = findLargestRectangle(heights, count);

    if (result == -1) {
        printf("Memory allocation failed\n");
    } else {
        printf("Largest rectangle area: %lld\n", result);
    }

    free(heights);

    return 0;
}
