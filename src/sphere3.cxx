/*\
 * sphere3.cxx
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * from : http://stackoverflow.com/questions/25057471/drawing-3d-sphere-in-c-c
\*/

#include <stdio.h>
#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/glut.h>

#include "sphere3.hxx"

static const char *module = "sphere3";

static int win_width, win_height;
/*\
 * Usage
 *
 * LED_cube cube;
 * cube.cls(0x00202020); // clear space to dark gray color
 * int a = cube.n >> 1;      // just place sphere to middle and size almost the whole space
 * int r = a-3;
 * cube.sphere(a,a,a,r,0x00FFFFFF);
 * cube.glDraw();        // just for mine visualization you have to rewrite it to your rendering system
 *
\*/

LED_cube *pc = 0;
static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT); 
    glColor3f(1.0,0.0,0.0); 
    glLoadIdentity(); 
    //glutSolidSphere( 5.0, 20.0, 20.0); 
    //glutSolidSphere( 1.0, 20.0, 20.0); 
    if (pc)
        pc->glDraw();
    glFlush(); 

}

static void keyboard(unsigned char key, int x, int y) 
{
    if (key == 27)
        exit(0);
}

static void myInit() 
{
    //glClearColor(1.0,1.0,1.0,1.0); 
    glClearColor(0.0,0.0,0.0,0.0); 
    glColor3f(1.0,0.0,0.0); 
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 
    //gluOrtho2D(0.0,499.0,0.0,499.0);  // BAD
    //gluOrtho2D(-5.0, 5.0, -5.0, 5.0); // get only equatorial band
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);   // ok, when radius changed to 1.0
    glMatrixMode(GL_MODELVIEW); 
    pc = new LED_cube;
    pc->cls(0x00202020); // clear space to dark gray color
    int a = pc->n >> 1;      // just place sphere to middle and size almost the whole space
    int r = a - 3;
    pc->sphere(a,a,a,r,0x00FFFFFF);

} 

static void reshape(int x, int y)
{
    if (y == 0 || x == 0) return;  //Nothing is visible then, so return
    win_width = x;
    win_height = y;
    //Set a new projection matrix
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();
    //Angle of view:40 degrees
    //Near clipping plane distance: 0.5
    //Far clipping plane distance: 20.0
     
    //gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.5,20.0);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);   // ok, when radius changed to 1.0

    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  //Use the whole window for rendering
}

// main() OS entry
int main( int argc, char **argv )
{
    int iret = 0;
    glutInit(&argc,argv); 
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB); 
    glutInitWindowSize(500,500); 
    glutCreateWindow("sphere3");         
    glutDisplayFunc(display); 
   	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
    myInit(); 
    glutMainLoop(); 
    return iret;
}


// eof = sphere3.cxx
