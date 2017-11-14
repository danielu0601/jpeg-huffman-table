#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 128
#define INPUT_NUM 16

typedef struct node {
    int value;
    struct node* left;
    struct node* right;
} Node;

typedef struct tmpNode {
    int prob;
    struct node* data;
    struct tmpNode* prev;
    struct tmpNode* next;
} TmpNode;

int prob[2][INPUT_NUM];

void Print( TmpNode* root1 ){
    puts("**** Print list start ****");
    TmpNode* tmp = root1;
    while( tmp != NULL ) {
        printf("val = %3d prob = %3d\n", tmp->data->value, tmp->prob);
        tmp = tmp->next;
    }
    puts("**** Print list end ****");
}

void i2b( int len, int num, char* output ) {
    for( int i = 0; i < len; i++ ) {
        output[i] = '0' + ((num>>(len-i-1))&1);
    }
    output[len] = '\0';
}

void PrintTree( Node *root, int len, int num ) {
    char output[32];
    if( root != NULL ) {
        if( root->value >= 0 ) {
            i2b( len, num, output );
            printf("val = %3d, code = %s\n", root->value, output);
        } else {
            printf("val = %3d,\n", root->value);
        }
        PrintTree( root->left, len+1, num<<1);
        PrintTree( root->right, len+1, (num<<1)+1);
    }
}

int main() {
    srand( time( NULL ) );
    puts("Random Input:");
    for( int i = 0; i < INPUT_NUM; i++ ) {
        prob[0][i] = i; // value
        prob[1][i] = rand()%MAX; // prob
        printf("%2d = %d\n", prob[0][i], prob[1][i] );
    }
    puts("\n\n");

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
   
    puts("Sorted linked list:");
    Print( root1 );
    puts("\n\n");
    for( int i = INPUT_NUM-1; i > 0; i-- ) {
        //Print( root1 );
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
            } else if( tmp->prob > newNode->prob ) {
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
    //Print( root1 );
    
    Node *root = root1->data;

    puts("Generated binary tree(DFS):");
    PrintTree( root, 0, 0 );
    puts("\n\n");

    return 0;
}
