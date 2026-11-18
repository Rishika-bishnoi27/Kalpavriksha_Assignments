
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define INPUT_BUFFER_SIZE 100
#define MIN_SIZE 2
#define MAX_SIZE 10
#define MAX_INTENSITY 255

int *getCellPointer(int **grid, int size, int row, int col) {
    return *(grid + row * size + col);
}

bool readMatrixSize(int *matrixSize) {
    char line[INPUT_BUFFER_SIZE];
    char *end;
    long size;
    size_t length;
    int inputChar;

    while (1) {
        printf("Enter matrix size (%d-%d): ", MIN_SIZE, MAX_SIZE);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nNo input received.\n");
            return false;
        }

        length = strlen(line);

        if (length == sizeof(line) - 1 && line[length - 1] != '\n') {
            while ((inputChar = getchar()) != '\n' && inputChar != EOF) {
            }

            printf("Input too long. Enter a number from %d to %d.\n",
                   MIN_SIZE, MAX_SIZE);
            continue;
        }

        size = strtol(line, &end, 10);

        while (*end == ' ' || *end == '\t' ||
               *end == '\n' || *end == '\r') {
            end++;
        }

        if (end == line || *end != '\0') {
            printf("Invalid input. Enter a whole number.\n");
            continue;
        }

        if (size < MIN_SIZE || size > MAX_SIZE) {
            printf("Size must be between %d and %d.\n",
                   MIN_SIZE, MAX_SIZE);
            continue;
        }

        *matrixSize = (int)size;
        return true;
    }
}

void printMatrix(int **grid, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            printf("%4d", *getCellPointer(grid, size, row, col));
        }
        printf("\n");
    }
}

void rotateMatrixClockwise(int **grid, int size) {
    for (int layer = 0; layer < size / 2; layer++) {
        for (int step = layer; step < size - 1 - layer; step++) {

            int *temp = getCellPointer(grid, size, layer, step);

            *getCellPointer(grid, size, layer, step) =
                *getCellPointer(grid, size, size - 1 - step, layer);

            *getCellPointer(grid, size, size - 1 - step, layer) =
                *getCellPointer(grid, size, size - 1 - layer, size - 1 - step);

            *getCellPointer(grid, size, size - 1 - layer, size - 1 - step) =
                *getCellPointer(grid, size, step, size - 1 - layer);

            *getCellPointer(grid, size, step, size - 1 - layer) = *temp;
        }
    }
}

int applySmoothingFilter(int **grid, int size) {
    int *previous = malloc(size * sizeof(int));
    int *current = malloc(size * sizeof(int));

    if (previous == NULL || current == NULL) {
        free(previous);
        free(current);
        return 0;
    }

    for (int row = 0; row < size; row++) {

        for (int col = 0; col < size; col++) {
            *(current + col) = *getCellPointer(grid, size, row, col);
        }

        for (int col = 0; col < size; col++) {
            int sum = 0;
            int count = 0;

            for (int neighbor = col - 1; neighbor <= col + 1; neighbor++) {

                if (neighbor < 0 || neighbor >= size) {
                    continue;
                }

                if (row > 0) {
                    sum += *(previous + neighbor);
                    count++;
                }

                sum += *(current + neighbor);
                count++;

                if (row < size - 1) {
                    sum += *getCellPointer(grid, size, row + 1, neighbor);
                    count++;
                }
            }

            *getCellPointer(grid, size, row, col) = sum / count;
        }

        int *temp = previous;
        previous = current;
        current = temp;
    }

    free(previous);
    free(current);

    return 1;
}

void freeMatrix(int **grid, int size) {
    if (grid == NULL) {
        return;
    }

    for (int index = 0; index < size * size; index++) {
        free(*(grid + index));
    }

    free(grid);
}

int **createMatrix(int size) {
    int total = size * size;

    int **grid = calloc(total, sizeof(int *));

    if (grid == NULL) {
        return NULL;
    }

    for (int index = 0; index < total; index++) {
        *(grid + index) = malloc(sizeof(int));

        if (*(grid + index) == NULL) {
            freeMatrix(grid, size);
            return NULL;
        }
    }

    return grid;
}

int main(void) {
    int size = 0;

    if (!readMatrixSize(&size)) {
        return 1;
    }

    int total = size * size;

    int **grid = createMatrix(size);

    if (grid == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    for (int index = 0; index < total; index++) {
        **(grid + index) = rand() % (MAX_INTENSITY + 1);
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix(grid, size);

    rotateMatrixClockwise(grid, size);

    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    printMatrix(grid, size);

    if (!applySmoothingFilter(grid, size)) {
        printf("Memory allocation failed.\n");
        freeMatrix(grid, size);
        return 1;
    } else {
        printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
        printMatrix(grid, size);
    }

    freeMatrix(grid, size);

    return 0;
}
