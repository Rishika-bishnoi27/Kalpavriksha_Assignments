
#include <stdio.h>
#include <stdlib.h>

int **createMatrix(int size) {
    int **matrix = malloc(size * sizeof(int *));

    if (matrix == NULL) {
        return NULL;
    }

    for (int row = 0; row < size; row++) {
        matrix[row] = malloc(size * sizeof(int));

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

void readMatrix(int **matrix, int size) {
    printf("Enter matrix elements:\n");

    for (int row = 0; row < size; row++) {
        for (int column = 0; column < size; column++) {
            scanf("%d", &matrix[row][column]);
        }
    }
}

void rotateMatrix(int **matrix, int size) {
    for (int row = 0; row < size; row++) {
        for (int column = row + 1; column < size; column++) {
            int value = matrix[row][column];
            matrix[row][column] = matrix[column][row];
            matrix[column][row] = value;
        }
    }

    for (int row = 0; row < size; row++) {
        int left = 0;
        int right = size - 1;

        while (left < right) {
            int value = matrix[row][left];
            matrix[row][left] = matrix[row][right];
            matrix[row][right] = value;

            left++;
            right--;
        }
    }
}

void displayMatrix(int **matrix, int size) {
    printf("Rotated matrix:\n");

    for (int row = 0; row < size; row++) {
        for (int column = 0; column < size; column++) {
            printf("%d ", matrix[row][column]);
        }

        printf("\n");
    }
}

void freeMatrix(int **matrix, int size) {
    for (int row = 0; row < size; row++) {
        free(matrix[row]);
    }

    free(matrix);
}

int main() {
    int size;

    printf("Enter matrix size: ");

    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("Invalid matrix size\n");
        return 1;
    }

    int **matrix = createMatrix(size);

    if (matrix == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readMatrix(matrix, size);
    rotateMatrix(matrix, size);
    displayMatrix(matrix, size);

    freeMatrix(matrix, size);

    return 0;
}
