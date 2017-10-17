#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358

int main() {
    double COSINE[8][8];
    for( int i = 0; i < 8; i++ ) {
        for( int j = 0; j < 8; j++ ) {
            COSINE[i][j] = cos((2*i+1)*j*PI/16);
            if( j == 0 )
                COSINE[i][j] *= 0.707106781186548;
        }
    }
    for( int i = 0; i < 8; i++ ) {
        for( int j = 0; j < 8; j++ ) {
            printf("%llf ", COSINE[i][j]);
        }
        puts("");
    }
    return 0;
}
