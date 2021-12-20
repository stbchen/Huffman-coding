#include "header.h"
#include "pq.h"
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

// OPTIONS are the command line options for decode
#define OPTIONS "hi:o:v"

// help()
// Parameters: void
// Returns: void
// help() prints out the program usage and help
void help(void) {
    printf("SYNOPSIS\n\
  A Huffman decoder.\n\
  Decompresses a file using the Huffman coding algorithm.\n\
\n\
USAGE\n\
  ./decode [-h] [-i infile] [-o outfile]\n\
\n\
OPTIONS\n\
  -h             Program usage and help.\n\
  -v             Print compression statistics.\n\
  -i infile      Input file to decompress.\n\
  -o outfile     Output of decompressed data.\n");
}

// main()
// Parameters: argc, **argv
// int argc: Used for optarg in finding user input from command line
// char **argv: Used for optarg in finding user input from command line
// Returns: 0 for no error, 1 for error
// main() runs the command line options, then reads in header information,
// and after rebuilds the tree from infile, and using the rebuilt tree the
// tree will be traversed to print out the correct symbols. Verbose printing
// will print if enabled in command line options, and all dynamically
// allocated memory is freed
int main(int argc, char **argv) {
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;
    int opt = 0;
    bool stats = false;
    bool out = false;
    // gets all command line options
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

    // get header info
    Header h;
    read_bytes(infile, (uint8_t *) &h, sizeof(h));
    // checks magic number
    if (h.magic != MAGIC) {
        fprintf(stderr, "Incorrect magic number.\n");
        close(infile);
        close(outfile);
        return 1;
    }

    // rebuilds the tree
    uint8_t tree_dump[h.tree_size];
    fchmod(outfile, h.permissions);
    read_bytes(infile, tree_dump, h.tree_size);
    Node *root = rebuild_tree(h.tree_size, tree_dump);
    uint8_t bit;
    Node *travel_node = root;
    uint64_t i = 0;
    // go through and print out the symbols to output and stop looping
    // until the output size is the same as the original file size's
    while (i < h.file_size) {
        // if travelled node is a leaf node then write the symbol
        if (travel_node->left == NULL && travel_node->right == NULL) {
            write_bytes(outfile, &travel_node->symbol, 1);
            i++;
            travel_node = root; // start back at root
        }
        read_bit(infile, &bit);
        // if bit thats read is 0, go left down tree, if 1 then go right down
        if (bit == 0) {
            travel_node = travel_node->left;
        } else if (bit == 1) {
            travel_node = travel_node->right;
        }
    }
    // check if verbose printing is on
    if (stats == true) {
        float diff = 100 * (1 - ((float) bytes_read / h.file_size));
        if (out == false) {
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "Compressed file size: %" PRId64 " bytes\n", bytes_read);
        fprintf(stderr, "Decompressed file size: %" PRId64 " bytes\n", h.file_size);
        fprintf(stderr, "Space saving: %0.2f%%\n", diff);
    }
    // free and close everything
    delete_tree(&root);
    close(infile);
    close(outfile);
    return 0;
}
