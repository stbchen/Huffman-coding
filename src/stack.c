#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "node.h"

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

// stack_create() is the constructor for Stack
Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (Node **) calloc(capacity, sizeof(Node *));
        if (!s->items) {
            free(s);
            s = NULL;
        }
    }
    return s;
}

//stack_delete is the destructor for Stack
void stack_delete(Stack **s) {
    //    for (uint32_t i = 0; i < (*s)->capacity; i++) {
    //        free((*s)->items[i]);
    //    }
    if (*s && (*s)->items) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

// Returns if stack is empty or not
bool stack_empty(Stack *s) {
    return s->top == 0;
}

// Returns if stack is full or not
bool stack_full(Stack *s) {
    return s->top == s->capacity;
}

// Returns the size of the stack
uint32_t stack_size(Stack *s) {
    return s->top;
}

// Pushes Node n onto the Stack s
bool stack_push(Stack *s, Node *n) {
    if (stack_full(s)) {
        return false;
    } else {
        s->items[s->top] = n;
        s->top++;
        return true;
    }
}

// Pops Node n from the Stack s
bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s)) {
        return false;
    } else {
        s->top--;
        *n = s->items[s->top];
        return true;
    }
}
/*
void stack_print(Stack *s) {
}*/
