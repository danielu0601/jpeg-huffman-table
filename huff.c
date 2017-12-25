#include "huff.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// longest huffman code length
#define MAX_BIT 16
// max input number, i don't know what happen
// if code length exceed 16 bit.
#define MAX_INPUT 256
/** Below is for debug **/
//#define debug
// 0 for random and 1 for fix input
#define RANDOM 1
// max random size
#define MAX 128
// random input number
#define INPUT_NUM 16

/**
 * Print the linked list
 * root1 : Input tree's root
 */
void Print( TmpNode* root1 ){
    puts("**** Print list start ****");
    TmpNode* tmp = root1;
    while( tmp != NULL ) {
        if( RANDOM == 1 ) {
            printf("val = %c prob = %3d\n", tmp->data->value, tmp->prob);
        } else if ( RANDOM == 0 ) {
            printf("val = %3d prob = %3d\n", tmp->data->value, tmp->prob);
        }
        tmp = tmp->next;
    }
    puts("**** Print list end ****");
    return ;
}

/**
 * integer to binary
 * len : codeword length
 * num : codeword value
 * output : codeword's binary
 */
void i2b( int len, int num, char* output ) {
    for( int i = 0; i < len; i++ ) {
        output[i] = '0' + ((num>>(len-i-1))&1);
    }
    output[len] = '\0';
    return ;
}

/**
 * Print the huffman tree
 * root : tree's root
 * len : codeword length
 * num : codeword value
 */
void PrintTree( Node *root, int len, int num ) {
    char output[32];
    if( root != NULL ) {
        if( root->value >= 0 ) {
            i2b( len, num, output );
            if( RANDOM == 1 && root->value > 0 ) {
                printf("val =   %c, code = %s\n", root->value, output);
            } else if ( RANDOM == 0 ) {
                printf("val = %3d, code = %s\n", root->value, output);
            }
        } else {
            printf("val = %3d,\n", root->value);
        }
        PrintTree( root->left, len+1, num<<1);
        PrintTree( root->right, len+1, (num<<1)+1);
    }
    return ;
}

int recordedIdx[MAX_BIT] = {0};
int recordedVal[MAX_BIT][MAX_INPUT] = {0};
/**
 * Read the huffman tree and record the length and value
 * Above 2 variable is used to record.
 */
void ReadTree( Node* root, int len ) {
    if( root != NULL ) {
        if( root->value >= 0 ) {
            //record root->value and length
            recordedVal[len][ recordedIdx[len] ] = root->value;
            recordedIdx[len]++;
        }
        ReadTree( root->left, len+1 );
        ReadTree( root->right, len+1 );
    }
    return ;
}

/**
 * Main program
 * input the length and probability and output the generated codeword
 * intput_length : intput length
 * prob :
 * OutputData :
 */
