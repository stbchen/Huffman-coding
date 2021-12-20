#include "huffman.h"
#include "pq.h"
#include "node.h"
#include "io.h"
#include "stack.h"

#include <stdio.h>
#include <inttypes.h>

// historgram is 1D array
// is 255 length, each index is ASCII value
// value is frequency
// Attended Eric's tutoring session on 11/3
Node *build_tree(uint64_t hist[static ALPHABET]) {
    // create a priority queue and a root node
    PriorityQueue *p = pq_create(ALPHABET);
    Node *root;
    for (int i = 0; i < ALPHABET; i++) { // loop through ALPHABET
        if (hist[i] > 0) { // for every non-zero value, create a node and enqueue it
            Node *n = node_create(i, hist[i]);
            enqueue(p, n);
        }
    }
    // while the size of priority queue is > 1,
    // dequeue the left an right node and enqueue
    // the parent (join the left and right)
    while (pq_size(p) > 1) {
        Node *left;
        Node *right;
        dequeue(p, &left);
        dequeue(p, &right);
        Node *parent = node_join(left, right);
        enqueue(p, parent);
    }
    // dequeue the root from priority queue, delete the priority queue, return root
    dequeue(p, &root);
    pq_delete(&p);
    return root;
}

// build_codes()
// Goes through table and if node is a leaf node, push the node's symbol and reset node
// to root, and if there's a left node, push a 0 and call build_codes() with left node.
// Do the same for the right node but push a 1 instead.
static Code c;
void build_codes(Node *root, Code table[static ALPHABET]) {
    uint8_t popped = 0;
    if (root != NULL) { // if root exists
        if (root->left == NULL && root->right == NULL) { // if node is leaf node
            table[root->symbol] = c; // add code to code table
        } else {
            // if left node exists, push a 0 onto code,
            // call build codes then pop to variable
            if (root->left != NULL) {
                code_push_bit(&c, 0);
                build_codes(root->left, table);
                code_pop_bit(&c, &popped);
            }
            // if left node exists, push a 1 onto code,
            // call build codes then pop to variable
            if (root->right != NULL) {
                code_push_bit(&c, 1);
                build_codes(root->right, table);
                code_pop_bit(&c, &popped);
            }
        }
    }
}

// dump_tree()
// Dumps the tree by performing a post order traversal and going through and writing an
// 'L' and the symbol if its a leaf node, and 'I' if it's an interior node
// Went to Eugene's section on 11/4 and he went over post order traversal
void dump_tree(int outfile, Node *root) {
    uint8_t *l = (uint8_t *) "L";
    uint8_t *i = (uint8_t *) "I";
    if (root != NULL) {
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);
        if (root->left == NULL && root->right == NULL) { //if node is leaf node
            write_bytes(outfile, l, 1);
            write_bytes(outfile, &root->symbol, 1);
        } else {
            write_bytes(outfile, i, 1);
        }
    }
}

// rebuild_tree()
// Rebuilds the tree by going through a post order traversal and if the
// value in tree[] is an 'L', push the next value in tree[] to the stack,
// and if it is an 'I', then pop two nodes, join them and make a parent
// node and push that to the stack. Once done, pop the root node and delete
// the stack and return the root.
// Went to Eugene's section and also went to Eric's session too
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Stack *s = stack_create(nbytes);
    Node *left;
    Node *right;
    Node *root;
    for (uint32_t i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') {
            Node *leaf = node_create(tree[i + 1], 0);
            stack_push(s, leaf);
            // If the next value is an 'L' or 'I', it is the symbol
            // and not a leaf/interior
            if (tree[i + 1] == 'I' || tree[i + 1] == 'L') {
                tree[i + 1] = 'X'; // Set to 'X' (non I or L value) instead of 'I' or 'L'
            }
        } else if (tree[i] == 'I') {
            // post order traversal
            stack_pop(s, &right);
            stack_pop(s, &left);
            Node *parent = node_join(left, right);
            stack_push(s, parent);
        }
    }
    stack_pop(s, &root);
    stack_delete(&s);
    return root;
}

// delete_tree()
// Deletes the tree with a post order traversal
void delete_tree(Node **root) {
    if (*root != NULL) { //if node is not NULL
        delete_tree(&(*root)->left);
        delete_tree(&(*root)->right);
        node_delete(root);
    }
}
