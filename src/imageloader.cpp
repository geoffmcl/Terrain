/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdlib.h> // for exit, ... in unix
#include "lodepng/lodepng.h"
#include "imageloader.h"

using namespace std;

static const char *module = "imageloader";

Image::Image(char* ps, int w, int h) : pixels(ps), width(w), height(h) {}

Image::~Image() {
	delete[] pixels;
}

namespace {
	//Converts a four-character array to an integer, using little-endian form
	int toInt(const char* bytes) {
		return (int)(((unsigned char)bytes[3] << 24) |
					 ((unsigned char)bytes[2] << 16) |
					 ((unsigned char)bytes[1] << 8) |
					 (unsigned char)bytes[0]);
	}
	
	//Converts a two-character array to a short, using little-endian form
	short toShort(const char* bytes) {
		return (short)(((unsigned char)bytes[1] << 8) |
					   (unsigned char)bytes[0]);
	}
	
	//Reads the next four bytes as an integer, using little-endian form
	int readInt(ifstream &input) {
		char buffer[4];
		input.read(buffer, 4);
		return toInt(buffer);
	}
	
	//Reads the next two bytes as a short, using little-endian form
	short readShort(ifstream &input) {
		char buffer[2];
		input.read(buffer, 2);
		return toShort(buffer);
	}
	
	//Just like auto_ptr, but for arrays
	template<class T>
	class auto_array {
		private:
			T* array;
			mutable bool isReleased;
		public:
			explicit auto_array(T* array_ = NULL) :
				array(array_), isReleased(false) {
			}
			
			auto_array(const auto_array<T> &aarray) {
				array = aarray.array;
				isReleased = aarray.isReleased;
				aarray.isReleased = true;
			}
			
			~auto_array() {
				if (!isReleased && array != NULL) {
					delete[] array;
				}
			}
			
			T* get() const {
				return array;
			}
			
			T &operator*() const {
				return *array;
			}
			
			void operator=(const auto_array<T> &aarray) {
				if (!isReleased && array != NULL) {
					delete[] array;
				}
				array = aarray.array;
				isReleased = aarray.isReleased;
				aarray.isReleased = true;
			}
			
			T* operator->() const {
				return array;
			}
			
			T* release() {
				isReleased = true;
				return array;
			}
			
			void reset(T* array_ = NULL) {
				if (!isReleased && array != NULL) {
					delete[] array;
				}
				array = array_;
			}
			
			T* operator+(int i) {
				return array + i;
			}
			
			T &operator[](int i) {
				return array[i];
			}
	};
}   // end anonymous namespace


