#include <stdio.h>
#include <stdlib.h>

void readArray(int *numbers, int count) {
    printf("Enter array elements:\n");

    for (int index = 0; index < count; index++) {
        if (scanf("%d", &numbers[index]) != 1) {
            printf("Invalid input\n");
            exit(1);
        }
    }
}

void findWindowMaximum(int *numbers, int count, int windowSize) {
    int *deque = malloc(count * sizeof(int));

    if (deque == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    int front = 0;
    int rear = -1;

    printf("Window maximums: ");

    for (int index = 0; index < count; index++) {

        while (front <= rear && deque[front] <= index - windowSize) {
            front++;
        }

        while (front <= rear && numbers[deque[rear]] <= numbers[index]) {
            rear--;
        }

        deque[++rear] = index;

        if (index >= windowSize - 1) {
            printf("%d ", numbers[deque[front]]);
        }
    }

    printf("\n");

    free(deque);
}

int main() {
    int count, windowSize;

    printf("Enter array size: ");

    if (scanf("%d", &count) != 1 || count <= 0) {
        printf("Invalid array size\n");
        return 1;
    }

    int *numbers = malloc(count * sizeof(int));

    if (numbers == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    readArray(numbers, count);

    printf("Enter window size: ");

    if (scanf("%d", &windowSize) != 1 || windowSize <= 0 || windowSize > count) {
        printf("Invalid window size\n");
        free(numbers);
        return 1;
    }

    findWindowMaximum(numbers, count, windowSize);

    free(numbers);

    return 0;
}
