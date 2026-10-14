#include <stdio.h>

int main() {
    char name[50], category[20];
    int tickets, choice, price;
    float subtotal, discount, finalbill;

    const int standard = 150, premium = 250, recliner = 400;

    scanf("%49s", name);
    if (scanf("%d", &tickets) != 1 || tickets < 1 || tickets > 20) {
        printf("Invalid ticket count\n");
        return 0;
    }

    if (scanf("%d", &choice) != 1) {
        printf("Invalid category\n");
        return 0;
    }

    switch (choice) {
        case 1:
            price = standard;
            sprintf(category, "Standard");
            break;
        case 2:
            price = premium;
            sprintf(category, "Premium");
            break;
        case 3:
            price = recliner;
            sprintf(category, "Recliner");
            break;
        default:
            printf("Invalid category\n");
            return 0;
    }

    subtotal = tickets * price;

    if (tickets >= 5)
        discount = subtotal * 0.10;
    else
        discount = 0;

    finalbill = subtotal - discount;

    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalbill);

    return 0;
}