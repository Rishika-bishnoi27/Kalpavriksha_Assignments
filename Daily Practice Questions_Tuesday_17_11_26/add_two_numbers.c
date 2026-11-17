
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *createNode(int value) {
    Node *newNode = malloc(sizeof(Node));

    if (newNode == NULL) {
        return NULL;
    }

    newNode->data = value;
    newNode->next = NULL;

    return newNode;
}

Node *createList(int count) {
    Node *head = NULL;
    Node *tail = NULL;

    printf("Enter digits in reverse order:\n");

    for (int index = 0; index < count; index++) {
        int digit;

        if (scanf("%d", &digit) != 1 || digit < 0 || digit > 9) {
            printf("Invalid digit\n");
            exit(1);
        }

        Node *newNode = createNode(digit);

        if (newNode == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        if (head == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    return head;
}

Node *addNumbers(Node *first, Node *second) {
    Node dummy;
    dummy.next = NULL;

    Node *tail = &dummy;
    int carry = 0;

    while (first != NULL || second != NULL || carry != 0) {
        int sum = carry;

        if (first != NULL) {
            sum += first->data;
            first = first->next;
        }

        if (second != NULL) {
            sum += second->data;
            second = second->next;
        }

        carry = sum / 10;

        Node *newNode = createNode(sum % 10);

        if (newNode == NULL) {
            Node *current = dummy.next;

            while (current != NULL) {
                Node *next = current->next;
                free(current);
                current = next;
            }

            return NULL;
        }

        tail->next = newNode;
        tail = newNode;
    }

    return dummy.next;
}

void displayList(Node *head) {
    while (head != NULL) {
        printf("%d", head->data);

        if (head->next != NULL) {
            printf(" -> ");
        }

        head = head->next;
    }

    printf("\n");
}

void freeList(Node *head) {
    while (head != NULL) {
        Node *current = head;
        head = head->next;
        free(current);
    }
}

int main() {
    int firstCount, secondCount;

    printf("Enter number of digits in first number: ");

    if (scanf("%d", &firstCount) != 1 || firstCount <= 0) {
        printf("Invalid input\n");
        return 1;
    }

    Node *first = createList(firstCount);

    printf("Enter number of digits in second number: ");

    if (scanf("%d", &secondCount) != 1 || secondCount <= 0) {
        printf("Invalid input\n");
        freeList(first);
        return 1;
    }

    Node *second = createList(secondCount);

    Node *result = addNumbers(first, second);

    if (result == NULL) {
        printf("Memory allocation failed\n");
    } else {
        printf("Sum: ");
        displayList(result);
    }

    freeList(first);
    freeList(second);
    freeList(result);

    return 0;
}
