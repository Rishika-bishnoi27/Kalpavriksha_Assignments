
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    Node **nodes;
    int size;
} MinHeap;

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

void swapNodes(Node **first, Node **second) {
    Node *value = *first;
    *first = *second;
    *second = value;
}

void insertHeap(MinHeap *heap, Node *node) {
    int position = heap->size++;

    heap->nodes[position] = node;

    while (position > 0) {
        int parent = (position - 1) / 2;

        if (heap->nodes[parent]->data <= heap->nodes[position]->data) {
            break;
        }

        swapNodes(&heap->nodes[parent], &heap->nodes[position]);
        position = parent;
    }
}

Node *removeMinimum(MinHeap *heap) {
    Node *minimum = heap->nodes[0];

    heap->nodes[0] = heap->nodes[--heap->size];

    int position = 0;

    while (1) {
        int smallest = position;
        int left = 2 * position + 1;
        int right = 2 * position + 2;

        if (left < heap->size &&
            heap->nodes[left]->data < heap->nodes[smallest]->data) {
            smallest = left;
        }

        if (right < heap->size &&
            heap->nodes[right]->data < heap->nodes[smallest]->data) {
            smallest = right;
        }

        if (smallest == position) {
            break;
        }

        swapNodes(&heap->nodes[position], &heap->nodes[smallest]);
        position = smallest;
    }

    return minimum;
}

Node *mergeLists(Node **lists, int listCount) {
    MinHeap heap;

    heap.nodes = malloc(listCount * sizeof(Node *));
    heap.size = 0;

    if (heap.nodes == NULL && listCount > 0) {
        return NULL;
    }

    for (int index = 0; index < listCount; index++) {
        if (lists[index] != NULL) {
            insertHeap(&heap, lists[index]);
        }
    }

    Node *head = NULL;
    Node *tail = NULL;

    while (heap.size > 0) {
        Node *current = removeMinimum(&heap);

        if (head == NULL) {
            head = current;
            tail = current;
        } else {
            tail->next = current;
            tail = current;
        }

        if (current->next != NULL) {
            insertHeap(&heap, current->next);
        }
    }

    free(heap.nodes);

    return head;
}

void displayList(Node *head) {
    printf("Merged list: ");

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
    int listCount;

    printf("Enter number of linked lists: ");

    if (scanf("%d", &listCount) != 1 || listCount < 0) {
        printf("Invalid input\n");
        return 1;
    }

    Node **lists = malloc((listCount > 0 ? listCount : 1) * sizeof(Node *));

    if (lists == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int index = 0; index < listCount; index++) {
        int nodeCount;

        printf("Enter number of nodes in list %d: ", index + 1);

        if (scanf("%d", &nodeCount) != 1 || nodeCount < 0) {
            printf("Invalid input\n");
            for (int previous = 0; previous < index; previous++) {
                freeList(lists[previous]);
            }
            free(lists);
            return 1;
        }

        printf("Enter sorted elements: ");
        lists[index] = createList(nodeCount);
    }

    Node *merged = mergeLists(lists, listCount);

    displayList(merged);

    freeList(merged);
    free(lists);

    return 0;
}
