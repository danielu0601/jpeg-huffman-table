all: jpgdecoder

jpgdecoder: jpgdecoder.c
	gcc -Wall -std=c99 -O2 jpgdecoder.c -o jpgdecoder

.PHONY: run
run: jpgdecoder
	time ./jpgdecoder gig-sn01.jpg
	time ./jpgdecoder gig-sn08.jpg
	time ./jpgdecoder monalisa.jpg
	time ./jpgdecoder teatime.jpg
