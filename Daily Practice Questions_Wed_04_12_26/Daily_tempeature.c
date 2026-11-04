#include <stdio.h>
#include <stdlib.h>

void readTemperatures(int *weather, int days) {
    printf("Enter temperatures:\n");

    for (int day = 0; day < days; day++) {
        scanf("%d", &weather[day]);
    }
}

void calculateWaitingDays(int *weather, int *waiting, int *stack, int days) {
    int peak = -1;

    for (int day = 0; day < days; day++) {

        while (peak != -1 && weather[day] > weather[stack[peak]]) {
            int previous = stack[peak--];
            waiting[previous] = day - previous;
        }

        stack[++peak] = day;
    }
}

void displayWaitingDays(int *waiting, int days) {
    printf("Waiting days: ");

    for (int day = 0; day < days; day++) {
        printf("%d ", waiting[day]);
    }

    printf("\n");
}

int main() {
    int days;

    printf("Enter number of days: ");
    scanf("%d", &days);

    if (days <= 0) {
        printf("Invalid number of days\n");
        return 1;
    }

    int *weather = malloc(days * sizeof(int));
    int *waiting = calloc(days, sizeof(int));
    int *stack = malloc(days * sizeof(int));

    if (weather == NULL || waiting == NULL || stack == NULL) {
        printf("Memory allocation failed\n");

        free(weather);
        free(waiting);
        free(stack);

        return 1;
    }

    readTemperatures(weather, days);

    calculateWaitingDays(weather, waiting, stack, days);

    displayWaitingDays(waiting, days);

    free(weather);
    free(waiting);
    free(stack);

    return 0;
}
