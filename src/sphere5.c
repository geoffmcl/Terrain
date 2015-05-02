/*\
 * winsphere.c
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * from : http://www.swiftless.com/tutorials/opengl/sphere.html
 * uses : File [earth.raw], 1572864 bytes.
 * That is exactly 1024 * 512 * 3
 * glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
 * 
\*/

#include <stdio.h>
#include <math.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>    // needed for some defines ONLY
#endif
#include <GL/glut.h>
#include "texLoad.h"

#ifdef _MSC_VER
// if needed
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#endif

static const char *module = "sphere5";

#ifndef DEF_EARTH_RAW
#define DEF_EARTH_RAW "../data/earth.raw"
#endif
#ifndef DEF_WIDTH
#define DEF_WIDTH 800   // 500
#endif
#ifndef DEF_HEIGHT
#define DEF_HEIGHT 800  // 500
#endif

/*\
OpenGL/GLUT Notes:

Zooming the model:

First you must define what you mean by "zoom". In the scenario of a perspective projection, 
there are a few possibilities:

1. You change the field of view. This is analogous to the zoom of cameras, where zooming 
   results in changing the focal width.
2. You just scale the model before the projection.
3. You change the distance of the model (nearer to the camera to zoom in, 
   farer away to zoom out)

The variant 1 is by changing the frustum.
Changing eyeZ is approach 3  - used here.

\*/
#define space 1 // 2 // 5 // 10 // must be divisor of 90 and 360
#define VertexCount (90 / space) * (360 / space) * 4

static GLuint textures[1];

static int show_frame_only = 0;
static double m_xrot = 0;
static double m_yrot = 0;
static double m_zrot = 0;

static int add_roll   = 0;  // rotate around x axis
static int add_rotate = 1;  // rotate around y axis
static int add_z_mot  = 0;  // rotate around z axis

static double sDelay = 30.0 / 1000.0;
static double x_increment = 0.2;
static double y_increment = 0.2;
static double z_increment = 0.2;


static int is_frozen = 0;

typedef struct tagVERTICES {
    int X;
    int Y;
    int Z;
    double U;
    double V;
}VERTICES;

const double PI = 3.1415926535897;

VERTICES VERTEX[VertexCount + (space * 4)];

void DisplaySphere (double R, GLuint texture)
{
    int b;
    glScalef (0.0125 * R, 0.0125 * R, 0.0125 * R);
    glRotatef (90, 1, 0, 0);
    glBindTexture (GL_TEXTURE_2D, texture);
    if (show_frame_only) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    glBegin (GL_TRIANGLE_STRIP);
    {
        for ( b = 0; b <= VertexCount; b++) {
            glTexCoord2f (VERTEX[b].U, VERTEX[b].V);
            glVertex3f (VERTEX[b].X, VERTEX[b].Y, -VERTEX[b].Z);
        }
    
        for ( b = 0; b <= VertexCount; b++) {
            glTexCoord2f (VERTEX[b].U, -VERTEX[b].V);
            glVertex3f (VERTEX[b].X, VERTEX[b].Y, VERTEX[b].Z);
        }
    }    
    glEnd();
}

void CreateSphere (double R, double H, double K, double Z) 
{
    int n;
    double a;
    double b;
    
    n = 0;
    for( b = 0; b <= 90 - space; b += space) {
        for( a = 0; a <= 360 - space; a += space) {
            VERTEX[n].X = R * sin((a) / 180 * PI) * sin((b) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a) / 180 * PI) * sin((b) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b) / 180 * PI) - Z;
            VERTEX[n].V = (2 * b) / 360;
            VERTEX[n].U = (a) / 360;
            n++;
            VERTEX[n].X = R * sin((a) / 180 * PI) * sin((b + space) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a) / 180 * PI) * sin((b + space) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b + space) / 180 * PI) - Z;
            VERTEX[n].V = (2 * (b + space)) / 360;
            VERTEX[n].U = (a) / 360;
            n++;
            VERTEX[n].X = R * sin((a + space) / 180 * PI) * sin((b) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a + space) / 180 * PI) * sin((b) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b) / 180 * PI) - Z;
            VERTEX[n].V = (2 * b) / 360;
            VERTEX[n].U = (a + space) / 360;
            n++;
            VERTEX[n].X = R * sin((a + space) / 180 * PI) * sin((b + space) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a + space) / 180 * PI) * sin((b + space) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b + space) / 180 * PI) - Z;
            VERTEX[n].V = (2 * (b + space)) / 360;
            VERTEX[n].U = (a + space) / 360;
            n++;
        }
    }
    printf("%s: Created sphere R=%lf, H=%lf, K=%lf, Z=%lf", module,
        R, H, K, Z );
    printf(", n=%d, v=%d\n", n, VertexCount);
}

