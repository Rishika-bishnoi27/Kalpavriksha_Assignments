#include <stdio.h>
#include <stdlib.h>

int solutionCount = 0;

void displayBoard(int **board, int size) {
    solutionCount++;

    printf("\nSolution %d:\n", solutionCount);

    for (int row = 0; row < size; row++) {
        for (int column = 0; column < size; column++) {
            printf("%c ", board[row][column] ? 'Q' : '.');
        }

        printf("\n");
    }
}

int isSafe(int **board, int size, int row, int column) {
    for (int previous = 0; previous < row; previous++) {
        if (board[previous][column]) {
            return 0;
        }

        if (column - (row - previous) >= 0 &&
            board[previous][column - (row - previous)]) {
            return 0;
        }

        if (column + (row - previous) < size &&
            board[previous][column + (row - previous)]) {
            return 0;
        }
    }

    return 1;
}

void solveQueens(int **board, int size, int row) {
    if (row == size) {
        displayBoard(board, size);
        return;
    }

    for (int column = 0; column < size; column++) {
        if (isSafe(board, size, row, column)) {
            board[row][column] = 1;

            solveQueens(board, size, row + 1);

            board[row][column] = 0;
        }
    }
}

int main() {
    int size;

    printf("Enter number of queens: ");

    if (scanf("%d", &size) != 1 || size <= 0 || size > 15) {
        printf("Enter a size between 1 and 15\n");
        return 1;
    }

    int **board = malloc(size * sizeof(int *));

    if (board == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int row = 0; row < size; row++) {
        board[row] = calloc(size, sizeof(int));

        if (board[row] == NULL) {
            for (int previous = 0; previous < row; previous++) {
                free(board[previous]);
            }

            free(board);
            printf("Memory allocation failed\n");
            return 1;
        }
    }

    solveQueens(board, size, 0);

    if (solutionCount == 0) {
        printf("No solution exists\n");
    } else {
        printf("\nTotal solutions: %d\n", solutionCount);
    }

    for (int row = 0; row < size; row++) {
        free(board[row]);
    }

    free(board);

    return 0;
}
