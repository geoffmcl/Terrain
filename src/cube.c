//////////////////////////////////////////////////////////////////////////////////////////
// cube.c
// manually drawn colored cube
// from : http://www.codemiles.com/c-opengl-examples/draw-3d-cube-using-opengl-t9018.html
//////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <time.h>
#include <GL\glut.h>

GLfloat xRotated = 0.0f;
GLfloat yRotated = 0.0f;
GLfloat zRotated = 0.0f;

GLfloat xInc = 0.3f;
GLfloat yInc = 0.2f;
GLfloat zInc = 0.1f;

void init(void)
{
    glClearColor(0,0,0,0);
}

void DrawCube(void)
{
     glMatrixMode(GL_MODELVIEW);
    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0,0.0,-10.5);

    // rotation about X axis
    glRotatef(xRotated,1.0,0.0,0.0);
    // rotation about Y axis
    glRotatef(yRotated,0.0,1.0,0.0);
    // rotation about Z axis
    glRotatef(zRotated,0.0,0.0,1.0);

      glBegin(GL_QUADS);        // Draw The Cube Using quads

        glColor3f(0.0f,1.0f,0.0f);    // 1: Color Blue
        glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Top)
        glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Top)
        glVertex3f(-1.0f, 1.0f, 1.0f);    // Bottom Left Of The Quad (Top)
        glVertex3f( 1.0f, 1.0f, 1.0f);    // Bottom Right Of The Quad (Top)

        glColor3f(1.0f,0.5f,0.0f);    // 2: Color Orange
        glVertex3f( 1.0f,-1.0f, 1.0f);    // Top Right Of The Quad (Bottom)
        glVertex3f(-1.0f,-1.0f, 1.0f);    // Top Left Of The Quad (Bottom)
        glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Bottom)
        glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Bottom)

        glColor3f(1.0f,0.0f,0.0f);    // 3: Color Red    
        glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Front)
        glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Front)
        glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Front)
        glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Front)

        glColor3f(1.0f,1.0f,0.0f);    // 4: Color Yellow
        glVertex3f( 1.0f,-1.0f,-1.0f);    // Top Right Of The Quad (Back)
        glVertex3f(-1.0f,-1.0f,-1.0f);    // Top Left Of The Quad (Back)
        glVertex3f(-1.0f, 1.0f,-1.0f);    // Bottom Left Of The Quad (Back)
        glVertex3f( 1.0f, 1.0f,-1.0f);    // Bottom Right Of The Quad (Back)

        glColor3f(0.0f,0.0f,1.0f);    // 5: Color Blue
        glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Left)
        glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Left)
        glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Left)
        glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Left)

        glColor3f(1.0f,0.0f,1.0f);    // 6: Color Violet
        glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Right)
        glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Right)
        glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Right)
        glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Right)

      glEnd();            // End Drawing The Cube
    glFlush();
}

int check_clock(void)
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

#define MX_FPS 16
void animation(void)
{
    static int framecnt[MX_FPS];
    static int currFrame = 0;
    static time_t currSec, lastSec = 0;
    static int frames = 0;

    frames++;
    if (check_clock()) {
        xRotated += xInc;
        if (xRotated >= 360.0f)
            xRotated = 0.0f;

        yRotated += yInc;
        if (yRotated >= 360.0f)
            yRotated = 0;

        zRotated += zInc;
        if (zRotated >= 360.0f)
            zRotated = 0;

    }
    DrawCube();
    currSec = time(0);
    if (currSec != lastSec) {
        lastSec = currSec;
        framecnt[currFrame++] = frames;
        frames = 0;
        if (currFrame >= MX_FPS) {
            int i, total = 0;
            currFrame = 0;
            for (i = 0; i < MX_FPS; i++)
                total += framecnt[i];
            total /= MX_FPS;
            printf("Av. FPS %d\n", total);
        }
    }
}


void reshape(int x, int y)
{
    if (y == 0 || x == 0) return;  //Nothing is visible then, so return
    //Set a new projection matrix
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();
    //Angle of view:40 degrees
    //Near clipping plane distance: 0.5
    //Far clipping plane distance: 20.0
     
    gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.5,20.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  //Use the whole window for rendering
}

static void handleKeypress(unsigned char key, int x, int y) 
{
    int do_redisp = 0;
	switch (key) {
    case 27: //Escape key
		//cleanup();
		exit(0);
        break;
    }
}

int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	//we initizlilze the glut. functions
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Manual Cube");
	init();
	glutDisplayFunc(DrawCube);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(handleKeypress);
	//Set the function for the animation.
	glutIdleFunc(animation);
	glutMainLoop();
	return 0;
}

// See more at: http://www.codemiles.com/c-opengl-examples/draw-3d-cube-using-opengl-t9018.html#sthash.Cd4xgGPG.dpuf

// eof

