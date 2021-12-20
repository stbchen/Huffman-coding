#include "header.h"
#include "node.h"
#include "defines.h"
#include "code.h"
#include "io.h"
#include "huffman.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>

#define OPTIONS "hi:o:v"

// help()
// Parameters: void
// Returns: void
// help() prints out the program usage and help
void help() {
    fprintf(stderr, "SYNOPSIS\n\
  A Huffman encoder.\n\
  Compresses a file using the Huffman coding algorithm.\n\
\n\
USAGE\n\
  ./encode [-h] [-i infile] [-o outfile]\n\
\n\
OPTIONS\n\
  -h             Program usage and help.\n\
  -v             Print compression statistics.\n\
  -i infile      Input file to compress.\n\
  -o outfile     Output of compressed data.)\n");
}

// main()
// Parameters: argc, **argv
// int argc: Used for optarg in finding user input from command line
// char **argv: Used for optarg in finding user input from command line
// Returns: 0 for no error, 1 for error
// main() reads in command line options, then creates a histogram from the
// infile, then builds a priority queue and a Huffman tree from the histogram.
// A code table is built from the Huffman tree and the header information is created,
// and the tree is dumped and the codes are written out. Verbose printing is printed if
// enabled and all dynamically allocated memory is freed.
// Went to Eric's tutoring session on 11/3, and also Eugene's section on 11/4// main()
int main(int argc, char **argv) {
    struct stat statbuf;
    int opt = 0;
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;
    uint64_t histogram[ALPHABET] = { 0 };
    bool stats = false;
    bool out = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help(); return 0;
        case 'i':
            infile = open(optarg, O_RDONLY);
            if (infile == -1) {
                fprintf(stderr, "Failed to open infile.\n");
                return 1;
            }
            break;
        case 'o':
            out = true;
            outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC);
            if (outfile == -1) {
                fprintf(stderr, "Failed to write outfile.\n");
                return 1;
            }
            break;
        case 'v': stats = true; break;
        default: return 1;
        }
    }

    //creating histogram
    uint8_t buffer[BLOCK] = { 0 };
    uint32_t num_sym = 2;
    Code table[ALPHABET] = { 0 };
    // have at least 2 values
    histogram[0]++;
    int bytes_reading = 0;
    histogram[ALPHABET - 1]++;
    while ((bytes_reading = read_bytes(infile, buffer, BLOCK)) > 0) {
        for (int i = 0; i < bytes_reading; i++) {
            if (histogram[buffer[i]] == 0) {
                num_sym++;
            }
            histogram[buffer[i]]++;
        }
    }

    //builds Huffman tree and the code table
    Node *root = build_tree(histogram);
    build_codes(root, table);
    //    for (uint32_t i = 0; i < ALPHABET; i++) {
    //        printf("index: %" PRId32 "\n", i);
    //        code_print(&table[i]);
    //        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    //    }

    //header initialization
    fstat(infile, &statbuf);
    fchmod(outfile, statbuf.st_mode);
    Header h;
    h.magic = MAGIC;
    h.file_size = statbuf.st_size;
    h.tree_size = 3 * num_sym - 1;
    h.permissions = statbuf.st_mode;
    write_bytes(outfile, (uint8_t *) &h, sizeof(Header));

    //dump tree
    dump_tree(outfile, root);
    //write out codes
    lseek(infile, 0, SEEK_SET);
    while ((bytes_reading = read_bytes(infile, buffer, BLOCK)) != 0) {
        for (int i = 0; i < bytes_reading; i++) {
            write_code(outfile, &table[buffer[i]]);
        }
    }

    // flushes codes
    flush_codes(outfile);

    // checks if verbose printing is enabled
    if (stats == true) {
        float diff = 100 * (1 - ((float) bytes_written / h.file_size));
        if (out == false) {
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "Uncompressed file size: %" PRId64 " bytes\n", h.file_size);
        fprintf(stderr, "Compressed file size: %" PRId64 " bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %0.2f %%\n", diff);
    }
    delete_tree(&root);
    close(infile);
    close(outfile);
    return 0;
}
