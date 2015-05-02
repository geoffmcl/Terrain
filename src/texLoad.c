/*\
 * texLoad.c
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>    // needed for some defines ONLY
#endif
#include <GL/glut.h>
#include "texLoad.h"

static const char *module = "texLoad";

GLuint GenMipmap( int width, int height, unsigned char *data )
{
    GLuint texture;
    glGenTextures( 1, &texture ); 
    
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );
    
    return texture;
}

/*get filesize:*/
size_t get_file_size( FILE *file )
{
    size_t size;
    long l;
    if (fseek(file , 0 , SEEK_END)) {
        printf("%s: fseek end FAILED\n", module);
        return -1;
    }
    l = ftell(file);
    if (l == -1) {
        printf("%s: ftell FAILED\n", module);
        return -1;
    }
    size = l;
    rewind(file);
    return size;
}

// TODO: Load OTHER texture formats besides RAW
// ============================================
GLuint LoadTextureRAW( const char * filename )
{
    GLuint texture;
    int width, height, size, res;
    unsigned char *data;
    FILE * file;
    file = fopen( filename, "rb" );
    if ( file == NULL ) {
        printf("%s: Unable to open file '%s'\n", module, filename);
        return 0;
    }
    
    width  = 1024;
    height = 512;
    res = get_file_size(file);
    size   = width * height * 3;
    if (res != size) {
        fclose(file);
        printf("%s: File size is incorrect! Expected %d, got %d!\n", module, size, res );
        return 0;
    }
    data = (unsigned char *)malloc( size );
    if (!data) {
        printf("%s: memory allocation FAILED on %d bytes!\n", module, size );
        fclose(file);
        return 0;
    }
    res = fread( data, size, 1, file );
    fclose( file );
    if (res != 1) {
        printf("%s: File READ failed!\n", module );
        free(data);
        return 0;
    }

    texture = GenMipmap( width, height, data );

    free( data );
    
    return texture; // return an 'int' index for the texture
}

int isRAW( const char * filename )
{
    int width, height, size, res;
    FILE * file;
    file = fopen( filename, "rb" );
    if ( file == NULL ) {
        printf("%s: Unable to open file '%s'\n", module, filename);
        return 0;
    }
    
    width  = 1024;
    height = 512;
    res = get_file_size(file);
    fclose(file);
    size   = width * height * 3;
    if (res != size) {
        printf("%s: File size is incorrect! Expected %d, got %d!\n", module, size, res );
        return 0;
    }
    return 1;   // yes, it is 'the' RAW file

}

/*\
 *
typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;         // 2  2
  DWORD bfSize;         // 4  6
  WORD  bfReserved1;    // 2  8
  WORD  bfReserved2;    // 2  10
  DWORD bfOffBits;      // 4  14
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;         // 4  4
  LONG  biWidth;        // 4  8
  LONG  biHeight;       // 4  12
  WORD  biPlanes;       // 2  14
  WORD  biBitCount;     // 2  16
  DWORD biCompression;  // 4  20
  DWORD biSizeImage;    // 4  24
  LONG  biXPelsPerMeter;// 4  28
  LONG  biYPelsPerMeter;// 4  32
  DWORD biClrUsed;      // 4  36
  DWORD biClrImportant; // 4  40
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
// And this is COMMON with a BITMAPV4HEADER, and BITMAPV5HEADER
 *
\*/
#define BMP_FILE_HDR    14
#define BMP_INFO_HDR    40
#define MX_BM_BUFF      BMP_FILE_HDR + BMP_INFO_HDR
#ifndef WIDTHBYTES
#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)
#endif

int getRowBytes( int width, int bpp )
{
    int j, l, k = width;
    switch(bpp) {
    case 1: j = k / 8;	 // fit 8 bits per byte
		if( k % 8 )	j++; // fit 8 bits per byte
		l = WIDTHBYTES( (j * 8) ); break;
    case 4: j = k / 2;	// fit 2 nibbles per byte 
        if( k % 2 ) j++;
        l = WIDTHBYTES( (j * 8) ); break;
    case 8: j = k;	// use byte index to 256 color table
        l = WIDTHBYTES( (j * 8) ); break;
    case 16: j = k * 2;
        l = WIDTHBYTES( (j * 8) ); break;
    case 24: j = k * 3;
        l = WIDTHBYTES( (j * 8) ); break;
    case 32: j = k * 4;
        l = WIDTHBYTES( (j * 8) ); break;
    default:
        printf("Invalid BitCount %d!\n", bpp);
        return -1;  // exit(1);	
    }
    return l;
}

int isBMP(const char *filename, int verb)
{
	char buffer[MX_BM_BUFF];
    unsigned int *pui;
    unsigned short *pus;
    int bret = 1;
	int headerSize;
	int width, height, bpp, rbs, comp;
    size_t res;
    FILE *file; 
    file = fopen( filename, "rb" );
    if ( file == NULL ) {
        printf("%s: Unable to open file '%s'\n", module, filename);
        return 0;
    }
	// Read the header - can be different sizes, but seem to have COMMON block
    // read BITMAPFILEHEADER + BITMAPINFOHEADER
    res = fread(buffer,MX_BM_BUFF,1,file);
    fclose(file);
	if ( !( (buffer[0] == 'B') && (buffer[1] == 'M') ) ) {
        if (verb)
            printf("'%s' not a bitmap file", filename);
        return 0;
    }
	pui = (unsigned int *)( &buffer[BMP_FILE_HDR] );
    headerSize = pui[0];
	width      = pui[1];
	height     = pui[2];
	// input.ignore(2);
    pus = (unsigned short *)( &buffer[BMP_FILE_HDR+14] );
    bpp        = pus[0];
	comp       = pus[1];
    rbs = getRowBytes( width, bpp );    // calculate the padded row byte size
	switch(headerSize) {
		case 40:
			//V3
            if (rbs == -1) {
                if (verb)
                    printf("Image file '%s' invalid BPP %d!\n", filename, bpp);
                bret = 0;
            } else 
			if (bpp != 24) {
                if (verb)
                    printf("Image file '%s' is not 24 bits per pixel\n", filename);
                bret = 0;
            } else
			if (comp != 0) {
                if (verb)
                    printf("Image '%s' is compressed!", filename);
                bret = 0;
            }
			break;
		case 12:
			//OS/2 V1
			if (bpp != 24) {
                if (verb)
                    printf("Image file '%s' is not 24 bits per pixel\n", filename);
                bret = 0;
            } else
			if (comp != 0) {
                if (verb)
                    printf("Image '%s' is compressed!", filename);
                bret = 0;
            }
			break;
		case 64:
			//OS/2 V2
            if (verb)
    			printf("Can't load OS/2 V2 bitmaps '%s'\n", filename);
            bret = 0;
			break;
		case 108:
			//Windows V4
            if (verb)
    			printf("Can't load Windows V4 bitmaps '%s'\n", filename);
            bret = 0;
			break;
		case 124:
			//Windows V5
            if (rbs == -1) {
                if (verb)
                    printf("Image file '%s' invalid BPP %d!\n", filename, bpp);
                bret = 0;
            } else {
                if (verb)
    			    printf("Can't load Windows V5 bitmaps '%s'\n", filename);
                bret = 0;
            }
			break;
		default:
            if (verb)
    			printf("Unknown bitmap format '%s'\n", filename);
            bret = 0;
            break;
	}
    return bret;
}


GLuint LoadTexture( const char * filename )
{
    GLuint texture = 0;
    if (isRAW(filename) ) {
        texture = LoadTextureRAW(filename);
    }

    return texture;
}


// eof = texLoad.c
