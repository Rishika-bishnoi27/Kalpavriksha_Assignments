
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *decodeString(char *encoded) {
    int length = strlen(encoded);
    int *countStack = malloc(length * sizeof(int));
    char **stringStack = malloc(length * sizeof(char *));

    char *result = calloc(100000, sizeof(char));

    if (countStack == NULL || stringStack == NULL || result == NULL) {
        free(countStack);
        free(stringStack);
        free(result);
        return NULL;
    }

    int top = -1;
    int number = 0;
    int position = 0;

    for (int index = 0; index < length; index++) {

        if (isdigit((unsigned char)encoded[index])) {
            number = number * 10 + (encoded[index] - '0');
        }

        else if (encoded[index] == '[') {
            top++;

            countStack[top] = number;
            stringStack[top] = malloc(strlen(result) + 1);

            if (stringStack[top] == NULL) {
                for (int previous = 0; previous < top; previous++) {
                    free(stringStack[previous]);
                }

                free(countStack);
                free(stringStack);
                free(result);
                return NULL;
            }

            strcpy(stringStack[top], result);

            result[0] = '\0';
            number = 0;
        }

        else if (encoded[index] == ']') {
            char *previous = stringStack[top];
            int repeat = countStack[top--];

            size_t currentLength = strlen(result);
            size_t previousLength = strlen(previous);

            if (repeat > 0 && currentLength > (99999 - previousLength) / (size_t)repeat) {
                free(previous);

                for (int remaining = 0; remaining <= top; remaining++) {
                    free(stringStack[remaining]);
                }

                free(countStack);
                free(stringStack);
                free(result);
                return NULL;
            }

            char *decoded = malloc(previousLength + currentLength * (size_t)repeat + 1);

            if (decoded == NULL) {
                free(previous);

                for (int remaining = 0; remaining <= top; remaining++) {
                    free(stringStack[remaining]);
                }

                free(countStack);
                free(stringStack);
                free(result);
                return NULL;
            }

            strcpy(decoded, previous);

            for (int repeatIndex = 0; repeatIndex < repeat; repeatIndex++) {
                strcat(decoded, result);
            }

            strcpy(result, decoded);

            free(decoded);
            free(previous);
        }

        else {
            size_t currentLength = strlen(result);

            if (currentLength >= 99999) {
                for (int remaining = 0; remaining <= top; remaining++) {
                    free(stringStack[remaining]);
                }

                free(countStack);
                free(stringStack);
                free(result);
                return NULL;
            }

            result[currentLength] = encoded[index];
            result[currentLength + 1] = '\0';
        }
    }

    free(countStack);
    free(stringStack);

    return result;
}

int main() {
    char encoded[10000];

    printf("Enter encoded string: ");

    if (scanf("%9999s", encoded) != 1) {
        printf("Invalid input\n");
        return 1;
    }

    char *decoded = decodeString(encoded);

    if (decoded == NULL) {
        printf("Decoding failed or output is too large\n");
        return 1;
    }

    printf("Decoded string: %s\n", decoded);

    free(decoded);

    return 0;
}
