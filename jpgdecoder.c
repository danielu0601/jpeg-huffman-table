#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_X 2048
#define MAX_Y 2048
#define BUFFER_SIZE 512

int X, Y;
unsigned char P, Nf, Ns, Ss, Se, Ah, Al;
unsigned char Q[4][8][8];
unsigned char Hi[4], Vi[4], Tqi[4];
unsigned char Tdi[4], Tai[4];
unsigned char Li[2][4][16];
unsigned int Vij[2][2][256][3];//[Tc][Th][i][length;codeword;symbol]
int image[3][MAX_X][MAX_Y];
unsigned char buffer[BUFFER_SIZE];
int previousDIFF[3] = {0};
double COSINE[8][8] = {
    {0.707107,  0.980785,  0.923880,  0.831470,  0.707107,  0.555570,  0.382683,  0.195090},
    {0.707107,  0.831470,  0.382683, -0.195090, -0.707107, -0.980785, -0.923880, -0.555570},
    {0.707107,  0.555570, -0.382683, -0.980785, -0.707107,  0.195090,  0.923880,  0.831470},
    {0.707107,  0.195090, -0.923880, -0.555570,  0.707107,  0.831470, -0.382683, -0.980785},
    {0.707107, -0.195090, -0.923880,  0.555570,  0.707107, -0.831470, -0.382683,  0.980785},
    {0.707107, -0.555570, -0.382683,  0.980785, -0.707107, -0.195090,  0.923880, -0.831470},
    {0.707107, -0.831470,  0.382683,  0.195090, -0.707107,  0.980785, -0.923880,  0.555570},
    {0.707107, -0.980785,  0.923880, -0.831470,  0.707107, -0.555570,  0.382683, -0.195090}};
int zigzag[64] = {
    0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63};

int readfile(unsigned char* buffer, int size, FILE *fp) {
    size_t tmp, read = 0;
    while( size > 0 && !feof(fp) ) {
        tmp = fread( buffer+read, sizeof(char), size, fp );
        size -= tmp;
        read += tmp;
    }
    return read;
}

