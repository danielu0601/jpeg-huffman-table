#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 128
#define INPUT_NUM 17
#define RANDOM 1

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

// Input data
int prob[2][INPUT_NUM];

// Output data
// 0 = lenght, 1 = CodeWord, 2 = Value
int OutputData[256][3] = {0};

// Print the linked list
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

// integer to binary
void i2b( int len, int num, char* output ) {
    for( int i = 0; i < len; i++ ) {
        output[i] = '0' + ((num>>(len-i-1))&1);
    }
    output[len] = '\0';
    return ;
}

// Print the huffman tree
// len : codeword length
// num : codeword value
// 010 -> len = 3, num = 0b010
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

int recordedIdx[16] = {0};
int recordedVal[16][256] = {0};
// Read the huffman tree and record the length and value
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

int main() {
    // Generate the Input or use the default input
    srand( time( NULL ) );
    if( RANDOM == 0 ) {
        puts("Random Input:");
        for( int i = 0; i < INPUT_NUM; i++ ) {
            prob[0][i] = i; // value
            prob[1][i] = rand()%MAX; // prob
            printf("%2d = %d\n", prob[0][i], prob[1][i] );
        }
    } else if ( RANDOM == 1 ) {
        puts("Fixed Input:");
        prob[0][ 0] = 0b00100000; prob[1][ 0] = 7;
        prob[0][ 1] = 0b01100001; prob[1][ 1] = 5;
        prob[0][ 2] = 0b01100101; prob[1][ 2] = 4;
        prob[0][ 3] = 0b01101001; prob[1][ 3] = 3;
        prob[0][ 4] = 0b01110011; prob[1][ 4] = 3;
        prob[0][ 5] = 0b01101000; prob[1][ 5] = 2;
        prob[0][ 6] = 0b01110000; prob[1][ 6] = 2;
        prob[0][ 7] = 0b01110010; prob[1][ 7] = 2;
        prob[0][ 8] = 0b01000011; prob[1][ 8] = 1;
        prob[0][ 9] = 0b01010100; prob[1][ 9] = 1;
        prob[0][10] = 0b01100011; prob[1][10] = 1;
        prob[0][11] = 0b01100110; prob[1][11] = 1;
        prob[0][12] = 0b01101100; prob[1][12] = 1;
        prob[0][13] = 0b01101101; prob[1][13] = 1;
        prob[0][14] = 0b01101110; prob[1][14] = 1;
        prob[0][15] = 0b01101111; prob[1][15] = 1;
        prob[0][16] = 0b01110111; prob[1][16] = 1;
        for( int i = 0; i < INPUT_NUM; i++ ) {
            printf("%c = %d\n", prob[0][i], prob[1][i] );
        }
    }
    puts("\n\n");

    // Make a linked list of previous data and sort it.
    TmpNode *root1 = NULL;
    for( int i = 0; i < INPUT_NUM; i++ ) {
        TmpNode *tmp = root1;
        while( 1 ) {
            //printf("1:");
            if( root1 == NULL ) {
                //printf("2a:");
                root1 = (TmpNode*)malloc( sizeof(TmpNode) );
                root1->prob = prob[1][i];
                root1->data = (Node*)malloc( sizeof(Node) );
                root1->data->value = prob[0][i];
                root1->data->left  = NULL;
                root1->data->right = NULL;
                root1->prev = NULL;
                root1->next = NULL;
                //printf("2b:");
            } else if( tmp->prob > prob[1][i] ) {
                //printf("3a:");
                TmpNode* tmp2 = (TmpNode*)malloc( sizeof(TmpNode) );
                tmp2->prob = prob[1][i];
                tmp2->data = (Node*)malloc( sizeof(Node) );
                tmp2->data->value = prob[0][i];
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
                tmp->next->prob = prob[1][i];
                tmp->next->data = (Node*)malloc( sizeof(Node) );
                tmp->next->data->value = prob[0][i];
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
    for( int i = INPUT_NUM-1; i > 0; i-- ) {
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

    return 0;
}

/*
int main() {
    return 0;
}
*/
