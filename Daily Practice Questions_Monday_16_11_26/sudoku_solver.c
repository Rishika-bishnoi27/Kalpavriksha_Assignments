
#include <stdio.h>

#define SIZE 9

void readBoard(int board[SIZE][SIZE]) {
    printf("Enter Sudoku board (use 0 for empty cells):\n");

    for (int row = 0; row < SIZE; row++) {
        for (int column = 0; column < SIZE; column++) {
            if (scanf("%d", &board[row][column]) != 1 ||
                board[row][column] < 0 || board[row][column] > 9) {
                printf("Invalid input\n");
                return;
            }
        }
    }
}

int isSafe(int board[SIZE][SIZE], int row, int column, int digit) {
    for (int index = 0; index < SIZE; index++) {
        if (board[row][index] == digit ||
            board[index][column] == digit) {
            return 0;
        }
    }

    int startrow = (row / 3) * 3;
    int startcolumn = (column / 3) * 3;

    for (int boxrow = startrow; boxrow < startrow + 3; boxrow++) {
        for (int boxcolumn = startcolumn; boxcolumn < startcolumn + 3; boxcolumn++) {
            if (board[boxrow][boxcolumn] == digit) {
                return 0;
            }
        }
    }

    return 1;
}

int solveSudoku(int board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int column = 0; column < SIZE; column++) {

            if (board[row][column] == 0) {

                for (int digit = 1; digit <= 9; digit++) {
                    if (isSafe(board, row, column, digit)) {

                        board[row][column] = digit;

                        if (solveSudoku(board)) {
                            return 1;
                        }

                        board[row][column] = 0;
                    }
                }

                return 0;
            }
        }
    }

    return 1;
}

void displayBoard(int board[SIZE][SIZE]) {
    printf("Solved Sudoku:\n");

    for (int row = 0; row < SIZE; row++) {
        for (int column = 0; column < SIZE; column++) {
            printf("%d ", board[row][column]);
        }

        printf("\n");
    }
}

int main() {
    int board[SIZE][SIZE];

    readBoard(board);

    if (solveSudoku(board)) {
        displayBoard(board);
    } else {
        printf("No solution exists\n");
    }

    return 0;
}
