
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
            if (scanf("%d", &matrix[row][column]) != 1) {
                printf("Invalid input\n");
                exit(1);
            }
        }
    }
}

void setZeroes(int **matrix, int rows, int columns) {
    int firstrow = 0;
    int firstcolumn = 0;

    for (int column = 0; column < columns; column++) {
        if (matrix[0][column] == 0) {
            firstrow = 1;
            break;
        }
    }

    for (int row = 0; row < rows; row++) {
        if (matrix[row][0] == 0) {
            firstcolumn = 1;
            break;
        }
    }

    for (int row = 1; row < rows; row++) {
        for (int column = 1; column < columns; column++) {
            if (matrix[row][column] == 0) {
                matrix[row][0] = 0;
                matrix[0][column] = 0;
            }
        }
    }

    for (int row = 1; row < rows; row++) {
        if (matrix[row][0] == 0) {
            for (int column = 1; column < columns; column++) {
                matrix[row][column] = 0;
            }
        }
    }

    for (int column = 1; column < columns; column++) {
        if (matrix[0][column] == 0) {
            for (int row = 1; row < rows; row++) {
                matrix[row][column] = 0;
            }
        }
    }

    if (firstrow) {
        for (int column = 0; column < columns; column++) {
            matrix[0][column] = 0;
        }
    }

    if (firstcolumn) {
        for (int row = 0; row < rows; row++) {
            matrix[row][0] = 0;
        }
    }
}

void displayMatrix(int **matrix, int rows, int columns) {
    printf("Updated matrix:\n");

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            printf("%d ", matrix[row][column]);
        }

        printf("\n");
    }
}

void freeMatrix(int **matrix, int rows) {
    for (int row = 0; row < rows; row++) {
        free(matrix[row]);
    }

    free(matrix);
}

int main() {
    int rows, columns;

    printf("Enter rows and columns: ");

    if (scanf("%d %d", &rows, &columns) != 2 || rows <= 0 || columns <= 0) {
        printf("Invalid matrix size\n");
        return 1;
    }

    int **matrix = createMatrix(rows, columns);

    if (matrix == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readMatrix(matrix, rows, columns);
    setZeroes(matrix, rows, columns);
    displayMatrix(matrix, rows, columns);

    freeMatrix(matrix, rows);

    return 0;
}