void readheader(FILE *fp) {
    int length, flag = 1;
    while( flag ) {
        readfile( buffer, 2, fp );
        switch( buffer[0] ) {
            case 0xFF:
                switch( buffer[1] ) {
                    case 0xC0:
                        readfile(buffer, 2, fp);
                        length = (buffer[0]<<8)+buffer[1];
                        length -= 2;
                        readfile(buffer, length, fp);
                        P = buffer[0];
                        Y = (buffer[1]<<8)+buffer[2];
                        X = (buffer[3]<<8)+buffer[4];
                        Nf = buffer[5];
                        for( int i = 6; i < length; i+=3 ) {
                            int Ci = buffer[i];
                            Hi[Ci] = buffer[i+1] >> 4;
                            Vi[Ci] = buffer[i+1] & 0x0F;
                            Tqi[Ci] = buffer[i+2];
                        }
                        break;
                    case 0xC4:
                        readfile(buffer, 2, fp);
                        length = (buffer[0]<<8)+buffer[1];
                        length -= 2;
                        readfile(buffer, length, fp);
                        for( int i = 0; length; ) {
                            int Tc = buffer[0+i] >> 4;
                            int Th = buffer[0+i] & 0x0F;
                            length -= 1;
                            memcpy(Li[Tc][Th], buffer+1+i, 16);
                            length -= 16;
                            int codeword = 0;
                            int total = 0;
                            for( int j = 0; j < 16; j++ ) {
                                for( int k = 0; k < Li[Tc][Th][j]; k++ ) {
                                    Vij[Tc][Th][total][0] = j+1;
                                    Vij[Tc][Th][total][1] = codeword;
                                    Vij[Tc][Th][total][2] = buffer[17+i+total];
                                    codeword++;
                                    total++;
                                }
                                codeword <<= 1;
                            }
                            i += total+17;
                            length -= total;
                        }
                        break;
                    case 0xD0: case 0xD1: case 0xD2: case 0xD3:
                    case 0xD4: case 0xD5: case 0xD6: case 0xD7:
                        break;
                    case 0xD8:
                        break;
                    case 0xD9:
                        break;
                    case 0xDA:
                        readfile(buffer, 2, fp);
                        length = (buffer[0]<<8)+buffer[1];
                        length -= 2;
                        readfile(buffer, length, fp);
                        Ns = buffer[0];
                        for( int i = 0; i < 6; i+=2 ) {
                            int Csi = buffer[1+i];
                            Tdi[Csi] = buffer[2+i] >> 4;
                            Tai[Csi] = buffer[2+i] & 0x0F;
                        }
                        Ss = buffer[7];
                        Se = buffer[8];
                        Ah = buffer[9] >> 4;
                        Al = buffer[9] & 0x0F;
                        //return ;?
                        break;
                    case 0xDB:
                        readfile(buffer, 2, fp);
                        length = (buffer[0]<<8)+buffer[1];
                        length -= 2;
                        readfile(buffer, length, fp);
                        for( int i = 0; length; i+=65 ) {
                            //int Pq = buffer[0+i] >> 4;
                            int Tq = buffer[0+i] & 0x0F;
                            //unsigned char Q[4][64];
                            //memcpy(Q[Tq], buffer+1+i, 64);
                            for( int j = 0; j < 64; j++ ) {
                                *(Q[Tq][0] +zigzag[j]) = buffer[1+i+j];
                            }
                            length -= 65;
                        }
                        break;
                    case 0xE0:
                        readfile(buffer, 2, fp);
                        length = (buffer[0]<<8)+buffer[1];
                        length -= 2;
                        readfile(buffer, length, fp);
                        /*
                           char identifier[5];
                           memcpy(identifier, buffer, 5);
                           char version[3];
                           version[0] = buffer[5]+'0';
                           version[1] = '.';
                           version[2] = buffer[6]+'0';
                           int units = buffer[7];
                           int xdensity = (buffer[8]<<8) + buffer[9];
                           int ydensity = (buffer[10]<<8) + buffer[11];
                           buffer[12];thumbnail horizontal pixels
                           buffer[13];thumbnail vertical pixels
                         */
                        break;
                    case 0xE1: case 0xE2: case 0xE3:
                    case 0xE4: case 0xE5: case 0xE6: case 0xE7:
                    case 0xE8: case 0xE9: case 0xEA: case 0xEB:
                    case 0xEC: case 0xED: case 0xEE: case 0xEF:
                        readfile(buffer, 2, fp);
                        length = (buffer[0]<<8)+buffer[1];
                        length -= 2;
                        readfile(buffer, length, fp);
                        break;
                    case 0xFE:
                        readfile(buffer, 2, fp);
                        length = (buffer[0]<<8)+buffer[1];
                        length -= 2;
                        readfile(buffer, length, fp);
                        break;
                    default:
                        readfile(buffer, 2, fp);
                        length = (buffer[0]<<8)+buffer[1];
                        length -= 2;
                        readfile(buffer, length, fp);
                        break;
                }
                break;
            default:
                flag = 0;
                break;
        }
    }
    fseek(fp,-2,SEEK_CUR);
    return;
}

size_t read;
int ptro, ptri, skip = 0;
unsigned char get1bit(FILE *fp) {
    // check if need to read buffer
    if( ptri < 0 ) {
        ptri = 7;
        ptro++;
        // skip 0x00 after 0xFF
        if( skip == 1 ) {
            ptro++;
            skip = 0;
        }
        // if end of buffer, read more
        if( ptro >= read ) {
            read = readfile( buffer, BUFFER_SIZE, fp );
            if( read == 0 ) {
                return -1;
            }
            ptro = 0;
        }
        // check 0xFF00 and header
        if( buffer[ptro] == 0xFF ) {
            if( ptro == read-1 ) { // end of buffer, next may be 0
                read = readfile( buffer, 1, fp );
                if( buffer[0] == 0x00 ) {
                } else {
                    printf("Encounter Header 0xFF%02X\n", buffer[0] );
                    if( buffer[0] == 0xD0 || buffer[0] == 0xD1 || buffer[0] == 0xD2
                     || buffer[0] == 0xD3 || buffer[0] == 0xD4 || buffer[0] == 0xD5
                     || buffer[0] == 0xD6 || buffer[0] == 0xD7 ) {
                        previousDIFF[0] = 0;
                        previousDIFF[1] = 0;
                        previousDIFF[2] = 0;
                    }
                }
            } else if( buffer[ptro+1] == 0x00 ) { // next is 0, skip
                skip = 1;
            } else { // header
                printf("Encounter Header 0xFF%02X\n", buffer[ptro+1] );
                if( buffer[0] == 0xD0 || buffer[0] == 0xD1 || buffer[0] == 0xD2
                 || buffer[0] == 0xD3 || buffer[0] == 0xD4 || buffer[0] == 0xD5
                 || buffer[0] == 0xD6 || buffer[0] == 0xD7 ) {
                    previousDIFF[0] = 0;
                    previousDIFF[1] = 0;
                    previousDIFF[2] = 0;
                }
            }
        }
    }
    unsigned char ans = ((buffer[ptro] >> ptri) & 0x01);
    ptri--;
    return ans;
}

