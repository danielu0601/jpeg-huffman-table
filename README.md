# jpeg-huffman-table
Replace the jpeg's huffman table to reduce the file size.

# Environment
Mac 10.12.4 with gcc 5.0.0

# How to compile
Use 'make'.

# How to execute
1. Put all baseline images and codes at the same folder.
2. Type 'make run'.
3. Or  use "./jpgdecoder image_name.jpg".

# Others
1. If the image size is over 2048, please increase the MAX_X
and MAX_Y at header.
2. Not finished yet, `huff.c` can generate a huffman table base on the input.   
   `jpg_huffman_table.c` can now read in a jpeg file and write to bmp.   
3. TODO: combine these two files to count how many times each codewords appears while reading the jpeg,   
   usd `huff.c` to generate new table, and substitute 