// XYZ position of the camera
// the default position of the camera is at gluLookAt(0, 0, 0, 0, 0,-1, 0, 1, 0);
// Specifies the position of the eye point.
static GLdouble eyex = 0.0;
static GLdouble eyey = 0.0;
static GLdouble eyez = 0.0;
// actual vector representing the camera's direction
// Specifies the position of the reference point.
static GLdouble centerx = 0.0;
static GLdouble centery = 0.0;
static GLdouble centerz = -1.0;
// Specifies the direction of the up vector.
static GLdouble upx = 0.0;
static GLdouble upy = 1.0;
static GLdouble upz = 0.0;
static int bButton1Down = 0;
static int yClick;
static void MouseButton(int button, int state, int x, int y)
{
    // Respond to mouse button presses.
    // If button1 pressed, mark this state so we know in motion function.
    if (button == GLUT_LEFT_BUTTON) {
        bButton1Down = (state == GLUT_DOWN) ? 1 : 0;
        yClick = y - (3 * (int)eyez);
    }
}

static char lastchg[264] = {0};
#define DEF_MAX_POS 50.0
#define DEF_MIN_POS -10.0
#define dViewDistance eyez
static int add_zoom_diags = 1;
// try to stop before front of model passes the camera, and we are inside the sphere
// read : https://www.opengl.org/archives/resources/faq/technical/viewing.htm#view0070
static void MouseMotion(int x, int y)
{
    char currchg[264];
    char *cp = currchg;
    char *lp = lastchg;
    // If button1 pressed, zoom in/out if mouse is moved up/down.
    if (bButton1Down) {
        int do_out = 0;
        double cvd = dViewDistance;
        dViewDistance = (y - yClick) / 3.0;
        if (dViewDistance > DEF_MAX_POS) {
            dViewDistance = DEF_MAX_POS;
            sprintf(cp,"VD clamped to max %lf", dViewDistance );
            do_out = 1;
        } else if (dViewDistance < DEF_MIN_POS) {
            dViewDistance = DEF_MIN_POS;
            sprintf(cp,"VD clamped to min %lf", dViewDistance );
            do_out = 1;
        } else {
            if (dViewDistance > cvd) {
                sprintf(cp,"VD increased %lf to %lf", cvd, dViewDistance );
                do_out = 1;
            } else if (dViewDistance > cvd) {
                sprintf(cp,"VD decreased %lf to %lf", cvd, dViewDistance );
                do_out = 1;
            }
        }
        if (add_zoom_diags && do_out) {
            if (strcmp(cp,lp)) {
                strcpy(lp,cp);
                printf("%s\n",lp);
            }
        }
        glutPostRedisplay();
    }
}

void display (void) 
{
    glClearDepth(1);
    glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); 
    // set camera location
    gluLookAt( eyex, eyey, eyez,
        eyex + centerx, 
        eyey + centery, 
        eyez + centerz, 
        upx, upy, upz );

    glTranslatef(0,0,-10);

	glRotatef(m_xrot, 1.0f, 0.0f, 0.0f);   // x
    glRotatef(m_yrot, 0.0f, 1.0f, 0.0f);   // y
    glRotatef(m_zrot, 0.0f, 0.0f, 1.0f);   // z

    DisplaySphere(5, textures[0]);
     
    glutSwapBuffers();

}

