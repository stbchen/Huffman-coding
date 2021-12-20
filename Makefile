CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra

all: encode decode

encode: encode.o code.o node.o pq.o io.o stack.o huffman.o
	$(CC) -o encode encode.o code.o node.o pq.o io.o stack.o huffman.o

encode.o: encode.c 
	$(CC) $(CFLAGS) -c encode.c

decode: decode.o code.o node.o pq.o io.o stack.o huffman.o 
	$(CC) -o decode decode.o code.o node.o pq.o io.o stack.o huffman.o

decode.o: decode.c 
	$(CC) $(CFLAGS) -c decode.c

code.o: code.c
	$(CC) $(CFLAGS) -c code.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

huffman.o: huffman.c
	$(CC) $(CFLAGS) -c huffman.c

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

debug: CFLAGS += -g

debug: all

clean:
	rm -f encode encode.o code.o huffman.o
	rm -f decode decode.o io.o stack.o pq.o node.o

format:
	clang-format -i -style=file *.[ch]
