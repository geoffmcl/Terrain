/*\
 * texLoad.h
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _TEXLOAD_H_
#define _TEXLOAD_H_
#ifdef WIN32
#include <windows.h>    // needed for some defines ONLY
#endif
#include <GL/glut.h>

#ifdef __cplusplus
extern "C" {
#endif

extern GLuint LoadTextureRAW( const char * filename );
extern GLuint GenMipmap( int width, int height, unsigned char *data );
extern GLuint LoadTexture( const char * filename );

extern size_t get_file_size( FILE *file );

#ifdef __cplusplus
}
#endif

#endif // #ifndef _TEXLOAD_H_
// eof - texLoad.h
