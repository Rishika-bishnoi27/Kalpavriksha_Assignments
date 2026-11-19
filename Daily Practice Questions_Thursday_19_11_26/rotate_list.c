#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
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

Node* rotateRight(Node* head, int rotationCount) {
    if (head == NULL || head->next == NULL || rotationCount == 0) {
        return head;
    }

    int length = 1;
    Node* lastNode = head;

    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
        length++;
    }

    rotationCount = rotationCount % length;

    if (rotationCount == 0) {
        return head;
    }

    lastNode->next = head;

    int steps = length - rotationCount;
    Node* newLastNode = head;

    for (int index = 1; index < steps; index++) {
        newLastNode = newLastNode->next;
    }

    Node* newHead = newLastNode->next;
    newLastNode->next = NULL;

    return newHead;
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
    int rotationCount;

    printf("Enter number of nodes: ");
    scanf("%d", &count);

    if (count < 0) {
        printf("Invalid number of nodes\n");
        return 1;
    }

    printf("Enter list elements: ");
    Node* head = createList(count);

    printf("Enter rotation count: ");
    scanf("%d", &rotationCount);

    printf("Original list: ");
    displayList(head);

    head = rotateRight(head, rotationCount);

    printf("Rotated list: ");
    displayList(head);

    freeList(head);

    return 0;
}