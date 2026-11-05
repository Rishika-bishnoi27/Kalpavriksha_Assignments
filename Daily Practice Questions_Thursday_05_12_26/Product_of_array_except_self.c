
#include <stdio.h>
#include <stdlib.h>

void readArray(int *numbers, int count) {
    printf("Enter array elements:\n");

    for (int index = 0; index < count; index++) {
        scanf("%d", &numbers[index]);
    }
}

void calculateProduct(int *numbers, int *products, int count) {
    int prefix = 1;

    for (int index = 0; index < count; index++) {
        products[index] = prefix;
        prefix *= numbers[index];
    }

    int suffix = 1;

    for (int index = count - 1; index >= 0; index--) {
        products[index] *= suffix;
        suffix *= numbers[index];
    }
}

void displayArray(int *products, int count) {
    printf("Product array: ");

    for (int index = 0; index < count; index++) {
        printf("%d ", products[index]);
    }

    printf("\n");
}

int main() {
    int count;

    printf("Enter array size: ");
    scanf("%d", &count);

    if (count <= 0) {
        printf("Invalid array size\n");
        return 1;
    }

    int *numbers = malloc(count * sizeof(int));
    int *products = malloc(count * sizeof(int));

    if (numbers == NULL || products == NULL) {
        printf("Memory allocation failed\n");
        free(numbers);
        free(products);
        return 1;
    }

    readArray(numbers, count);

    calculateProduct(numbers, products, count);

    displayArray(products, count);

    free(numbers);
    free(products);

    return 0;
}
