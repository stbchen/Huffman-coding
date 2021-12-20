#include "io.h"
#include "defines.h"
#include "code.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;
uint64_t static code_index = 0;
uint8_t static buffer[BLOCK] = { 0 };
//uint8_t static bytes_top = 0;

// read_bytes()
// Reads in a byte in a buffer and adds to the static variable
// bytes_read to track the # of bytes read. Returns the total
// bytes read then.
// From Eugene's section on 10/28
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int bytes = -1;
    int total = 0;
    // if the total # of bits is not equal to nbytes
    // and bytes is not equal to 0, read in bytes
    while (total != nbytes && bytes != 0) {
        bytes = read(infile, buf, nbytes - total);
        total += bytes;
        bytes_read += bytes;
    }
    return total;
}

// write_bytes()
// Writes in a byte in a buffer and adds to the static variable
// bytes_written to track the # of bytes written. Returns the total
// bytes written then.
// From Eugene's section on 10/28
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int bytes = -1;
    int total = 0;
    // if the total # of bits is not equal to nbytes and
    // bytes is not equal to 0, write out bytes
    while (total != nbytes && bytes != 0) {
        bytes = write(outfile, buf, nbytes - total);
        total += bytes;
        bytes_written += bytes;
    }
    return total;
}

// read_bit()
// Checks if the buffer is empty, if it is, then call read_bytes
// and then see if the bytes read is less than BLOCK, if it is then find
// the bottom. Then add a bit from the buffer and add one to the counter
// of the number of bytes read in the buffer.
// Got inspiration from 10/27 tutoring session from Eric
bool read_bit(int infile, uint8_t *bit) {
    static int bottom = 0;
    static int buf_counter = 0;
    uint64_t reading_bytes = 0;
    if (buf_counter == 0) { // if buffer is empty, call read_bytes
        reading_bytes = read_bytes(infile, buffer, BLOCK);
        if (reading_bytes < BLOCK) { // if read bytes is less than BLOCK, get bottom
            bottom = (reading_bytes * 8) + 1;
        }
    }
    // save in bit from buffer
    *bit = (buffer[buf_counter / 8] >> buf_counter % 8) & 0x1;
    buf_counter++;
    // reset/don't reset buffer
    if (buf_counter == BLOCK * 8) {
        buf_counter = 0;
    }
    return (buf_counter != bottom);
}
// write_code()
// Loops through the bits in Code c, and if the bit is 1, then set the
// bit to one, and if bit is 0, set the bit to 0. Then add to the index,
// and if the index is 0, then call write bytes and use memset to clear the
// buffer.
// Got inspiration from 10/27 tutoring session from Eric
void write_code(int outfile, Code *c) {
    for (uint32_t i = 0; i < code_size(c); i++) { //for the size of code C
        uint8_t curr_bit = c->bits[i / 8] >> (i % 8) & 0x1; // get current bit
        if (curr_bit == 1) { // if current bit is 1, set bit to 1 at buffer
            buffer[code_index / 8] |= (0x1 << (code_index % 8));
        } else { // if current bit is not 1 (0), then clear bit at buffer
            buffer[code_index / 8] &= ~(0x1 << (code_index % 8));
        }
        code_index = (code_index + 1) % (BLOCK * 8);
        //if code index is 0, then call write bytes and use memset to fill buffer with 0's
        if (code_index == 0) {
            write_bytes(outfile, buffer, BLOCK);
            memset(buffer, 0, BLOCK);
        }
    }
}

// flush_codes()
// Retrieves the bytes to write to, then checks if the bytes is 0,
// if it is them set it to the index/8, if not then set it to index/8 + 1
// Got inspiration from 10/27 tutoring session from Eric
void flush_codes(int outfile) {
    uint64_t writing_bytes = code_index % 8; // retrieve the bytes to write to
    if (writing_bytes == 0) { // if writing bytes is 0, then set writing bytes to code index / 8
        writing_bytes = code_index / 8;
    } else { // else set writing bytes to code index / 8 + 1
        writing_bytes = (code_index / 8) + 1;
    }
    write_bytes(outfile, buffer, writing_bytes); // call write_bytes with writing bytes
}