//unsigned char image[3][MAX_X][MAX_Y];
void MCU(FILE *fp, int layer, int arr[][8] ) {
    int flag = 1;
    while( flag ) {
        int cod = 0;
        // for all codeword length
        for( int i = 1, j = 0; i <= 16 && flag; i++ ) {
            cod |= get1bit(fp);
            // for all codeword in length 'i'
            for( ; j < 256 && flag; j++ ) {
                // equal length 
                if( Vij[ 0 ][ Tdi[layer+1] ][j][0] == i ) {
                    // same codeword
                    if( Vij[ 0 ][ Tdi[layer+1] ][j][1] == cod ) {
                        unsigned char symbol = Vij[0][ Tdi[layer+1] ][j][2];
                        int atai = 0;
                        for( int k = 0; k < (symbol&0x0F); k++ ) {
                            atai <<= 1;
                            atai |= get1bit(fp);
                        }
                        int tmp = symbol&0x0F;
                        if( atai >> (tmp-1) == 0 ) {
                            atai = -((~atai)&((0x01<<tmp)-1));
                        }
                        previousDIFF[layer] += atai;
                        arr[0][0] = previousDIFF[layer] * Q[ Tqi[layer+1] ][0][0];
                        flag=0;
                    }
                } else if ( Vij[ 0 ][ Tdi[layer+1] ][j][0] < i ) {
                    // end of haffman code
                    j=256;flag=0;
                } else if ( Vij[ 0 ][ Tdi[layer+1] ][j][0] > i ) {
                    // length > codeword
                    break;
                }
            }
            cod <<= 1;
        }
    }
    // zz is AC mode numbers 1~63
    for( int zz = 1; zz < 64; zz++ ) {
        flag = 1;
        while( flag ) {
            int cod = 0;
            for( int i = 1, j = 0; i <= 16 && flag; i++ ) {
                cod |= get1bit(fp);
                for( ; j < 256 && flag; j++ ) {
                    if( Vij[1][ Tai[layer+1] ][j][0] == i ) {
                        // compare codeword
                        if( Vij[1][ Tai[layer+1] ][j][1] == cod ) {
                            unsigned char symbol = Vij[1][ Tai[layer+1] ][j][2];
                            zz += (symbol>>4)&0x0F;
                            if( symbol == 0 ) {
                                return ;
                                flag = 0;
                                break;
                            }
                            int atai = 0;
                            for( int k = 0; k < (symbol&0x0F); k++ ) {
                                atai <<= 1;
                                atai |= get1bit(fp);
                            }
                            int tmp = symbol&0x0F;
                            if( atai >> (tmp-1) == 0 ) {
                                atai = -((~atai)&((0x01<<tmp)-1));
                            }
                            int _X = zigzag[zz]/8;
                            int _Y = zigzag[zz]%8;
                            arr[_X][_Y] = atai * Q[ Tqi[layer+1] ][_X][_Y];
                            flag=0;
                        }
                    } else if ( Vij[1][ Tai[layer+1] ][j][0] < i ) {
                        // endof haffman code
                        printf("Error, not found match code for len%2d, %02X\n", i, cod);
                        j=256;flag=0;
                    } else if ( Vij[1][ Tai[layer+1] ][j][0] > i ) {
                        break;
                    }
                }
                cod <<= 1;
            }
        }
    }
}

void idct(int block[][8], int RECOVER[][8]) {
    for( int i  = 0; i < 8; i++ ) {
        for( int j = 0; j < 8; j++ ) {
            double tmp = 0.0;
            for( int u = 0; u < 8; u++ ) {
                for( int v = 0; v < 8; v++ ) {
                    tmp += (double)block[u][v] * COSINE[i][u] * COSINE[j][v];
                }
            }
            tmp /= 4;
            RECOVER[i][j] = (int)tmp;
        }
    }
}

