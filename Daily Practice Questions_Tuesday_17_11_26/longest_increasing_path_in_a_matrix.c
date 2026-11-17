
#include <stdio.h>
#include <stdlib.h>

int findLongestPath(int **matrix, int **memo, int rows, int columns, int row, int column) {
    if (memo[row][column] != 0) {
        return memo[row][column];
    }

    int directions[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    int maximum = 1;

    for (int direction = 0; direction < 4; direction++) {
        int nextrow = row + directions[direction][0];
        int nextcolumn = column + directions[direction][1];

        if (nextrow >= 0 && nextrow < rows &&
            nextcolumn >= 0 && nextcolumn < columns &&
            matrix[nextrow][nextcolumn] > matrix[row][column]) {

            int length = 1 + findLongestPath(
                matrix, memo, rows, columns, nextrow, nextcolumn
            );

            if (length > maximum) {
                maximum = length;
            }
        }
    }

    memo[row][column] = maximum;

    return maximum;
}

int findMaximumPath(int **matrix, int rows, int columns, int **memo) {
    int maximum = 0;

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            int length = findLongestPath(
                matrix, memo, rows, columns, row, column
            );

            if (length > maximum) {
                maximum = length;
            }
        }
    }

    return maximum;
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
    int **memo = malloc(rows * sizeof(int *));

    if (matrix == NULL || memo == NULL) {
        printf("Memory allocation failed\n");
        free(matrix);
        free(memo);
        return 1;
    }

    int allocated = 0;

    for (int row = 0; row < rows; row++) {
        matrix[row] = malloc(columns * sizeof(int));
        memo[row] = calloc(columns, sizeof(int));

        if (matrix[row] == NULL || memo[row] == NULL) {
            printf("Memory allocation failed\n");

            for (int previous = 0; previous <= row; previous++) {
                if (previous < rows) {
                    free(matrix[previous]);
                    free(memo[previous]);
                }
            }

            free(matrix);
            free(memo);
            return 1;
        }

        allocated++;
    }

    printf("Enter matrix elements:\n");

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            if (scanf("%d", &matrix[row][column]) != 1) {
                printf("Invalid input\n");

                for (int previous = 0; previous < allocated; previous++) {
                    free(matrix[previous]);
                    free(memo[previous]);
                }

                free(matrix);
                free(memo);
                return 1;
            }
        }
    }

    int result = findMaximumPath(matrix, rows, columns, memo);

    printf("Longest increasing path: %d\n", result);

    for (int row = 0; row < rows; row++) {
        free(matrix[row]);
        free(memo[row]);
    }

    free(matrix);
    free(memo);

    return 0;
}
