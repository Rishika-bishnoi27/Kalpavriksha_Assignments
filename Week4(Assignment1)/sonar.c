#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int **slot(int **g, int n, int r, int c) {
    return g + r * n + c;
}

int readSize(void) {
    char line[100];
    char *end;
    long n;
    size_t len;
    int ch;

    while (1) {
        printf("Enter matrix size (2-10): ");
        if (fgets(line, sizeof line, stdin) == NULL) {
            printf("\nNo input received.\n");
            exit(1);
        }
        len = strlen(line);
        if (len == sizeof line - 1 && line[len - 1] != '\n') {
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            printf("Input too long. Enter a number from 2 to 10.\n");
            continue;
        }
        n = strtol(line, &end, 10);
        while (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')
            end++;
        if (end == line || *end != '\0') {
            printf("Invalid input. Enter a whole number.\n");
            continue;
        }
        if (n < 2 || n > 10) {
            printf("Size must be between 2 and 10.\n");
            continue;
        }
        return (int)n;
    }
}

void show(int **g, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%4d", **slot(g, n, i, j));
        printf("\n");
    }
}

void rotate(int **g, int n) {
    for (int l = 0; l < n / 2; l++) {
        for (int k = l; k < n - 1 - l; k++) {
            int *tmp = *slot(g, n, l, k);
            *slot(g, n, l, k) = *slot(g, n, n - 1 - k, l);
            *slot(g, n, n - 1 - k, l) = *slot(g, n, n - 1 - l, n - 1 - k);
            *slot(g, n, n - 1 - l, n - 1 - k) = *slot(g, n, k, n - 1 - l);
            *slot(g, n, k, n - 1 - l) = tmp;
        }
    }
}

int smooth(int **g, int n) {
    int *prev = malloc(n * sizeof(int));
    int *cur = malloc(n * sizeof(int));
    if (prev == NULL || cur == NULL) {
        free(prev);
        free(cur);
        return 0;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            *(cur + j) = **slot(g, n, i, j);

        for (int j = 0; j < n; j++) {
            int sum = 0, cnt = 0;
            for (int c = j - 1; c <= j + 1; c++) {
                if (c < 0 || c >= n)
                    continue;
                if (i > 0) {
                    sum += *(prev + c);
                    cnt++;
                }
                sum += *(cur + c);
                cnt++;
                if (i < n - 1) {
                    sum += **slot(g, n, i + 1, c);
                    cnt++;
                }
            }
            **slot(g, n, i, j) = sum / cnt;
        }

        int *tmp = prev;
        prev = cur;
        cur = tmp;
    }

    free(prev);
    free(cur);
    return 1;
}

int main(void) {
    int n = readSize();
    int total = n * n;

    int **grid = malloc(total * sizeof(int *));
    if (grid == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    srand((unsigned)time(NULL));
    for (int k = 0; k < total; k++) {
        *(grid + k) = malloc(sizeof(int));
        if (*(grid + k) == NULL) {
            printf("Memory allocation failed.\n");
            for (int m = 0; m < k; m++)
                free(*(grid + m));
            free(grid);
            return 1;
        }
        **(grid + k) = rand() % 256;
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    show(grid, n);

    rotate(grid, n);
    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    show(grid, n);

    if (!smooth(grid, n)) {
        printf("Memory allocation failed.\n");
    } else {
        printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
        show(grid, n);
    }

    for (int k = 0; k < total; k++)
        free(*(grid + k));
    free(grid);
    return 0;
}