
#include <stdio.h>
#include <string.h>
#include <limits.h>

void findMinimumWindow(char *text, char *pattern) {
    int required[256] = {0};
    int window[256] = {0};

    int textLength = strlen(text);
    int patternLength = strlen(pattern);

    if (patternLength == 0 || textLength < patternLength) {
        printf("Minimum window: \"\"\n");
        return;
    }

    for (int index = 0; index < patternLength; index++) {
        required[(unsigned char)pattern[index]]++;
    }

    int left = 0;
    int matched = 0;
    int minimumLength = INT_MAX;
    int minimumStart = 0;

    for (int right = 0; right < textLength; right++) {
        unsigned char character = (unsigned char)text[right];

        window[character]++;

        if (required[character] > 0 &&
            window[character] <= required[character]) {
            matched++;
        }

        while (matched == patternLength) {
            int currentLength = right - left + 1;

            if (currentLength < minimumLength) {
                minimumLength = currentLength;
                minimumStart = left;
            }

            unsigned char removed = (unsigned char)text[left];

            window[removed]--;

            if (required[removed] > 0 &&
                window[removed] < required[removed]) {
                matched--;
            }

            left++;
        }
    }

    if (minimumLength == INT_MAX) {
        printf("Minimum window: \"\"\n");
        return;
    }

    printf("Minimum window: ");

    for (int index = minimumStart; index < minimumStart + minimumLength; index++) {
        printf("%c", text[index]);
    }

    printf("\n");
}

int main() {
    char text[100000];
    char pattern[100000];

    printf("Enter main string: ");

    if (scanf("%99999s", text) != 1) {
        printf("Invalid input\n");
        return 1;
    }

    printf("Enter target string: ");

    if (scanf("%99999s", pattern) != 1) {
        printf("Invalid input\n");
        return 1;
    }

    findMinimumWindow(text, pattern);

    return 0;
}
