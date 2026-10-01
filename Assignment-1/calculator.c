#include <stdio.h>
#include <ctype.h>

int main() {
    char str[1000];

    printf("Enter expression: ");
    fgets(str, sizeof(str), stdin);

    int i = 0;
    int num = 0;
    int ans = 0;
    int temp = 0;
    char op = '+';
    int needNum = 1;
    int hasNum = 0;

    while (str[i] != '\0' && str[i] != '\n') {
        if (str[i] == ' ') {
            i++;
            continue;
        }

        if (isdigit(str[i])) {
            if (needNum == 0) {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            num = 0;

            while (isdigit(str[i])) {
                num = num * 10 + (str[i] - '0');
                i++;
            }

            if (op == '+') {
                temp = num;
            }else if (op == '-') {
                temp = -num;
            }else if (op == '*') {
                temp = temp * num;
            }else if (op == '/') {

                if (num == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                temp = temp / num;
            }
            hasNum = 1;
            needNum = 0;
        }else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') {

            if (needNum == 1) {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            op = str[i];

            if (op == '+' || op == '-') {
                ans = ans + temp;
            }
            needNum = 1;
            i++;
        }else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }
    
    if (hasNum == 0 || needNum == 1) {
        printf("Error: Invalid expression.\n");
        return 0;
    }
    
    ans = ans + temp;

    printf("%d\n", ans);

    return 0;
}