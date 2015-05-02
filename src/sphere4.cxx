/*\
 * sphere4.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <time.h>
#include <GL/glut.h>
#include "texLoad.h"

#ifndef DEF_EARTH_RAW
#define DEF_EARTH_RAW "../data/earth.raw"
#endif

static const char *module = "sphere4";

static GLuint textures[1];
static const char *texture = DEF_EARTH_RAW;

static GLfloat xRotated, yRotated, zRotated;
static GLdouble radius = 1.5;   // 0.5
static GLfloat qaBlack[] = {0.0, 0.0, 0.0, 1.0}; //Black Color
static GLfloat qaGreen[] = {0.0, 1.0, 0.0, 1.0}; //Green Color
static GLfloat qaWhite[] = {1.0, 1.0, 1.0, 1.0}; //White Color
static GLfloat qaRed[] = {1.0, 0.0, 0.0, 1.0}; //White Color

// Set lighting intensity and color
static GLfloat qaAmbientLight[]    = {0.2, 0.2, 0.2, 1.0};
static GLfloat qaDiffuseLight[]    = {0.8, 0.8, 0.8, 1.0};
static GLfloat qaSpecularLight[]    = {1.0, 1.0, 1.0, 1.0};
static GLfloat emitLight[] = {0.9, 0.9, 0.9, 0.01};
static GLfloat Noemit[] = {0.0, 0.0, 0.0, 1.0};

// Light source position
static GLfloat qaLightPosition[]    = {0.5, 0, -3.5, 0.5};

static void display(void);
static void reshape(int x, int y);
 
static int check_clock(void)
{
    static clock_t lastClock = 0;
    static double sDelay = 30.0 / 1000.0;
    int iret = 0;
    clock_t currClock;
    double  duration;

    if (lastClock == 0)
        lastClock = clock();
    currClock = clock(); // elapsed time in seconds times CLOCKS_PER_SEC
    duration = (double)(currClock - lastClock) / CLOCKS_PER_SEC;
    if (duration > sDelay) {
        iret = 1;
        lastClock = currClock;
    }
    return iret;
}

static void idleFunc(void)
{
    if (check_clock()) {
        zRotated += 0.02;
        if (zRotated >= 360.0)
            zRotated -= 360.0;
    }
    //display();
   	glutPostRedisplay();

}

static void initLighting()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set lighting intensity and color
    glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

    // Set the light position
    glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
}

static void display(void)
{

    glMatrixMode(GL_MODELVIEW);
    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT);
    // clear the identity matrix.
    glLoadIdentity();
    
    // traslate the draw by z = -4.0
    // Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
    glTranslatef(0.0,0.0,-5.0);

    // scaling transfomation 
    glScalef(1.0,1.0,1.0);
    glRotatef(zRotated,0.0,0.0,1.0);

    {
        // Set material properties
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, qaGreen);

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, qaGreen);

        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, qaWhite);

        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20);
    }

    if (textures[0]) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
    } 

    // built-in (glut library) function , draw you a sphere.
    glutSolidSphere(radius,25,25);
    // Flush buffers to screen
     
    glFlush();
    glutSwapBuffers();      
    // sawp buffers called because we are using double buffering 
   // glutSwapBuffers();
}

void reshape(int x, int y)
{
    if (y == 0 || x == 0) return;   
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity(); 
    
    gluPerspective(39.0,(GLdouble)x/(GLdouble)y,0.6,21.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  //Use the whole window for rendering
}
  // See more at: http://www.codemiles.com/c-opengl-examples/adding-lighting-to-a-solid-sphere-t9125.html#sthash.MPx7cru0.dpuf

static void keyboard(unsigned char key, int x, int y) 
{
    if (key == 27)
        exit(0);
}

static int initTexture()
{
    int iret = 0;
    textures[0] = LoadTexture( texture );
    if (textures[0] == 0) {
        printf("%s: WARNING: Failed to load texture file '%s'!\n", module, texture );
        iret = 1;
    } else {
        printf("%s: Loaded texture file '%s'!\n", module, texture );
    }
    return iret;
}

// main() OS entry
int main (int argc, char **argv)
{
    glutInit(&argc, argv); 

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(350,350);
    glutCreateWindow("Earth Sphere");
    initLighting(); 
    initTexture();

    xRotated = yRotated = zRotated = 0.0;
    
    glutIdleFunc(idleFunc);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
  	glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
 
  
// eof = sphere4.cxx
