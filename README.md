# Huffman Coding

This program will encode and decode files with encode.c and decode.c respectively. This program will perform encoding/decoding with Huffman coding. Files can be inputted and outputted from encode using the command line options -i or -o, as well as print compression statistics using the command line option -v. The basic structure of encoding a file is: creating a histogram of symbols and frequencies, creating a Huffman tree using a priority queue and a min heap, and dumping the tree and building the codes. Decoding a file is similar to the reverse order encoding a file, as the data is decoded by rebuilding the tree and traversing the tree to get the data decoded.

This is a school project so no students of CSE 13s should be looking at this source code.

## Building

Build the program with:

```
$ clang -Wall -Wextra -Werror -Wpedantic -c encode.c
$ clang -Wall -Wextra -Werror -Wpedantic -c decode.c
$ clang -o encode encode.o code.o node.o pq.o io.o stack.o huffman.o
$ clang -o decode decode.o code.o node.o pq.o io.o stack.o huffman.o
```
or

```
$ make all
```

## Running

Run the program with (including command line options):

```
$ ./encode [-h] [-v] [-i infile] [-o outfile]
$ ./decode [-h] [-v] [-i infile] [-o outfile]
```



