#include <stdio.h>
#include <stdlib.h>

int **createMatrix(int rows, int columns) {
    int **matrix = malloc(rows * sizeof(int *));

    if (matrix == NULL) {
        return NULL;
    }

    for (int row = 0; row < rows; row++) {
        matrix[row] = malloc(columns * sizeof(int));

        if (matrix[row] == NULL) {
            for (int previous = 0; previous < row; previous++) {
                free(matrix[previous]);
            }

            free(matrix);
            return NULL;
        }
    }

    return matrix;
}

void readMatrix(int **matrix, int rows, int columns) {
    printf("Enter matrix elements:\n");

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            scanf("%d", &matrix[row][column]);
        }
    }
}

void spiralTraversal(int **matrix, int rows, int columns) {
    int startrow = 0;
    int endrow = rows - 1;
    int startcolumn = 0;
    int endcolumn = columns - 1;

    printf("Spiral order: ");

    while (startrow <= endrow && startcolumn <= endcolumn) {

        for (int column = startcolumn; column <= endcolumn; column++) {
            printf("%d ", matrix[startrow][column]);
        }
        startrow++;

        for (int row = startrow; row <= endrow; row++) {
            printf("%d ", matrix[row][endcolumn]);
        }
        endcolumn--;

        if (startrow <= endrow) {
            for (int column = endcolumn; column >= startcolumn; column--) {
                printf("%d ", matrix[endrow][column]);
            }
            endrow--;
        }

        if (startcolumn <= endcolumn) {
            for (int row = endrow; row >= startrow; row--) {
                printf("%d ", matrix[row][startcolumn]);
            }
            startcolumn++;
        }
    }

    printf("\n");
}

void freeMatrix(int **matrix, int rows) {
    for (int row = 0; row < rows; row++) {
        free(matrix[row]);
    }

    free(matrix);
}

int main() {
    int rows, columns;

    printf("Enter number of rows and columns: ");
    scanf("%d %d", &rows, &columns);

    if (rows <= 0 || columns <= 0) {
        printf("Invalid matrix size\n");
        return 1;
    }

    int **matrix = createMatrix(rows, columns);

    if (matrix == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readMatrix(matrix, rows, columns);
    spiralTraversal(matrix, rows, columns);
    freeMatrix(matrix, rows);

    return 0;
}
