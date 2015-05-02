# LodePNG

This beautiful source came from http://lodev.org/lodepng/, repo https://github.com/lvandeve/lodepng.

As the homepage states "LodePNG is a PNG image decoder and encoder, all in one, no dependency or linkage to zlib or libpng required. It's made for C (ISO C90), and has a C++ wrapper with a more convenient interface on top.

The important fact being that is decodes PNG data without any dependency on libPNG nor ZLIB, making it easy to include in an app. Just two files loadpng.cpp and loadpng.h...

As can be seen in say the example_png2bmp.cpp in the original source, it decode into a vector `  std::vector<unsigned char> image; //the raw pixels` in 24-bit RGB format.

See LICENSE.txt for licence details.

20150422

; eof
