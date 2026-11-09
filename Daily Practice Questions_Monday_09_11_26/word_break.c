#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isWordPresent(char *word, char dictionary[][100], int wordcount) {
    for (int index = 0; index < wordcount; index++) {
        if (strcmp(word, dictionary[index]) == 0) {
            return 1;
        }
    }

    return 0;
}

int canBreakWord(char *text, char dictionary[][100], int wordcount) {
    int length = strlen(text);

    int *possible = calloc(length + 1, sizeof(int));

    if (possible == NULL) {
        return -1;
    }

    possible[0] = 1;

    char word[100];

    for (int end = 1; end <= length; end++) {
        for (int start = 0; start < end; start++) {
            if (possible[start] && end - start < (int)sizeof(word)) {
                int size = end - start;

                strncpy(word, text + start, size);
                word[size] = '\0';

                if (isWordPresent(word, dictionary, wordcount)) {
                    possible[end] = 1;
                    break;
                }
            }
        }
    }

    int result = possible[length];

    free(possible);

    return result;
}

int main() {
    char text[1000];
    char dictionary[100][100];
    int wordcount;

    printf("Enter string: ");

    if (scanf("%999s", text) != 1) {
        printf("Invalid input\n");
        return 1;
    }

    printf("Enter number of dictionary words: ");

    if (scanf("%d", &wordcount) != 1 || wordcount < 0 || wordcount > 100) {
        printf("Invalid dictionary size\n");
        return 1;
    }

    printf("Enter dictionary words:\n");

    for (int index = 0; index < wordcount; index++) {
        if (scanf("%99s", dictionary[index]) != 1) {
            printf("Invalid input\n");
            return 1;
        }
    }

    int result = canBreakWord(text, dictionary, wordcount);

    if (result == -1) {
        printf("Memory allocation failed\n");
    } else if (result == 1) {
        printf("Word can be segmented\n");
    } else {
        printf("Word cannot be segmented\n");
    }

    return 0;
}
