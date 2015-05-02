# Multilayered Terrain

I was looking for example of overlaying a heightmap with a texture, and came across this
great sample - http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=24

It was a LOT more than I wanted, but decided to dig into it, and see what could be 
discovered. At present it only has a 'Windows' (WIN32) interface, but could maybe be 
extended.

Also, unfortunately it has quite a LONG list of dependencies -

 1. GLUT/OpenGLUT - http://openglut.sourceforge.net/
 2. GLEW - http://glew.sourceforge.net/
 3. GLM - https://sourceforge.net/projects/ogl-math/files/latest/download
 4. FreeType - http://www.freetype.org/download.html
 5. FreeImage - http://freeimage.sourceforge.net/download.html
 6. Assimp - http://assimp.sourceforge.net/main_downloads.html

And, of course, GL in general.

Plus a considerable list of 'data' objects, all of which can be found in the zip downloaded 
from the above tutorial. Some of which are - only 1 example from each folder -

 1. data\models\house\house.3ds
 2. data\models\Wolf\Wolf.obj
 3. data\shaders\color.frag
 4. data\skyboxes\elbrus\elbrus_back.jpg
 5. data\textures\path.png
 6. data\worlds\consider_this_question.bmp

To run ALL this data MUST be found.

To be able to 'find' this data, I added a command line parameter giving the 'root' of 
the 'data' objects...

Have FUN!

Geoff.  
20150406

; eof