void readMCU(FILE *fp, int MCUV, int MCUH, int offsetX, int offsetY ) {
    int tmp1[8][8], tmp2[8][8];
    for(int k = 1; k <= 3; k++ ) {
        for( int i = 0; i < Vi[k]; i++ ) {
            for( int j = 0; j < Hi[k]; j++ ) {
                memset( tmp1, 0, sizeof(tmp1) );
                memset( tmp2, 0, sizeof(tmp2) );
                MCU( fp, k-1, tmp1 );
                idct( tmp1, tmp2 );
                for( int p = 0; p < 8; p++ ) {
                    for( int q = 0; q < 8; q++ ) {
                        int _X = offsetX + (i*8) + (p*MCUV/Vi[k]);
                        int _Y = offsetY + (j*8) + (q*MCUH/Hi[k]);
                        for( int m = 0; m < MCUV/Vi[k]; m++ ) {
                            for( int n = 0; n < MCUH/Hi[k]; n++ ) {
                                image[k-1][_X+m][_Y+n] = tmp2[p][q];
                            }
                        }
                    }
                }
            }
        }
    }
}

void writebmpheader(FILE *fp) {
    /**
     * bmp header format
     *  2 ~  5 size
     * 18 ~ 21 width
     * 22 ~ 25 height
     * 28 ~ 29 bits
     * 34 ~ 37 width*height*bits/8
     * 38 ~ 41 x resolution
     * 42 ~ 45 y resolution
     * 46 ~ 49 n colours
     * 50 ~ 53 important  colours
     */
    unsigned char bmpheader[54] = {
        0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x36, 0x00, 0x00 ,0x00, 0x28, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    int T = X*Y*4 + 54;
    bmpheader[ 2] =   T         & 0xFF;
    bmpheader[ 3] = ( T >>  8 ) & 0xFF;
    bmpheader[ 4] = ( T >> 16 ) & 0xFF;
    bmpheader[ 5] = ( T >> 24 ) & 0xFF;

    bmpheader[18] =   X         & 0xFF;
    bmpheader[19] = ( X >>  8 ) & 0xFF;
    bmpheader[20] = ( X >> 16 ) & 0xFF;
    bmpheader[21] = ( X >> 24 ) & 0xFF;
    T = -Y;
    bmpheader[22] =   T         & 0xFF;
    bmpheader[23] = ( T >>  8 ) & 0xFF;
    bmpheader[24] = ( T >> 16 ) & 0xFF;
    bmpheader[25] = ( T >> 24 ) & 0xFF;
    fwrite(bmpheader, sizeof(char), 54, fp);
}

int main( int argc, char *argv[] ) {
    /* check the input format */
    if( argc < 2 ) {
        puts("./jpgdecoder filename");
    }

    /* open the file stream for read and write */
    FILE *rfp, *wfp;
    rfp = fopen(argv[1], "r");
    char *tmp;
    tmp = strstr(argv[1], ".jpg");
    strncpy(tmp, ".bmp", 4);
    wfp = fopen(argv[1], "w");

    /* read the header */
    readheader( rfp );

    /* deal with image */
    int MCUH = Hi[1] , MCUV = Vi[1];
    if( MCUH < Hi[2] ) MCUH = Hi[2];
    if( MCUH < Hi[3] ) MCUH = Hi[3];
    if( MCUV < Vi[2] ) MCUV = Vi[2];
    if( MCUV < Vi[3] ) MCUV = Vi[3];

    int MCUX = X/(MCUH<<3);
    if( X%(MCUH<<3) > 0 ) MCUX++;
    int MCUY = Y/(MCUV<<3);
    if( Y%(MCUV<<3) > 0 ) MCUY++;

    ptro = BUFFER_SIZE, ptri = -1;
    for( int i = 0; i < MCUY; i++ ) {
        for( int j = 0; j < MCUX; j++ ) {
            readMCU( rfp, MCUV, MCUH, i*8*MCUV, j*8*MCUH );
        }
    }

    /* write bmp header */
    writebmpheader( wfp );

    /* write rest of image*/
    for( int i = 0; i < Y; i++ ) {
        for( int j = 0; j < X; j++ ) {
            int R = 128 + image[0][i][j]                                      + (int)(1.402 * (double)image[2][i][j]);
            int G = 128 + image[0][i][j] - (int)(0.34414 * (double)image[1][i][j] + 0.71414 * (double)image[2][i][j]);
            int B = 128 + image[0][i][j] + (int)(1.772   * (double)image[1][i][j]                                   );
//            R = (R>255)?255:(R<0)?0:R;
            if( R > 255 ) R = 255;
            if( R < 0   ) R = 0;
            if( G > 255 ) G = 255;
            if( G < 0   ) G = 0;
            if( B > 255 ) B = 255;
            if( B < 0   ) B = 0;
            unsigned char _tmp[4] = {B,G,R,0x00}; //BGRA
            fwrite(_tmp, sizeof(char), 4, wfp);
        }
    }

    /* close the file stream */
    fclose(rfp);
    fclose(wfp);

    return 0;
}
