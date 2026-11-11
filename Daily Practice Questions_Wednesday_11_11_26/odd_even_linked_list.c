
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

    printf("Enter linked list elements:\n");

    for (int index = 0; index < count; index++) {
        int value;

        if (scanf("%d", &value) != 1) {
            printf("Invalid input\n");
            exit(1);
        }

        Node *newNode = createNode(value);

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

Node *rearrangeList(Node *head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }

    Node *odd = head;
    Node *even = head->next;
    Node *evenHead = even;

    while (even != NULL && even->next != NULL) {
        odd->next = even->next;
        odd = odd->next;

        even->next = odd->next;
        even = even->next;
    }

    odd->next = evenHead;

    return head;
}

void displayList(Node *head) {
    printf("Rearranged list: ");

    while (head != NULL) {
        printf("%d ", head->data);
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
    int count;

    printf("Enter number of nodes: ");

    if (scanf("%d", &count) != 1 || count < 0) {
        printf("Invalid number of nodes\n");
        return 1;
    }

    Node *head = createList(count);

    head = rearrangeList(head);

    displayList(head);

    freeList(head);

    return 0;
}