Image *loadBMP(const char* filename) 
{
	ifstream input;
	input.open(filename, ifstream::binary);
	//assert(!input.fail() || !"Could not find file");
    if (input.fail()) {
        printf("Could NOT find file '%s'\n", filename);
        exit(1);
    }
	char buffer[2];
	input.read(buffer, 2);
	//assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
	if ( !( (buffer[0] == 'B') && (buffer[1] == 'M') ) ) {
        printf("'%s' not a bitmap file", filename);
        exit(1);
    }
	input.ignore(8);
	int dataOffset = readInt(input);
	
	//Read the header
	int headerSize = readInt(input);
	int width;
	int height;
	switch(headerSize) {
		case 40:
			//V3
			width = readInt(input);
			height = readInt(input);
			input.ignore(2);
			//assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			if (readShort(input) != 24) {
                printf("Image file '%s' is not 24 bits per pixel\n", filename);
                exit(1);
            }
			//assert(readShort(input) == 0 || !"Image is compressed");
			if (readShort(input) != 0) {
                printf("Image '%s' is compressed!", filename);
                exit(1);
            }
			break;
		case 12:
			//OS/2 V1
			width = readShort(input);
			height = readShort(input);
			input.ignore(2);
			//assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			if (readShort(input) != 24) {
                printf("Image file '%s' is not 24 bits per pixel\n", filename);
                exit(1);
            }
			break;
		case 64:
			//OS/2 V2
			//assert(!"Can't load OS/2 V2 bitmaps");
			printf("Can't load OS/2 V2 bitmaps '%s'\n", filename);
            exit(1);
			break;
		case 108:
			//Windows V4
			//assert(!"Can't load Windows V4 bitmaps");
			printf("Can't load Windows V4 bitmaps '%s'\n", filename);
            exit(1);
			break;
		case 124:
			//Windows V5
			//assert(!"Can't load Windows V5 bitmaps");
			printf("Can't load Windows V5 bitmaps '%s'\n", filename);
            exit(1);
			break;
		default:
			//assert(!"Unknown bitmap format");
			printf("Unknown bitmap format '%s'\n", filename);
            exit(1);
	}
	
	//Read the data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;
    int soff, doff;
	auto_array<char> pixels(new char[size]);

	input.seekg(dataOffset, ios_base::beg);
	input.read(pixels.get(), size);
	
	//Get the data into the right format
	auto_array<char> pixels2(new char[width * height * 3]);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			for(int c = 0; c < 3; c++) {
                doff = 3 * (width * y + x) + c;
                soff = bytesPerRow * y + 3 * x + (2 - c);
				pixels2[doff] = pixels[soff];
			}
		}
	}
	
	input.close();
    printf("Loaded bitmap '%s',\nImage width %d, height %d\n", filename, width, height);
	return new Image(pixels2.release(), width, height);
}


//Reads a png image from file.
Image *loadPNG(const char *infile)
{
    std::vector<unsigned char> pixels; //the raw pixels
    unsigned width, height, x, y, c;

    unsigned error = lodepng::decode(pixels, width, height, infile, LCT_RGB, 8);

    if(error)
    {
        std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
        exit(1);
    }
    std::cout << "Loaded image, width " << width << ", height " << height << std::endl;

	// Get the data into the right format, and into an array returnable
    int bytesPerRow = width * 3;
    size_t offd, offs;
	auto_array<char> pixels2(new char[width * height * 3]);
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			for (c = 0; c < 3; c++) {
                offd = 3 * (width * y + x) + c;
                offs = bytesPerRow * y + 3 * x + (2 - c);
				pixels2[offd] = pixels[offs];
			}
		}
	}

    Image *pi = new Image(pixels2.release(),width,height);
    return pi;
}

//////////////////////////////////////////////////////////////////////////////////
// Test BMP, PNG suitable
//Converts a four-character array to an integer, using little-endian form
int m_toInt(const char* bytes) 
{
	return (int)(((unsigned char)bytes[3] << 24) |
					((unsigned char)bytes[2] << 16) |
					((unsigned char)bytes[1] << 8) |
					(unsigned char)bytes[0]);
}

//Converts a two-character array to a short, using little-endian form
short m_toShort(const char* bytes) 
{
	return (short)(((unsigned char)bytes[1] << 8) |
					(unsigned char)bytes[0]);
}


// Reads the next four bytes as an integer, using little-endian form
int m_readInt(ifstream &input) 
{
    char buffer[4];
	input.read(buffer, 4);
	return m_toInt(buffer);
}
//Reads the next two bytes as a short, using little-endian form
short m_readShort(ifstream &input) 
{
	char buffer[2];
	input.read(buffer, 2);
	return m_toShort(buffer);
}

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

