
#include <stdio.h>


enum Category {
    STANDARD = 1,
    PREMIUM,
    RECLINER
};

int main() {
    char customerName[50];
    int tickets, category;

    const float STANDARD_PRICE = 150.0;
    const float PREMIUM_PRICE = 250.0;
    const float RECLINER_PRICE = 400.0;

    float ticketPrice;
    float subtotal;
    float discount;
    float finalBill;

    printf("Enter customer name: ");
    scanf("%49s", customerName);

    printf("Enter number of tickets: ");
    scanf("%d", &tickets);

    printf("Enter ticket category (1-Standard, 2-Premium, 3-Recliner): ");
    scanf("%d", &category);

    if (tickets < 1 || tickets > 20) {
        printf("Invalid number of tickets. Tickets must be between 1 and 20.\n");
        return 0;
    }

    switch (category) {
        case STANDARD:
            ticketPrice = STANDARD_PRICE;
            break;

        case PREMIUM:
            ticketPrice = PREMIUM_PRICE;
            break;

        case RECLINER:
            ticketPrice = RECLINER_PRICE;
            break;

        default:
            printf("Invalid ticket category.\n");
            return 0;
    }

    subtotal = tickets * ticketPrice;

    if (tickets >= 5) {
        discount = subtotal * 0.10;
    } else {
        discount = 0.0;
    }

    finalBill = subtotal - discount;

    printf("\nCustomer: %s\n", customerName);

    switch (category) {
        case STANDARD:
            printf("Ticket Category: Standard\n");
            break;

        case PREMIUM:
            printf("Ticket Category: Premium\n");
            break;

        case RECLINER:
            printf("Ticket Category: Recliner\n");
            break;
    }

    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalBill);

    return 0;
}