void update (void) 
{
    static clock_t lastClock = 0;

    clock_t currClock;
    double  duration;

    if (lastClock == 0)
        lastClock = clock();
    currClock = clock(); // elapsed time in seconds times CLOCKS_PER_SEC
    duration = (double)(currClock - lastClock) / CLOCKS_PER_SEC;

    if (!is_frozen) {
        if (duration > sDelay) {
            lastClock = currClock;
            if (add_rotate) {
                m_yrot += y_increment;
                if (m_yrot >= 360.0)
                    m_yrot = 0.0;
            }
            if (add_roll) {
                m_xrot += x_increment;
                if (m_xrot >= 360.0)
                    m_xrot = 0.0;
            }
            if (add_z_mot) {
                m_zrot += z_increment;
                if (m_zrot >= 360.0)
                    m_zrot = 0.0;

            }
        	glutPostRedisplay();
        }
    }	
}


void init (void) 
{
    glEnable(GL_DEPTH_TEST);
    glEnable( GL_TEXTURE_2D );
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    textures[0] = LoadTexture( DEF_EARTH_RAW );
    CreateSphere(70,0,0,0);
}

void reshape (int w, int h) 
{
    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
    glMatrixMode (GL_MODELVIEW);
}


void keyHelp()
{
    printf("Keyboard help...\n");
    printf(" ESC = Exit\n");
    printf(" ?   = Show this keyboard help.\n");
    //printf(" +   = Increase rotation speed\n");
    //printf(" -   = Decrease rotation speed\n");
    printf(" x   = Toggle m_xrot motion %s\n", add_roll ? "Off" : "On");
    printf(" y   = Toggle y_rotate motion %s\n", add_rotate ? "Off" : "On");
    printf(" z   = Toggle m_zrot motion %s\n", add_z_mot ? "Off" : "On");
    printf(" f   = Toggle freeze %s.\n", is_frozen ? "Off" : "On");
    printf(" R   = Reset angles to original values.\n");
}

void keyboard(unsigned char key, int x, int y) 
{
	switch (key) {
    case 27: //Escape key
		//cleanup();
		exit(0);
        break;
    case '?':
        keyHelp();
        break;
    case 'f':
        if (is_frozen)
            is_frozen = 0;
        else
            is_frozen = 1;
        break;
    case 'R':
        m_yrot  = 0;
        m_xrot = 0;
        m_zrot = 0;
        add_roll   = 0;
        add_rotate = 1;
        add_z_mot  = 0;
        break;
    case 'x':
        if (add_roll)
            add_roll = 0;
        else
            add_roll = 1;
        break;
    case 'y':
        if (add_rotate)
            add_rotate = 0;
        else
            add_rotate = 1;
        break;
    case 'z':
        if (add_z_mot)
            add_z_mot = 0;
        else
            add_z_mot = 1;
        break;
    }
}

static const char *texFile = DEF_EARTH_RAW;
char *get_title(void)
{
    static char buf[264];
    sprintf(buf, "OpenGL Window - %s", texFile);
    return buf;
}
void give_help(void)
{
    printf("Usage: %s [options] [texfile]\n", module);
    printf("Options:\n");
    printf(" --help (-h or -?) = THis help and exit(2)\n");
    printf(" If an input texture file is given, it MUST be RAW format,\n");
    printf(" and exactly 1024 x 512.\n");
    printf(" If no texFile given will use default '%s'\n", texFile);
    printf("\n");
}

int parse_command (int argc, char **argv) 
{
    int c, i;
    char *arg, *sarg;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        c = *arg;
        if (c == '-') {
            sarg = &arg[1];
            while (*sarg == '-')
                sarg++;
            c = *sarg;
            switch (c) {
            case 'h':
            case '?':
                give_help();
                return 2;
                break;
            default:
                give_help();
                printf("%s: Unknown arg '%s'!\n", module, arg);
                return 1;
            }
        } else {
            texFile = strdup(arg);
        }
    }
    return 0;
}

int main (int argc, char **argv) 
{
    int iret = parse_command(argc,argv);
    if (iret) 
        return iret;
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize (DEF_WIDTH, DEF_HEIGHT);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (get_title());

    init();
    
    glutDisplayFunc(display);
    //glutIdleFunc(display);
    glutIdleFunc(update);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);

    keyHelp();

    glutMainLoop ();
    
    return iret;
}
// eof = winsphere.c
