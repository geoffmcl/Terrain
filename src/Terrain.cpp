/*\
 * Terrain.cpp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
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
 * File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 *
\*/

#include <stdio.h>
#include <stdlib.h> // for exit, ... in unix
#include "imageloader.h"
#include "Terrain.h"

static const char *module = "Terrain";

// implementation
//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
//
// PNG color is a special alogorithm
// elevation0 = 256 * cropFile[cropIndex++] + cropFIle[cropIndex++];
// elevation = elevation0 < 32767 ? elevation0 : elevation0 - 65536
// png[pngIndex++] = elevation & 0x0000ff;
// png[pngIndex++] = (elevation & 0x00ff00) >> 8;
// png[pngIndex++] = (elevation & 0xff0000) >> 16;
// pgn[pngIndex++] = 255;

Terrain* loadTerrain(const char* filename, float height, bool is_bmp) 
{
    Image *image;
    size_t off;
    //unsigned char color;
    unsigned char r, g, b;
    int colr;
    float h;
    float div = (255 << 16) + (255 << 8) + 255;
    if (is_bmp)
        image = loadBMP(filename);
    else 
        image = loadPNG(filename);
    if (!image) {
        printf("Failed to load '%s'!\n", filename );
        exit(1);
    }
    printf("Converting loaded '%s' to a Terrain array width %d, height %d\n", filename, image->width, image->height );
    //is_bmp = true;
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
            off = 3 * (y * image->width + x);   // compute offset
            //if (is_bmp) {
			//    color = (unsigned char)image->pixels[off];
    		//	h = height * ((color / 255.0f) - 0.5f);
            //} else {
			    r = (unsigned char)image->pixels[off];
			    g = (unsigned char)image->pixels[off+1];
			    b = (unsigned char)image->pixels[off+2];
                colr = (r << 16) + (g << 8) + b;
    			h = height * ((colr / div) - 0.5f);
            //}
			t->setHeight(x, y, h);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}


// eof = Terrain.cpp
