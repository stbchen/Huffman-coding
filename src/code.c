#include "code.h"
#include "defines.h"
#include <inttypes.h>
#include <stdio.h>

// Code initialize from Christian's section
Code code_init(void) {
    Code c = { .top = 0, .bits = { 0 } };
    return c;
}

// code_size()
// Parameters: *c
// Code *c: Code c being used
// Returns: uint32_t, the "index"/top value in code
uint32_t code_size(Code *c) {
    return c->top;
}

// code_empty()
// Parameters: *c
// Code *c: Code c being used
// Returns: bool, checks to see if c->top is empty (0)
bool code_empty(Code *c) {
    return c->top == 0;
}

// code_full()
// Parameters: *c
// Code *c: Code c being used
// Returns: bool, checks to see if c->top is full (1)
bool code_full(Code *c) {
    return c->top == ALPHABET;
}

// code_set_bit()
// Parameters: *c, i
// Code *c: Code c being used
// uint32_t i: the index to set bit at
// Returns: bool, checks to see if bit is successfully set
// code_set_bit() checks to see if i is within the range of ALPHABET, then sets the
// bit using bitwise operations, and returns T/F if the bit is set or not
bool code_set_bit(Code *c, uint32_t i) {
    if (i < ALPHABET) {
        c->bits[i / 8] |= 0x1 << i % 8; // From Prof. Long's code comment repo
        return true;
    } else {
        return false;
    }
}

// code_clr_bit()
// Parameters: *c, i
// Code *c: Code c being used
// uint32_t i: the index to clear bit at
// Returns: bool, checks to see if bit is successfully cleared
// code_clr_bit() checks to see if i is within the range of ALPHABET, then clears the
// bit using bitwise operations, and returns T/F if the bit is set or not
bool code_clr_bit(Code *c, uint32_t i) {
    if (i < ALPHABET) {
        c->bits[i / 8] &= ~(0x1 << i % 8); // From Prof. Long's code comment repo
        return true;
    } else {
        return false;
    }
}

// code_get_bit()
// Parameters: *c, i
// Code *c: Code c being used
// uint32_t i: the index to get bit at
// Returns: bool, returns a T/F (0/1) depending on bit retrieved
// code_get_bit() uses bitwise operations to get the bit at index i and returns it
bool code_get_bit(Code *c, uint32_t i) {
    return (c->bits[i / 8] >> (i % 8)) & 0x1; // From Prof. Long's code comment repo
}

// code_push_bit()
// Parameters: *c, bit
// Code *c: Code c being used
// uint8_t bit: bit being pushed
// ReturnsL bool, T/F if bit was successfully pushed or not
// code_push_bit() checks to see if Code c is full, if not it will push/pop bit
// depending on the value of bit
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    if (bit == 1) {
        code_set_bit(c, c->top);
        c->top++;
        return true;
    } else if (bit != 1) {
        code_clr_bit(c, c->top);
        c->top++;
        return true;
    } else {
        return false;
    }
}

// code_pop_bit()
// Parameters: *c, *bit
// Code *c: Code c being used
// uint8_t *bit: pointer bit being saved to
// ReturnsL bool, T/F if bit was successfully popped or not
// code_pop_bit() checks to see if Code c is empty, if not it will call get bit
// and save bit to *bit and then clear that bit
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    c->top--;
    *bit = code_get_bit(c, c->top);
    code_clr_bit(c, c->top);
    return true;
}

// Debugger function that prints code
void code_print(Code *c) {
    for (uint32_t i = 0; i < c->top; i++) {
        printf("Position: %" PRIu32 " Bit: %" PRIu32 "\n", i, code_get_bit(c, i));
    }
}
