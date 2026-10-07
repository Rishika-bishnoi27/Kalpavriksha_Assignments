#include <stdio.h>
#include <ctype.h>

int main() {
    char expression[1000];

    printf("Enter expression: ");

    if (fgets(expression, sizeof(expression), stdin) == NULL) {
        printf("Error: Invalid input.\n");
        return 0;
    }

    int index = 0;
    long long number = 0;
    long long result = 0;
    long long current = 0;
    char operator = '+';
    int expecting = 1;
    int found = 0;

    while (expression[index] != '\0' && expression[index] != '\n') {

        if (expression[index] == ' ') {
            index++;
            continue;
        }

        if (isdigit(expression[index])) {

            if (expecting == 0) {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            number = 0;

            while (isdigit(expression[index])) {
                number = number * 10 + (expression[index] - '0');
                index++;
            }

            if (operator == '+') {
                current = number;
            }
            else if (operator == '-') {
                current = -number;
            }
            else if (operator == '*') {
                current = current * number;
            }
            else if (operator == '/') {

                if (number == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }

                current = current / number;
            }

            found = 1;
            expecting = 0;
        }

        else if (expression[index] == '+' ||
                 expression[index] == '-' ||
                 expression[index] == '*' ||
                 expression[index] == '/') {

            if (expecting == 1) {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            operator = expression[index];

            if (operator == '+' || operator == '-') {
                result = result + current;
            }

            expecting = 1;
            index++;
        }

        else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    if (found == 0 || expecting == 1) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    result = result + current;

    printf("%lld\n", result);

    return 0;
}