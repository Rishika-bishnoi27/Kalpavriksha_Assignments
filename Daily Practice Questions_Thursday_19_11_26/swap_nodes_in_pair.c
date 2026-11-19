#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node* createNode(int data) {
    Node* newNode = malloc(sizeof(Node));

    if (newNode == NULL) {
        return NULL;
    }

    newNode->data = data;
    newNode->next = NULL;

    return newNode;
}

Node* createList(int count) {
    Node* head = NULL;
    Node* tail = NULL;

    for (int index = 0; index < count; index++) {
        int data;
        scanf("%d", &data);

        Node* newNode = createNode(data);

        if (newNode == NULL) {
            return head;
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

Node* swapPairs(Node* head) {
    Node dummy;
    dummy.next = head;

    Node* previousNode = &dummy;

    while (previousNode->next != NULL &&
           previousNode->next->next != NULL) {

        Node* firstNode = previousNode->next;
        Node* secondNode = firstNode->next;

        firstNode->next = secondNode->next;
        secondNode->next = firstNode;
        previousNode->next = secondNode;

        previousNode = firstNode;
    }

    return dummy.next;
}

void displayList(Node* head) {
    Node* currentNode = head;

    while (currentNode != NULL) {
        printf("%d", currentNode->data);

        if (currentNode->next != NULL) {
            printf(" -> ");
        }

        currentNode = currentNode->next;
    }

    printf("\n");
}

void freeList(Node* head) {
    Node* currentNode = head;

    while (currentNode != NULL) {
        Node* nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }
}

int main() {
    int count;

    printf("Enter number of nodes: ");
    scanf("%d", &count);

    if (count < 0) {
        printf("Invalid number of nodes\n");
        return 1;
    }

    printf("Enter list elements: ");
    Node* head = createList(count);

    printf("Original list: ");
    displayList(head);

    head = swapPairs(head);

    printf("After swapping pairs: ");
    displayList(head);

    freeList(head);

    return 0;
}