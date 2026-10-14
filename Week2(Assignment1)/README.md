# Movie Ticket Booking System in C
## About the Project
This is a simple movie ticket booking program written in C. It takes the customer's name, number of tickets, and ticket category as input. It then calculates the total amount and applies a discount if the customer books 5 or more tickets.

## Features
- Takes customer details as input.
- Provides three ticket categories: Standard, Premium, and Recliner.
- Calculates the subtotal based on ticket price and ticket count.
- Applies a 10% discount for booking 5 or more tickets.
- Displays the final bill with two decimal places.
- Checks for invalid ticket counts and category choices.

## Ticket Prices
| Category | Choice | Price |
|---|---:|---:|
| Standard | 1 | ₹150 |
| Premium | 2 | ₹250 |
| Recliner | 3 | ₹400 |

## How to Run the Program
1. Save the code in a file named `movie.c`.
2. Open the terminal in the folder where the file is saved.
3. Compile the program using:
   ```bash
   gcc movie.c -o movie
   ```
4. Run the program:
   ```bash
   ./movie
   ```

## Sample Input
```text
Rishika
5
2
```

## Sample Output
```text
Customer: Rishika
Ticket Category: Premium
Tickets: 5
Subtotal: 1250.00
Discount: 125.00
Final Bill: 1125.00
```

## Concepts Used
- Variables and constants
- Data types
- `scanf()` and `printf()`
- Arithmetic operators
- `if-else` statements
- `switch-case`
- Basic input validation

## Conclusion
This project helped me practice basic C programming concepts by creating a simple ticket booking system with bill calculation and discount rules.