bool isBMP(const char *filename, bool verb)
{
    bool bret = true;
	ifstream input;
	input.open(filename, ifstream::binary);
	//assert(!input.fail() || !"Could not find file");
    if (input.fail()) {
        if (verb)
            printf("Could NOT find file '%s'\n", filename);
        return false;
    }
    // read BITMAPFILEHEADER
	char buffer[2];
	input.read(buffer, 2);
	//assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
	if ( !( (buffer[0] == 'B') && (buffer[1] == 'M') ) ) {
        if (verb)
            printf("'%s' not a bitmap file", filename);
        return false;
    }
	input.ignore(8);
	int dataOffset = m_readInt(input);
	
	// Read the header - can be different sizes, but seem to have COMMON block
	int headerSize = m_readInt(input);
	int width, height, bpp, rbs, comp;
	width = m_readInt(input);
	height = m_readInt(input);
	input.ignore(2);
    bpp = m_readShort(input);
	comp = m_readShort(input);
    rbs = getRowBytes( width, bpp );    // calculate the padded row byte size
	switch(headerSize) {
		case 40:
			//V3
            if (rbs == -1) {
                if (verb)
                    printf("Image file '%s' invalid BPP %d!\n", filename, bpp);
                bret = false;
            } else 
			//assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			if (bpp != 24) {
                if (verb)
                    printf("Image file '%s' is not 24 bits per pixel\n", filename);
                bret = false;
            } else
			//assert(readShort(input) == 0 || !"Image is compressed");
			if (comp != 0) {
                if (verb)
                    printf("Image '%s' is compressed!", filename);
                bret = false;
            }
			break;
		case 12:
			//OS/2 V1
			//assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
			if (bpp != 24) {
                if (verb)
                    printf("Image file '%s' is not 24 bits per pixel\n", filename);
                bret = false;
            } else
			if (comp != 0) {
                if (verb)
                    printf("Image '%s' is compressed!", filename);
                bret = false;
            }
			break;
		case 64:
			//OS/2 V2
			//assert(!"Can't load OS/2 V2 bitmaps");
            if (verb)
    			printf("Can't load OS/2 V2 bitmaps '%s'\n", filename);
            bret = false;
			break;
		case 108:
			//Windows V4
			//assert(!"Can't load Windows V4 bitmaps");
            if (verb)
    			printf("Can't load Windows V4 bitmaps '%s'\n", filename);
            bret = false;
			break;
		case 124:
			//Windows V5
            if (rbs == -1) {
                if (verb)
                    printf("Image file '%s' invalid BPP %d!\n", filename, bpp);
                bret = false;
            } else {
	    		//assert(!"Can't load Windows V5 bitmaps");
                if (verb)
    			    printf("Can't load Windows V5 bitmaps '%s'\n", filename);
                bret = false;
            }
			break;
		default:
			//assert(!"Unknown bitmap format");
            if (verb)
    			printf("Unknown bitmap format '%s'\n", filename);
            bret = false;
            break;
	}
	input.close();
    return bret;
}

#define BMP_FILE_HDR    14
#define BMP_INFO_HDR    40
#define MX_BM_BUFF      BMP_FILE_HDR + BMP_INFO_HDR

int isBMP2(const char *filename, int verb)
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


bool isPNG(const char *file, bool verb)
{
    FILE *fp = fopen(file,"rb");
    if (!fp) {
        if (verb)
            printf("isPNG: error can not open file '%s'\n", file);
        return false;
    }
    unsigned char in[16];
    size_t res = fread(in,16,1,fp);
    fclose(fp);
    if (res != 1) {
        if (verb)
            printf("isPNG: error: can not read file %s\n",file);
        return false;
    }
   if (in[0] != 137 || in[1] != 80 || in[2] != 78 || in[3] != 71
       || in[4] != 13 || in[5] != 10 || in[6] != 26 || in[7] != 10)
   {
       if (verb)
            printf("isPNG: error: the first 8 bytes are not the correct PNG signature\n");
        return false;
   }
   if(in[12] != 'I' || in[13] != 'H' || in[14] != 'D' || in[15] != 'R')
   {
       if (verb)
           printf("isPNG: error: it doesn't start with a IHDR chunk!\n");
       return false;
   }
   return true;
}

//////////////////////////////////////////////////////////////////////////////////

// eof
