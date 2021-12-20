#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

// node_create() is a constructor for Node
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->symbol = symbol;
        n->frequency = frequency;
        n->left = NULL;
        n->right = NULL;
    }
    return n;
}

// node_delete() is a destructor for Node
void node_delete(Node **n) {
    if (*n) {
        if ((*n)->right) {
            node_delete(&(*n)->right);
        }
        if ((*n)->left) {
            node_delete(&(*n)->left);
        }
        free(*n);
        *n = NULL;
    }
    return;
}

// node_join() joins two nodes into a parent node
Node *node_join(Node *left, Node *right) {
    Node *parent = node_create('$', left->frequency + right->frequency);
    parent->left = left;
    parent->right = right;
    return parent;
}

// node_print() is a debugger function that prints Node
void node_print(Node *n) {
    printf("Frequency: %" PRIu64 "\n", n->frequency);
    printf("Symbol %c \n", n->symbol);
}
