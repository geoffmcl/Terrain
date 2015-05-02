///////////////////////////////////////////////////////////////////
///
#ifdef WIN32
#include <Windows.h>
#include "resource.h"
#endif
#include <stdio.h>
#include "zpr.h"

/* Macro for checking OpenGL error state */

#define GLERROR                                                    \
    {                                                              \
        GLenum code = glGetError();                                \
        while (code!=GL_NO_ERROR)                                  \
        {                                                          \
            printf("%s\n",(char *) gluErrorString(code));          \
                code = glGetError();                               \
        }                                                          \
    }

/* Draw axes */

void drawAxes(void)
{
    /* Name-stack manipulation for the purpose of
       selection hit processing when mouse button
       is pressed.  Names are ignored in normal
       OpenGL rendering mode.                    */

   glPushMatrix();
                                /* No name for grey sphere */

      glColor3f(0.3,0.3,0.3);
      glutSolidSphere(0.7, 20, 20);

      glPushMatrix();
      glPushName(1);            /* Red cone is 1 */
         glColor3f(1,0,0);
         glRotatef(90,0,1,0);
         glutSolidCone(0.6, 4.0, 20, 20);
      glPopName();
      glPopMatrix();

      glPushMatrix ();
      glPushName(2);            /* Green cone is 2 */
         glColor3f(0,1,0);
         glRotatef(-90,1,0,0);
         glutSolidCone(0.6, 4.0, 20, 20);
      glPopName();
      glPopMatrix();

      glColor3f(0,0,1);         /* Blue cone is 3 */
      glPushName(3);
         glutSolidCone(0.6, 4.0, 20, 20);
      glPopName();

   glPopMatrix();
}

/* Callback function for drawing */

void display(void)
{
   GLERROR;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   drawAxes();
   glutSwapBuffers();

   GLERROR;
}

/* Callback function for pick-event handling from ZPR */

void pick(GLint name)
{
    static GLint last = 0;
    if (name != last) {
       printf("Pick: %d\n",name);
       fflush(stdout);
       last = name;
    }
}

static GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

static GLfloat mat_ambient[]    = { 0.7, 0.7, 0.7, 1.0 };
static GLfloat mat_diffuse[]    = { 0.8, 0.8, 0.8, 1.0 };
static GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat high_shininess[] = { 100.0 };

static void help(void)
{
    printf("Help\n");
    printf(" ESC = Exit\n");
    printf(" ?   = THis help\n");
    printf(" Left Mouse - pick and rotate model\n");
    printf(" Center Mouse - zomm in and our\n");
    printf(" Right Mouse - pan model\n");
}

static void handleKeypress(unsigned char key, int x, int y) 
{
    int do_redisp = 0;
	switch (key) {
    case 27: //Escape key
		//cleanup();
		exit(0);
        break;
    case '?':
        help();
        break;
    }
}

void setWindowIcon( const char *title, const char *name )
{
#ifdef WIN32
#ifndef GWL_HINSTANCE
#define GWL_HINSTANCE (-6)
#endif
    HWND hwnd = FindWindow(NULL, title); //probably you can get the window handler in different way.. 
    if (hwnd) {
        SIZE smallIconSize = { GetSystemMetrics(SM_CXSMICON),
                                GetSystemMetrics(SM_CYSMICON) };
#if (defined(_WIN64) || defined(WIN64))
        // Did NOT seem to work in a 64-bit build???? Returns ZERO
        // so changed to this GetModuleHandle(name)
        // could maybe use this for BOTH cases???
        HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(name);
#else
        HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
#endif
        HICON icon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(GLUT_ICON),
                                        IMAGE_ICON,
                                        smallIconSize.cx, smallIconSize.cy,
                                        0);
        if (icon) {
            SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
        }
    }
#else
    // TODO: how to do this in NOT windows
#endif // WIN32
}

/* Entry point - console app, with OpenGLUT windows */
static const char *title = "GLT Mouse Zoom-Pan-Rotate";
int main(int argc, char *argv[])
{
    /* Initialise GLUT and create a window */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400,400);
    glutCreateWindow(title);
    setWindowIcon(title, argv[0]);   // add an ICON to the window

    /* Configure GLUT callback functions */

    glutDisplayFunc(display);

    glScalef(0.25,0.25,0.25);

    /* Configure ZPR module */

    zprInit();
    zprSelectionFunc(drawAxes);     /* Selection mode draw function */
    zprPickFunc(pick);              /* Pick event client callback   */

     /* Initialise OpenGL */

    GLERROR;

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

   	glutKeyboardFunc(handleKeypress);


    GLERROR;

    help(); // show help

    /* Enter GLUT event loop */

    glutMainLoop();

    return 0;
}

