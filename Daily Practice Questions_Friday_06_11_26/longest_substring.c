
#include <stdio.h>
#include <string.h>

int findLongestSubstring(char *word) {
    int lastSeen[256];

    for (int index = 0; index < 256; index++) {
        lastSeen[index] = -1;
    }

    int start = 0;
    int maximum = 0;

    for (int end = 0; word[end] != '\0'; end++) {
        unsigned char character = (unsigned char)word[end];

        if (lastSeen[character] >= start) {
            start = lastSeen[character] + 1;
        }

        lastSeen[character] = end;

        int length = end - start + 1;

        if (length > maximum) {
            maximum = length;
        }
    }

    return maximum;
}

int main() {
    char word[100000];

    printf("Enter a string: ");

    if (fgets(word, sizeof(word), stdin) == NULL) {
        return 1;
    }

    word[strcspn(word, "\n")] = '\0';

    int result = findLongestSubstring(word);

    printf("Longest substring length: %d\n", result);

    return 0;
}
