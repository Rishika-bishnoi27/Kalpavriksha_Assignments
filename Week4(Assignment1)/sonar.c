#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int **slot(int **grid, int size, int row, int col) {
    return grid + row * size + col;
}

int readSize(void) {
    char line[100];
    char *end;
    long size;
    size_t length;
    int inputChar;

    while (1) {
        printf("Enter matrix size (2-10): ");

        if (fgets(line, sizeof line, stdin) == NULL) {
            printf("\nNo input received.\n");
            exit(1);
        }

        length = strlen(line);

        if (length == sizeof line - 1 && line[length - 1] != '\n') {
            while ((inputChar = getchar()) != '\n' && inputChar != EOF)
                ;

            printf("Input too long. Enter a number from 2 to 10.\n");
            continue;
        }

        size = strtol(line, &end, 10);

        while (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')
            end++;

        if (end == line || *end != '\0') {
            printf("Invalid input. Enter a whole number.\n");
            continue;
        }

        if (size < 2 || size > 10) {
            printf("Size must be between 2 and 10.\n");
            continue;
        }

        return (int)size;
    }
}

void show(int **grid, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++)
            printf("%4d", **slot(grid, size, row, col));

        printf("\n");
    }
}

void rotate(int **grid, int size) {
    for (int layer = 0; layer < size / 2; layer++) {
        for (int step = layer; step < size - 1 - layer; step++) {

            int *temp = *slot(grid, size, layer, step);

            *slot(grid, size, layer, step) =
                *slot(grid, size, size - 1 - step, layer);

            *slot(grid, size, size - 1 - step, layer) =
                *slot(grid, size, size - 1 - layer, size - 1 - step);

            *slot(grid, size, size - 1 - layer, size - 1 - step) =
                *slot(grid, size, step, size - 1 - layer);

            *slot(grid, size, step, size - 1 - layer) = temp;
        }
    }
}

int smooth(int **grid, int size) {
    int *previous = malloc(size * sizeof(int));
    int *current = malloc(size * sizeof(int));

    if (previous == NULL || current == NULL) {
        free(previous);
        free(current);
        return 0;
    }

    for (int row = 0; row < size; row++) {

        for (int col = 0; col < size; col++)
            *(current + col) = **slot(grid, size, row, col);

        for (int col = 0; col < size; col++) {
            int sum = 0, count = 0;

            for (int neighbor = col - 1; neighbor <= col + 1; neighbor++) {

                if (neighbor < 0 || neighbor >= size)
                    continue;

                if (row > 0) {
                    sum += *(previous + neighbor);
                    count++;
                }

                sum += *(current + neighbor);
                count++;

                if (row < size - 1) {
                    sum += **slot(grid, size, row + 1, neighbor);
                    count++;
                }
            }

            **slot(grid, size, row, col) = sum / count;
        }

        int *temp = previous;
        previous = current;
        current = temp;
    }

    free(previous);
    free(current);

    return 1;
}

int main(void) {
    int size = readSize();
    int total = size * size;

    int **grid = malloc(total * sizeof(int *));

    if (grid == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    for (int index = 0; index < total; index++) {
        *(grid + index) = malloc(sizeof(int));

        if (*(grid + index) == NULL) {
            printf("Memory allocation failed.\n");

            for (int count = 0; count < index; count++)
                free(*(grid + count));

            free(grid);
            return 1;
        }

        **(grid + index) = rand() % 256;
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    show(grid, size);

    rotate(grid, size);

    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    show(grid, size);

    if (!smooth(grid, size)) {
        printf("Memory allocation failed.\n");
    } else {
        printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
        show(grid, size);
    }

    for (int index = 0; index < total; index++)
        free(*(grid + index));

    free(grid);

    return 0;
}