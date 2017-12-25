#ifndef HUFF_H_
#define HUFF_H_
// longest huffman code length
#define MAX_BIT 16
// max input number, i don't know what happen
// if code length exceed 16 bit.
#define MAX_INPUT 256

// Node of huffman tree
typedef struct node {
    int value;
    struct node* left;
    struct node* right;
} Node;

// Node of linked list
typedef struct tmpNode {
    int prob;
    struct node* data;
    struct tmpNode* prev;
    struct tmpNode* next;
} TmpNode;

// Node of Recorded values
typedef struct recNode {
    int value;
    struct recNode* next;
} RecNode;

// Print the linked list
void Print( TmpNode* root1 );

// integer to binary
void i2b( int len, int num, char* output );

// Print the huffman tree
// len : codeword length
// num : codeword value
// 010 -> len = 3, num = 0b010
void PrintTree( Node *root, int len, int num );

// Read the huffman tree and record the length and value
void ReadTree( Node* root, int len );

/**
 * Main program, generate huffman table
 * input_length : input's length
 * prob : input value and probablity
 * OutputData : output length, codeword and value
 */
void GenerateHuffmanTable( int input_length, int prob[][2], int OutputData[][3] );

#endif
