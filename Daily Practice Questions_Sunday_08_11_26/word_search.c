
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int searchWord(char **board, int rows, int columns, char *word, int row, int column, int position) {
    if (word[position] == '\0') {
        return 1;
    }

    if (row < 0 || row >= rows || column < 0 || column >= columns) {
        return 0;
    }

    if (board[row][column] != word[position]) {
        return 0;
    }

    char letter = board[row][column];
    board[row][column] = '#';

    int found = searchWord(board, rows, columns, word, row + 1, column, position + 1) ||
                searchWord(board, rows, columns, word, row - 1, column, position + 1) ||
                searchWord(board, rows, columns, word, row, column + 1, position + 1) ||
                searchWord(board, rows, columns, word, row, column - 1, position + 1);

    board[row][column] = letter;

    return found;
}

int findWord(char **board, int rows, int columns, char *word) {
    if (word[0] == '\0') {
        return 1;
    }

    if (strlen(word) > (size_t)rows * columns) {
        return 0;
    }

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            if (board[row][column] == word[0]) {
                if (searchWord(board, rows, columns, word, row, column, 0)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

char **createBoard(int rows, int columns) {
    char **board = malloc(rows * sizeof(char *));

    if (board == NULL) {
        return NULL;
    }

    for (int row = 0; row < rows; row++) {
        board[row] = malloc(columns * sizeof(char));

        if (board[row] == NULL) {
            for (int previous = 0; previous < row; previous++) {
                free(board[previous]);
            }

            free(board);
            return NULL;
        }
    }

    return board;
}

void freeBoard(char **board, int rows) {
    for (int row = 0; row < rows; row++) {
        free(board[row]);
    }

    free(board);
}

int main() {
    int rows, columns;
    char word[1000];

    printf("Enter rows and columns: ");

    if (scanf("%d %d", &rows, &columns) != 2 || rows <= 0 || columns <= 0) {
        printf("Invalid board size\n");
        return 1;
    }

    char **board = createBoard(rows, columns);

    if (board == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    printf("Enter board characters:\n");

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            if (scanf(" %c", &board[row][column]) != 1) {
                printf("Invalid input\n");
                freeBoard(board, rows);
                return 1;
            }
        }
    }

    printf("Enter word to search: ");

    if (scanf("%999s", word) != 1) {
        printf("Invalid word\n");
        freeBoard(board, rows);
        return 1;
    }

    if (findWord(board, rows, columns, word)) {
        printf("Word found\n");
    } else {
        printf("Word not found\n");
    }

    freeBoard(board, rows);

    return 0;
}
