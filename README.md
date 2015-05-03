# Terrain Project - 20150503

I was looking for examples of overlaying a heightmap as a texture, and found a few. Most have a prequistite of GLUT/OpenGLUT, and cmake will abort if this is NOT found. The current 'main' example is Terrain2.

##### Terrain2:

Note: Is a Windows only application!

It reads data/Terrain.raw and renders it as a height map. Originally it was just shades of green, but borrowing from some work done on my [Dem Project](https://gitlab.com/fgtools/dem-project) I get the range of elevations to be used, and generate a look up height to color table.

This give it great coloring, but still not happy with the model location in the windows. Added some x,y,z 'arrows' and see that the model is weidly offset from 0,0,0, the center of the universe!

Added some rotations but due to this offset it does not yet rotate as desired. Also has a zoom... and can be run in windows full screeen mode toggled by the F1 key.


## Other Example found.

##### terrain:

Found this tutorial - http://www.videotutorialsrock.com/opengl_tutorial/terrain/text.php -
and its small sample source. There is also a short video.

It uses a heightmap to generate the terrain. It is a very simple GLUT app, thus 
GLUT/OpenGLUT MUST be found. It uses its own heightmap.bmp, the path to which can 
be given on the command line.

But the display is only of a 2D model in 3D.

##### Lighthouse3D:

In searching for other examples, I also found Lighthouse3D, but this is quite simple, 
and it not really a good example of what I was looking for.

##### ML_Terrain:

And also Multilayered Terrain example, in the ML_Terrain directory. However it is windows 
only and has a considerable nummber of dependencies... see src\ML_Terrain\README.md 
for further details. Only if ALL the dependencies are found will this be included 
in the build. And for this wrote some cmake 'find' modules not usually present in the 
standard cmake distribution, or even if they are, do not do exactly what I wanted, namely -

 1. If using MSVC, try to find the 'static' library version first. Avoid DLL location problems for running.
 2. If using MSVC, try to find a Debug AND Release version setting debug;libd.lib;optimized;lib.lib
 
The list of personlised cmake 'find' module is FindASSIMP.cmake, FindFREEIMAGE.cmake, FindGLEW.cmake, FindGLM.cmake, and of course FindGLUT.cmake. Only the latter is required for all examples.

Also a whole series of experiments on generating a cube, and then a sphere...

##### cube:

A simple colored rotating cube. Crude, looks a little odd, and some sides appear missing...

##### cube2:

A colored rotating cube with two faces overlayed with the vtr.bmp texture. Simple but ok...

##### sphere1:

Uses glutSolidSphere(1.0,20.0,20.0) and gluOrtho2D(-1.0,1.0,-1.0,1.0); File contains so other scrasp for sphere generations, but NOT tested/used

##### sphere2:

Not really a glut display at all. Generates a crude svg (xml) shape approximating a 'sphere' of sorts.

##### sphere3:

Is supposed to be generating a sphere, through class LED_cube, but at present ONLY displays a 16x16 grid of points... nothing like what the site suggested. Needs some work.

##### sphere4:

Was an attempt to load earth.raw and overlay is on a lit green sphere generated using glutSolidSphere(radius,25,25);, but FAILS. A green sphere is there, and I guess there is some error in creating and/or binding the texture...

##### sphere5:

Loads earth.raw, and overlays the texture on a sphere generated internally. A good rotating earth display, with left mouse click zooming.

##### sphere6:

Loads earth.bmp, and overlays the texture on a sphere generated with gluQuadricTexture(quad,1);, gluSphere(quad,m_radius,20,20);. Is a very good rotating earth display.

#### Utilities

The source includes a number of 'utility' libraries.

##### terrutils:

 1. lodepng,[cpp|h] - http://lodev.org/lodepng/ - LodePNG is a PNG image decoder and encoder, all in one, no dependency or linkage to zlib or libpng required.
 2. Terrain.[cpp|h] - Terrain* loadTerrain(const char* filename, float height, bool is_bmp);
 3. vec3f.[cpp|h] - Used by Terrain
 4. texLoad.[c|h] - GLuint LoadTextureRAW( const char * filename ); - loads RAW format file.
 5. imageloader.[cpp|h] - loadPNG(file) - Reads a png image from file, and returns a class Image ptr, an array of the form (R1, G1, B1, R2, G2, B2, ...) indicating the color of each pixel in image. The array starts the bottom-left pixel, the format for OpenGL. AND Image *loadBMP(file), returning the same class Image ptr. Only handles 24-bpp bitmaps. 

##### gltzpr:

http://www.nigels.com/glt/gltzpr/ - A generalised - Left button `->` rotate, Middle button `->` zoom, and Right button `->` pan library. Tried to use it in other cases, but not very successful.

It comes with a zprdemo, which in Windows, sets the windows ICON loaded through a resource file. And has a neat GLERROR macros that looks very useful.

##### genutils:

 1. dir_utils.[cxx|hxx] - `std::vector<std::string> read_directory( std::string dir, std::string mask, int mode, bool verb )` - returns the set of files in the directory, using the mask.
 2. slippystuff.[cxx|hxx] - Some utilities for getting the slippy tile name from lat, lon, zoom - http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
 3. sprtf.[cxx|hxx] - A 'printf' functions that also writes to a log file.
 4. utils.[cxx|hxx] - A mish-mash of utility functions.
 
At present no apps in this collect use any of these general utilities, but that could change!
 
### Building:

This uses the CMake build file generator, so for building, it should be just -

#### for unix/linux

 1. cd build
 2. cmake ..
 3. make
 
#### for windows

 1. cd build
 2. cmake ..
 3. cmake --build . --config Release

The **build** folder contains perhaps a helpful `build-me.bat` file, which demostrates how to add paths to assist cmake find the install location of the various 3rdParty library, particularly GLUT/OpenGLUT. And also a `build-me.sh` script for unix/linux.

And if MSVC supports a 64-bit build, there is also a **build.x64** folder, again with a `build-me.bat` file showing the extra setup required to establish the 64-bit environment. May need adjustment to suit your MSVC install location.
 
Of course the CMake GUI app can also be used. Set the source to the source root, and the binary directory to the build folder. Click `[Configure]`, choose a `generator`. This may need to be repeated until error items cleared, before writing the build files.

And of course the MSVC IDE can be used to loaded the terrain.sln file, if that is the generator chosen.

Have FUN ;=))

Geoff.   
20150503 - 20150406

; eof
