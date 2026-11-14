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

Node *reverseGroups(Node *head, int groupSize) {
    Node dummy;
    dummy.next = head;

    Node *previousGroup = &dummy;

    while (1) {
        Node *groupEnd = previousGroup;

        for (int count = 0; count < groupSize && groupEnd != NULL; count++) {
            groupEnd = groupEnd->next;
        }

        if (groupEnd == NULL) {
            break;
        }

        Node *groupStart = previousGroup->next;
        Node *nextGroup = groupEnd->next;

        Node *previous = nextGroup;
        Node *current = groupStart;

        while (current != nextGroup) {
            Node *next = current->next;
            current->next = previous;
            previous = current;
            current = next;
        }

        previousGroup->next = groupEnd;
        previousGroup = groupStart;
    }

    return dummy.next;
}

void displayList(Node *head) {
    printf("Reversed list: ");

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
    int count, groupSize;

    printf("Enter number of nodes: ");

    if (scanf("%d", &count) != 1 || count < 0) {
        printf("Invalid number of nodes\n");
        return 1;
    }

    Node *head = createList(count);

    printf("Enter group size: ");

    if (scanf("%d", &groupSize) != 1 || groupSize <= 0) {
        printf("Invalid group size\n");
        freeList(head);
        return 1;
    }

    head = reverseGroups(head, groupSize);

    displayList(head);

    freeList(head);

    return 0;
}
