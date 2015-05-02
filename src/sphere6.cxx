/*\
 * sphere5.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * from : http://www.codeincodeblock.com/2012/05/simple-method-for-texture-mapping-on.html
 *
\*/

#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include "imageloader.h"

using namespace std;

#ifndef DEF_EARTH_BMP
#define DEF_EARTH_BMP "../data/earth.bmp"
#endif

static const char *earth_bmp = DEF_EARTH_BMP;
static GLdouble m_radius = 5.0; // 4.0; // 2.0
static GLuint _textureId; // The id of the texture
static GLUquadric *quad = 0;
static float m_rotate = 0.0;
//GLUquadricObj quad;
static unsigned int m_mstime = 25;  // 25
static float m_increment = 0.2f;    // 1.0f;    // 2.0f
static float m_xrot = -90.0f;       // 90.0f
static float m_yrot =  23.4f;

// try to center in the window
static GLfloat x_trans = 0.0f;
static GLfloat y_trans = 0.0f;  // 1.0f;
static GLfloat z_trans = -16.0f;

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) 
{
    GLuint textureId;

    glGenTextures(1, &textureId); //Make room for our texture

    glBindTexture(GL_TEXTURE_2D, textureId); // Tell OpenGL which texture to edit

    // Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,        //Always GL_TEXTURE_2D
         0,                            //0 for now
         GL_RGB,                       //Format OpenGL uses for image
         image->width, image->height,  //Width and height
         0,                            //The border of the image
         GL_RGB, //GL_RGB, because pixels are stored in RGB format
         GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored as unsigned numbers
         image->pixels);               //The actual pixel data

    return textureId; //Returns the id of the texture

}

static void initRendering() 
{
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    quad = gluNewQuadric();

    Image *image = loadBMP(earth_bmp);
    
    _textureId = loadTexture(image);

    delete image;
}


static void handleResize(int w, int h) 
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);

}

static void drawScene() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();


    glTranslatef(x_trans, y_trans, z_trans);    // was 0.0f, 1.0f, -16.0f);

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, _textureId);

    //Bottom
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glRotatef(m_xrot  , 1.0f, 0.0f, 0.0f );  // x
    glRotatef(m_yrot  , 0.0f, 1.0f, 0.0f );  // y
    glRotatef(m_rotate, 0.0f, 0.0f, 1.0f );  // z

    gluQuadricTexture(quad,1);

    gluSphere(quad, m_radius, 20, 20 );

    glutSwapBuffers();

}

static void update(int value)
{

    m_rotate += m_increment;
    if(m_rotate > 360.f) {
        m_rotate-=360;
    }

    glutPostRedisplay();

    glutTimerFunc(m_mstime, update, 0);

}

void handleKeypress(unsigned char key, int x, int y) 
{
    switch (key) {
    case 27: //Escape key
       exit(0);
    }
}

// OS entry
int main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    //glutCreateWindow("Textures-codeincodeblock.blogspot.com");
    glutCreateWindow("theEarth");
    initRendering();
    glutTimerFunc(m_mstime,update,0);
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMainLoop();
    return 0;
}


// eof = sphere5.cxx
