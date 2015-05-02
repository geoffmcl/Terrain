// Author:		Vahe Karamian
// Date:			03/10/2006
// Quick Note:	OpenGL Library Fundamentals
// Filename:	vkgllib.h

/* ********************************************************************
   *                 SOMETHINGS TO KEEP IN MIND                       *
	*                                                                  *
	* Suffix		Data Type	Typical C++ Type		 OpenGL Type Name     *
	* ======   =========   ================		 ================     *
	* b		   08-bit int	signed char				   GLbyte             *
	* s			16-bit int	short                   GLshort            *
	* i			32-bit int	int or long					GLint, GLsizei		 *
	* f			32-bit flp	float							GLfloat, GLclampf  *
	* d			64-bit flp	double						GLdouble, GLclampd *
	* ub			08-bit unc	unsigned char				GLubyte, GLboolean *
	* us			16-bit unn	unsigned short			   GLushort				 *
	* ui			32-bit unn	unsigned int or long    GLunint, GLenum,   *
	*                                               GLbitfield         *
	******************************************************************** */

// We don't need to include the gl.h and glu.h files, because glut.h does
// it for us!
//#include <glut.h>
/* ===============================================================================
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
   =============================================================================== */
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

#define WINDOW_WIDTH	640	// Window Width  Default
#define WINDOW_HEIGHT   480 // Window Height Default

#ifndef WIDTHBYTES
#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)
#endif

// definition of PI
#define PI						3.14159265

// Used to defien the title of the window
#define WINDOW_TITLE  "Vahe Karamian - OpenGL Terrain Generation - CodeProject Version 1.0"

#ifndef DEF_GREEN_TEX
#define DEF_GREEN_TEX "green.bmp"
#endif

static const char *green_bmp = DEF_GREEN_TEX;

// A simple structure to define a point whose coordinates are integers
/*typedef struct { GLint x, y; } GLintPoint;

// This structure is used to store the vertices of a polyline
typedef struct { int num; GLintPoint pt[100]; } GLintPointArray;

// Data for an Icosahedron
#define ICO_X	0.525731112119133606
#define ICO_Z	0.850650808352039932*/

/*static GLfloat vdataICO[12][3] =
{
	{ -ICO_X, 0.0, ICO_Z }, { ICO_X, 0.0, ICO_Z }, { -ICO_X, 0.0, -ICO_Z }, { ICO_X, 0.0, -ICO_Z },
	{ 0.0, ICO_Z, ICO_X }, { 0.0, ICO_Z, -ICO_X }, { 0.0, -ICO_Z, ICO_X }, { 0.0, -ICO_Z, -ICO_X },
	{ ICO_Z, ICO_X, 0.0 }, { -ICO_Z, ICO_X, 0.0 }, { ICO_Z, -ICO_X, 0.0 }, { -ICO_Z, -ICO_X, 0.0 }
};

static GLuint tindicesICO[20][3] =
{
	{ 1, 4, 0 }, { 4, 9, 0 }, { 4, 5, 9 }, { 8, 5, 4 }, { 1, 8, 4 },
	{ 1, 10, 8 }, { 10, 3, 8 }, { 8, 3, 5 }, { 3, 2, 5 }, { 3, 7, 2 },
	{ 3, 10, 7 }, { 10, 6, 7 }, { 6, 11, 7 }, { 6, 0, 11 }, {6, 1, 0 },
	{ 10, 1, 6 }, { 11, 0, 9 }, { 2, 11, 9 }, { 5, 2, 9 }, { 11, 2, 7 }
};*/

// Data for Tetrahedron
static GLfloat P1T[3] = { -2, 3, 0 }; 
static GLfloat P2T[3] = { -3, 0, 0 };
static GLfloat P3T[3] = { -1, 0, 3 }; 
static GLfloat P4T[3] = { -4, 0, 0 };

// Calculating the Normalized Cross Product of Two Vectors
void normalize( float v[3] )
{
	GLfloat d = sqrt( float(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) );
	if( d==0.0 )
	{
		cerr<<"zero length vector"<<endl;
		return;
	}
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

void normcrossprod( float v1[3], float v2[3], float out[3] )
{
	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];
	normalize( out );
}

