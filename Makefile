all: jpg_huffman_table

jpg_huffman_table: jpg_huffman_table.o huff.o
	gcc -Wall -std=c99 -O2 jpg_huffman_table.o huff.o -o jpg_huffman_table
jpg_huffman_table.o: jpg_huffman_table.c
	gcc -Wall -std=c99 -O2 -c jpg_huffman_table.c
huff.o: huff.c
	gcc -Wall -std=c99 -O2 -c huff.c

.PHONY: clean run
clean:
	rm *.o
run: jpgdecoder
	time ./jpgdecoder gig-sn01.jpg
	time ./jpgdecoder gig-sn08.jpg
	time ./jpgdecoder monalisa.jpg
	time ./jpgdecoder teatime.jpg
