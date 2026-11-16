
#include <stdio.h>
#include <stdlib.h>

void readMatrix(int **matrix, int rows, int columns) {
    printf("Enter binary matrix elements:\n");

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            scanf("%d", &matrix[row][column]);
        }
    }
}

int findLargestRectangle(int *heights, int columns, int *stack) {
    int peak = -1;
    int maximumArea = 0;

    for (int index = 0; index <= columns; index++) {
        int currentHeight = (index == columns) ? 0 : heights[index];

        while (peak != -1 && heights[stack[peak]] > currentHeight) {
            int height = heights[stack[peak--]];

            int width = (peak == -1) ? index : index - stack[peak] - 1;

            int area = height * width;

            if (area > maximumArea) {
                maximumArea = area;
            }
        }

        stack[++peak] = index;
    }

    return maximumArea;
}

int findMaximalRectangle(int **matrix, int rows, int columns) {
    int *heights = calloc(columns, sizeof(int));
    int *stack = malloc((columns + 1) * sizeof(int));

    if (heights == NULL || stack == NULL) {
        free(heights);
        free(stack);
        return -1;
    }

    int maximumArea = 0;

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            if (matrix[row][column] == 1) {
                heights[column]++;
            } else {
                heights[column] = 0;
            }
        }

        int currentArea = findLargestRectangle(heights, columns, stack);

        if (currentArea > maximumArea) {
            maximumArea = currentArea;
        }
    }

    free(heights);
    free(stack);

    return maximumArea;
}

int main() {
    int rows, columns;

    printf("Enter rows and columns: ");

    if (scanf("%d %d", &rows, &columns) != 2 ||
        rows <= 0 || columns <= 0) {
        printf("Invalid matrix size\n");
        return 1;
    }

    int **matrix = malloc(rows * sizeof(int *));

    if (matrix == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int row = 0; row < rows; row++) {
        matrix[row] = malloc(columns * sizeof(int));

        if (matrix[row] == NULL) {
            for (int previous = 0; previous < row; previous++) {
                free(matrix[previous]);
            }

            free(matrix);
            printf("Memory allocation failed\n");
            return 1;
        }
    }

    readMatrix(matrix, rows, columns);

    int result = findMaximalRectangle(matrix, rows, columns);

    if (result == -1) {
        printf("Memory allocation failed\n");
    } else {
        printf("Maximum rectangle area: %d\n", result);
    }

    for (int row = 0; row < rows; row++) {
        free(matrix[row]);
    }

    free(matrix);

    return 0;
}
