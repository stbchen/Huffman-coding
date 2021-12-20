#include "pq.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

struct PriorityQueue {
    uint32_t capacity;
    uint32_t top;
    uint32_t size;
    Node **items;
};

// pq_create() is a constructor for PriorityQueue
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->capacity = capacity;
        q->items = (Node **) calloc(capacity, sizeof(Node *));
        q->top = 0;
        q->size = 0;
        if (!q->items) {
            free(q);
            q = NULL;
        }
    }
    return q;
}

// pq_delete is a destructor for PriorityQueue
void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->items) {
        free((*q)->items);
        free(*q);
        *q = NULL;
    }
    return;
}

//pq_empty checks to see if the PriorityQueue is empty or not
bool pq_empty(PriorityQueue *q) {
    return q->size == 0;
}

//pq_full checks to see if the PriorityQueue is full or not
bool pq_full(PriorityQueue *q) {
    return q->size == q->capacity;
}
//pq_full checks the size of the PriorityQueue
uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

//swap() is a helper function that swaps Nodes
void static swap(Node **x, Node **y) {
    Node *temp = *x;
    *x = *y;
    *y = temp;
}

// min_child() is a helper function that finds the min child
uint32_t static min_child(Node **A, uint32_t first, uint32_t last) {
    uint32_t left = 2 * first;
    uint32_t right = left + 1;
    if (right <= last && A[right - 1]->frequency < A[left - 1]->frequency) {
        return right;
    }
    return left;
}

// fix_heap() is a helper function that fixes the heap
void static fix_heap(Node **A, uint32_t first, uint32_t last) {
    bool found = false;
    uint32_t mother = first;
    uint32_t great = min_child(A, mother, last);
    while (mother <= last / 2 && !found) {
        if (A[mother - 1]->frequency > A[great - 1]->frequency) {
            swap(&A[mother - 1], &A[great - 1]);
            mother = great;
            great = min_child(A, mother, last);
        } else {
            found = true;
        }
    }
}

// enqueue() enqueues a Node onto the PriorityQueue, and fixes the heap
bool enqueue(PriorityQueue *q, Node *n) {
    if (pq_full(q)) {
        return false;
    }
    q->items[q->size] = n;
    uint32_t num = q->size;
    while (q->items[num / 2]->frequency > q->items[num]->frequency) {
        Node *temp = q->items[num / 2];
        q->items[num / 2] = q->items[num];
        q->items[num] = temp;
        num = num / 2;
    }
    q->size++;
    return true;
}

// dequeue() removes the "highest" priority item from the queue, and fixes heap
bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q)) {
        return false;
    }
    *n = q->items[0];
    q->items[0] = q->items[q->size - 1];
    q->size--;
    fix_heap(q->items, 1, q->size);
    return true;
}

// pq_print() is a debugger function that prints out the queue
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size; i++) {
        printf("Number: %" PRIu32 " ", i);
        node_print(q->items[i]);
    }
}