////// Defines
#define BITMAP_ID 0x4D42		      // the universal bitmap ID
#define MAP_X	32				      // size of map along x-axis
#define MAP_Y	32				      // size of map along y-axis
#define MAP_Z	32				      // size of map along z-axis
#define MAP_SCALE	20.0f		      // the scale of the terrain map

////// Texture Information
BITMAPINFOHEADER	bitmapInfoHeader;	// temp bitmap info header
BITMAPINFOHEADER	landInfo;			// land texture info header
BITMAPINFOHEADER	waterInfo;			// water texture info header

//AUX_RGBImageRec
unsigned char *imageData   = 0;	// the map image data
unsigned char *landTexture = 0;	// land texture data
unsigned int   land = 0;		// the land texture object

////// Terrain Data
float terrain[MAP_X][MAP_Z][3];	// heightfield terrain data (0-255); 32x32

// LoadBitmapFile
// desc: Returns a pointer to the bitmap image of the bitmap specified
// by filename. Also returns the bitmap header information.
// No support for 8-bit bitmaps.
// 20150430 - Added LOTS more 'tests' that it is a SUITABLE bitmap
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;						    // the file pointer
	BITMAPFILEHEADER	bitmapFileHeader;	// bitmap file header
	unsigned char		*bitmapImage;		// bitmap image data
	DWORD   			imageIdx;		    // image index counter, for R/B swap
	unsigned char		tempRGB;			// swap variable
    LONG                width, height;
    WORD                Planes, BitCount;
    DWORD               Compression, SizeImage, Offset, byteWidth;
    BITMAPINFOHEADER    *pih = bitmapInfoHeader;

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1) {
		fclose(filePtr);
        return NULL;
    }
	
	// verify that this is a bitmap by checking for the universal bitmap id
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fclose(filePtr);
		return NULL;
	}
    Offset = bitmapFileHeader.bfOffBits;

	// read the bitmap information header
	if (fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1) {
		fclose(filePtr);
		return NULL;
    }

    width       = pih->biHeight;
    height      = pih->biWidth;
    Planes      = pih->biPlanes;
    BitCount    = pih->biBitCount;
    Compression = pih->biCompression;
    SizeImage   = pih->biSizeImage;
    byteWidth   = WIDTHBYTES(width * 8 * 3);
    if (BitCount != 24) {
        fclose(filePtr);
        return NULL;
    }
    if (Compression != BI_RGB) {
        fclose(filePtr);
        return NULL;
    }
    if ((byteWidth * height) != SizeImage) {
        fclose(filePtr);
        return NULL;
    }

    // NOT true for both files read!!!
    //if ((height != MAP_Y)||(width != MAP_X)) {
	//	fclose(filePtr);
    //    return NULL;
    //}

	// move file pointer to beginning of bitmap data
	if (fseek(filePtr, Offset, SEEK_SET)) {
        fclose(filePtr);
        return NULL;
    }

	// allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(SizeImage);
	// verify memory allocation
	if (!bitmapImage) {
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	// make sure bitmap image data was read
	if (fread(bitmapImage, 1, SizeImage, filePtr) != SizeImage) {
		free(bitmapImage);
        fclose(filePtr);
        bitmapImage = 0;
        return NULL;
    }

	// swap the R and B values to get RGB since the bitmap color format is in BGR
    // already tested that it *IS* a 24-bit bitmap, AND
    // that each row has NO padding, so can use just += 3
	for (imageIdx = 0; imageIdx < SizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// close the file and return the bitmap image data
	fclose(filePtr);
	return bitmapImage;
}

bool LoadTextures()
{
	// load the land texture data
	landTexture = LoadBitmapFile((char *)green_bmp, &landInfo);
	if (!landTexture)
		return false;

	// generate the land texture as a mipmap
	glGenTextures(1, &land);                  
	glBindTexture(GL_TEXTURE_2D, land);       
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, landInfo.biHeight, landInfo.biWidth, GL_RGB, GL_UNSIGNED_BYTE, landTexture);

	return true;
}