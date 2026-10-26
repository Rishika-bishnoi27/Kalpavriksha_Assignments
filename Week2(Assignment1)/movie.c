
#include <stdio.h>

#define NAMECAPACITY 50

enum {
    MINTICKETS = 1,
    MAXTICKETS = 20,
    DISCOUNTTICKETS = 5
};

enum {
    STANDARD = 1,
    PREMIUM = 2,
    RECLINER = 3
};

static const double STANDARDPRICE = 150.0;
static const double PREMIUMPRICE = 250.0;
static const double RECLINERPRICE = 400.0;
static const double DISCOUNTRATE = 0.10;

static int validtickets(int tickets)
{
    return tickets >= MINTICKETS && tickets <= MAXTICKETS;
}

static int getcategory(int choice, const char **category, double *price)
{
    switch (choice) {
        case STANDARD:
            *category = "Standard";
            *price = STANDARDPRICE;
            return 1;

        case PREMIUM:
            *category = "Premium";
            *price = PREMIUMPRICE;
            return 1;

        case RECLINER:
            *category = "Recliner";
            *price = RECLINERPRICE;
            return 1;

        default:
            return 0;
    }
}

static double getdiscount(double subtotal, int tickets)
{
    if (tickets >= DISCOUNTTICKETS)
        return subtotal * DISCOUNTRATE;

    return 0.0;
}

int main(void)
{
    char customer[NAMECAPACITY];
    int tickets = 0;
    int choice = 0;
    const char *category = NULL;
    double price = 0.0;
    double subtotal = 0.0;
    double discount = 0.0;
    double finalbill = 0.0;

    if (scanf("%49s %d %d", customer, &tickets, &choice) != 3) {
        printf("Error: Invalid input.\n");
        return 1;
    }

    if (!validtickets(tickets)) {
        printf("Error: Number of tickets must be between %d and %d.\n",
               MINTICKETS, MAXTICKETS);
        return 1;
    }

    if (!getcategory(choice, &category, &price)) {
        printf("Error: Invalid ticket category. Choose 1, 2, or 3.\n");
        return 1;
    }

    subtotal = price * tickets;
    discount = getdiscount(subtotal, tickets);
    finalbill = subtotal - discount;

    printf("Customer: %s\n", customer);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalbill);

    return 0;
}