void GenerateHuffmanTable( int input_length, int prob[][2], int OutputData[][3] ) {
    // Make a linked list of previous data and sort it.
    TmpNode *root1 = NULL;
    for( int i = 0; i < input_length; i++ ) {
        TmpNode *tmp = root1;
        while( 1 ) {
            //printf("1:");
            if( root1 == NULL ) {
                //printf("2a:");
                root1 = (TmpNode*)malloc( sizeof(TmpNode) );
                root1->prob = prob[i][1];
                root1->data = (Node*)malloc( sizeof(Node) );
                root1->data->value = prob[i][0];
                root1->data->left  = NULL;
                root1->data->right = NULL;
                root1->prev = NULL;
                root1->next = NULL;
                //printf("2b:");
            } else if( tmp->prob > prob[i][1] ) {
                //printf("3a:");
                TmpNode* tmp2 = (TmpNode*)malloc( sizeof(TmpNode) );
                tmp2->prob = prob[i][1];
                tmp2->data = (Node*)malloc( sizeof(Node) );
                tmp2->data->value = prob[i][0];
                tmp2->data->left  = NULL;
                tmp2->data->right = NULL;
                tmp2->prev = tmp->prev;
                tmp2->next = tmp;
                if( tmp->prev == NULL ) {
                    root1 = tmp2;
                } else {
                    tmp->prev->next = tmp2;
                }
                tmp->prev = tmp2;
                //printf("3b:");
            } else if( tmp->next == NULL ) {
                //printf("4a:");
                tmp->next = (TmpNode*)malloc( sizeof(TmpNode) );
                tmp->next->prob = prob[i][1];
                tmp->next->data = (Node*)malloc( sizeof(Node) );
                tmp->next->data->value = prob[i][0];
                tmp->next->data->left  = NULL;
                tmp->next->data->right = NULL;
                tmp->next->prev = tmp;
                tmp->next->next = NULL;
                //printf("4b:");
            } else {
                //printf("5a:");
                tmp = tmp->next;
                //printf("5b:");
                continue;
            }
            //puts("6:");
            break;
        }
    }

    // Print Sorted liked list
    puts("Sorted linked list:");
    Print( root1 );
    puts("\n\n");

    // Generate of huffman tree
    for( int i = input_length-1; i > 0; i-- ) {
        TmpNode *tmp1, *tmp2;
        tmp1 = root1;
        tmp2 = root1->next;
        root1 = root1->next->next;
        if( root1 != NULL )
            root1->prev = NULL;

        TmpNode* newNode = (TmpNode*)malloc( sizeof(TmpNode) );
        newNode->prob = tmp1->prob + tmp2->prob;
        newNode->data = (Node*)malloc( sizeof(Node) );
        newNode->data->value = -i;
        newNode->data->left  = tmp1->data;
        newNode->data->right = tmp2->data;
        newNode->prev = NULL;
        newNode->next = NULL;

        free(tmp1);
        free(tmp2);
        TmpNode *tmp = root1;
        for( int j = 0; j < i; j++ ) {
            //printf("1:");
            if( root1 == NULL ) {
                //printf("s:");
                root1 = newNode;
            } else if( tmp->prob >= newNode->prob ) {
                //printf("2a:");
                newNode->next = tmp;
                newNode->prev = tmp->prev;
                if( tmp->prev == NULL ) {
                    root1 = newNode;
                } else {
                    tmp->prev->next = newNode;
                }
                tmp->prev = newNode;
                //printf("2b:");
            } else if( tmp->next == NULL ) {
                //printf("3a:");
                tmp->next = newNode;
                newNode->prev = tmp;
                //printf("3b:");
            } else {
                //printf("4a:");
                tmp = tmp->next;
                //printf("4b:");
                continue;
            }
            //puts("5:");
            break;
        }
    } 
    Node *root = root1->data;

    // Print generated huffman tree
    puts("Generated binary tree(DFS):");
    PrintTree( root, 0, 0 );
    puts("\n\n");

    /**
      * For JPEG huffman table, we need to sort it by codeword length
      * and reproduce a huffman tree again.  For example, 00 011 010 
      * 10 110 111 needs to be modified to 00 01 100 101 110 111.    
      * Which is due to the decoder default the codeword is appeared 
      * in this order.
      */
    ReadTree( root, 0 );
    int num = 0;
    char output[32];
    for( int i = 0; i < 16; i++ ) {
        for( int j = 0; j < recordedIdx[i]; j++ ) {
            i2b( i, num, output );
            printf("val = %c, code = %s\n", recordedVal[i][j], output);
            OutputData[i][0] = i;
            OutputData[i][1] = num;
            OutputData[i][2] = recordedVal[i][j];
            num++;
        }
        num <<= 1;
    }

    return ;
}

#ifdef debug
int main() {
    int input_length = INPUT_NUM;
    // Input data
    // [i][0] = value, [i][1] = appear times
    int prob[MAX_INPUT][2];
    // Output data
    // 0 = lenght, 1 = CodeWord, 2 = Value
    int OutputData[MAX_INPUT][3] = {0};

    // Generate the Input or use the default input
    if( RANDOM == 0 ) {
        puts("Random Input:");
        srand( time( NULL ) );
        for( int i = 0; i < input_length; i++ ) {
            prob[i][0] = i; // value
            prob[i][1] = rand()%MAX; // prob
            printf("%2d = %d\n", prob[i][0], prob[i][1] );
        }
    } else if ( RANDOM == 1 ) {
        puts("Fixed Input:");
        input_length = 17;
        prob[ 0][0] = ' ', prob[ 0][1] = 7,
        prob[ 1][0] = 'a', prob[ 1][1] = 5,
        prob[ 2][0] = 'e', prob[ 2][1] = 4,
        prob[ 3][0] = 'i', prob[ 3][1] = 3,
        prob[ 4][0] = 's', prob[ 4][1] = 3,
        prob[ 5][0] = 'h', prob[ 5][1] = 2,
        prob[ 6][0] = 'p', prob[ 6][1] = 2,
        prob[ 7][0] = 'r', prob[ 7][1] = 2,
        prob[ 8][0] = 'C', prob[ 8][1] = 1,
        prob[ 9][0] = 'T', prob[ 9][1] = 1,
        prob[10][0] = 'c', prob[10][1] = 1,
        prob[11][0] = 'f', prob[11][1] = 1,
        prob[12][0] = 'l', prob[12][1] = 1,
        prob[13][0] = 'm', prob[13][1] = 1,
        prob[14][0] = 'n', prob[14][1] = 1,
        prob[15][0] = 'o', prob[15][1] = 1,
        prob[16][0] = 'w', prob[16][1] = 1;
        for( int i = 0; i < input_length; i++ ) {
            printf("%c = %d\n", prob[i][0], prob[i][1] );
        }
    }
    puts("\n\n");

    GenerateHuffmanTable( input_length, prob, OutputData );

    return 0;
}
#endif
