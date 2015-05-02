/*\
 * main01.cpp
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

/*
 * Just some GLU/GLUT notes
 * On resize
 * void gluPerspective(	GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
 * fovy   - Specifies the field of view angle, in degrees, in the y direction.
 * aspect - Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
 * zNear  - Specifies the distance from the viewer to the near clipping plane (always positive).
 * zFar   - Specifies the distance from the viewer to the far clipping plane (always positive).
 *
\*/
#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#ifndef WIN32
#include <string.h> // for strcmp,...
#endif
#include "Terrain.h"
#include "imageloader.h"
#include "vec3f.h"
#ifndef WIN32
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#endif
using namespace std;

static const char *module = "main01";

#ifndef DEF_WIDTH
#define DEF_WIDTH   1024
#endif
#ifndef DEF_HEIGHT
#define DEF_HEIGHT  512
#endif
#ifndef DEF_HEIGHTMAP
#define DEF_HEIGHTMAP "heightmap.bmp"
#endif

static bool add_zoom_diags = false;  // just to see what is happening

static float y_vert = 60.0f;
static float x_horiz = 30.0f;
static float z_depth = 0.0f;

static float _increment = 1.0f;
static float org__angle = 60.0f;
static float org_horizontal = 30.0f;
static float org_increment = 1.0f;
static float _factor = 0.5f;
static float _depth_chg = 0.5f;

static bool add_x_chg = true;
static bool add_y_chg = false;
static bool add_z_chg = false;

static bool is_frozen = false;

static Terrain *_terrain = 0;

void cleanup() {
    if (_terrain)
        delete _terrain;
    _terrain = 0;
}

void keyHelp()
{
    printf(" ESC = Exit\n");
    printf(" +   = Increase rotation speed.\n");
    printf(" -   = Decrease rotation speed.\n");
    printf(" f   = Toggle freeze %s.\n", is_frozen ? "Off" : "On");
    printf(" r   = Reset angles and change to default values.\n");
    printf(" x   = Toggle x motion %s.\n", add_x_chg ? "Off" : "On");
    printf(" y   = Toggle y motion %s.\n", add_y_chg ? "Off" : "On");
    printf(" z   = Toggle z motion %s.\n", add_z_chg ? "Off" : "On");
}

void handleKeypress(unsigned char key, int x, int y) 
{
    int do_redisp = 0;
	switch (key) {
    case 27: //Escape key
		cleanup();
		exit(0);
        break;
    case '+':
        _increment += _factor;
        if (_increment > 90.0f)
            _increment = 90.0f;
        printf("%s: Increase increment to %f\n", module, _increment);
        break;
    case '-':
        if (_increment > _factor) {
            _increment -= _factor;
        } else {
            _increment = 0.0f;
        }
        printf("%s: Decreased increment to %f\n", module, _increment);
        break;
    case 'f':
        if (is_frozen)
            is_frozen = false;
        else
            is_frozen = true;
        break;
    case '?':
        keyHelp();
        break;
    case 'r':
        do_redisp = 1;
        y_vert = org__angle;
        x_horiz = org_horizontal;
        _increment = org_increment;
        z_depth = 0;
        add_y_chg = false;
        add_x_chg = true;
        add_z_chg = false;
        printf("%s: Done reset. x %s, y %s, z %s\n", module,
            (add_x_chg ? "on" : "off"),
            (add_y_chg ? "on" : "off"),
            (add_z_chg ? "on" : "off"));
        break;
    case 'x':
        if (add_x_chg)
            add_x_chg = false;
        else
            add_x_chg = true;
        printf("%s: Toggled x change %s\n", module, 
            (add_x_chg ? "on" : "off"));
        break;
    case 'y':
        if (add_y_chg)
            add_y_chg = false;
        else
            add_y_chg = true;
        printf("%s: Toggled y change %s\n", module, 
            (add_y_chg ? "on" : "off"));
        break;
    case 'z':
        if (add_z_chg)
            add_z_chg = false;
        else
            add_z_chg = true;
        printf("%s: Toggled z change %s\n", module, 
            (add_z_chg ? "on" : "off"));
        break;
	}
    if (do_redisp) 
    	glutPostRedisplay();
        
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

#define VIEWING_DISTANCE_MIN  3.0
static BOOL g_bButton1Down = FALSE;
static int g_yClick = 0;
static GLdouble g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
// XYZ position of the camera
// float x=0.0f, z=5.0f, y = 1.75f;
// Specifies the position of the eye point.
GLdouble eyex = 0.0;
GLdouble eyey = 0.0;
//GLdouble eyez = 9;
#define eyez g_fViewDistance
// actual vector representing the camera's direction
// Specifies the position of the reference point.
GLdouble centerx = 0.0;
GLdouble centery = 0.0;
GLdouble centerz = -1.0;
// Specifies the direction of the up vector.
GLdouble upx = 0.0;
GLdouble upy = 1.0;
GLdouble upz = 0.0;

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    gluLookAt( eyex, eyey, eyez,
        eyex + centerx, 
        eyey + centery, 
        eyez + centerz, 
        upx, upy, upz );

	glTranslatef(0.0f, 0.0f, -10.0f);

	glRotatef(x_horiz, 1.0f, 0.0f, 0.0f);
	glRotatef(-y_vert, 0.0f, 1.0f, 0.0f);
	glRotatef(z_depth, 0.0f, 0.0f, 1.0f);
	
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(_terrain->width() - 1) / 2,
				 0.0f,
				 -(float)(_terrain->length() - 1) / 2);
	
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < _terrain->width(); x++) {
			Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}
	
	glutSwapBuffers();
}

void update(int value) 
{
    if (!is_frozen) {
        if (add_y_chg) {
	        y_vert += _increment;
	        if (y_vert > 360) {
		        y_vert -= 360;
	        }
        }
        if (add_x_chg) {
            x_horiz += _increment;
            if (x_horiz > 360) 
                x_horiz -= 360;
        }
        if (add_z_chg) {
            z_depth += _increment;
            if (z_depth > 360) 
                z_depth -= 360;
        }
    	glutPostRedisplay();
    }	
	glutTimerFunc(25, update, 0);
}

static void MouseButton(int button, int state, int x, int y)
{
    // Respond to mouse button presses.
    // If button1 pressed, mark this state so we know in motion function.
    if (button == GLUT_LEFT_BUTTON) {
        g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
        g_yClick = y - (3 * g_fViewDistance);
    }
}

static char lastchg[264] = {0};
#define DEF_MAX_POS 50.0
#define DEF_MIN_POS -10.0

static void MouseMotion(int x, int y)
{
    char currchg[264];
    char *cp = currchg;
    char *lp = lastchg;
    // If button1 pressed, zoom in/out if mouse is moved up/down.
    if (g_bButton1Down) {
        bool do_out = false;
        double cvd = g_fViewDistance;
        g_fViewDistance = (y - g_yClick) / 3.0;
        if (g_fViewDistance > DEF_MAX_POS) {
            g_fViewDistance = DEF_MAX_POS;
            sprintf(cp,"VD clamped to max %lf", g_fViewDistance );
            do_out = true;
        } else if (g_fViewDistance < DEF_MIN_POS) {
            g_fViewDistance = DEF_MIN_POS;
            sprintf(cp,"VD clamped to min %lf", g_fViewDistance );
            do_out = true;
        } else {
            if (g_fViewDistance > cvd) {
                sprintf(cp,"VD increased %lf to %lf\n", cvd, g_fViewDistance );
                do_out = true;
            } else if (g_fViewDistance > cvd) {
                sprintf(cp,"VD decreased %lf to %lf\n", cvd, g_fViewDistance );
                do_out = true;
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


static const char *hmf = DEF_HEIGHTMAP;

void give_help(char *name)
{
    printf("%s [options] [in_heightmap]\n");
    printf("options:\n");
    printf(" --help (-h or -?) = This help and exit 2\n");
    printf("\n");
    printf(" If no bmp, or png file given, will use def '%s'\n", hmf);
}

int parse_command(int argc, char** argv) 
{
    int i, c;
    char *arg, *sarg;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        if (*arg == '-') {
            sarg = &arg[1];
            while (*sarg == '-') sarg++;
            c = *sarg;
            switch(c) {
            case 'h':
            case '?':
                give_help(argv[0]);
                return 2;
            default:
                printf("Unknown argument '%s'\n", arg );
                return 1;
            }
        } else {
            hmf = strdup(arg);
        }
    }
    return 0;
}

char *get_title()
{
    static char buf[264];
    char *cp = buf;
    sprintf(cp,"Terrain - %s", hmf );
    return cp;
}

//
// SAMPLE PNG: ..\data\0-512x256.png
// SAMPLE BMP: ..\data\heightmap.bmp (default)
// SAMPLE PNG: ..\data\heightmap.png
//
// IFF freeglut.DLL is used, may need,
// PATH=F:\Projects\software\bin;%PATH%
// but the default is the search for the 'static' first
int main(int argc, char** argv) 
{
    int iret = 0;
    bool is_bmp = true;
    iret = parse_command( argc, argv );
    if (iret)
        return iret;

    if (isBMP(hmf))
        is_bmp = true;
    else if (isPNG(hmf))
        is_bmp = false;
    else {
        printf("Error: File '%s' does NOT appear to be a BMP, nor PNG file!\n", hmf );
        return 1;
    }

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(DEF_WIDTH, DEF_HEIGHT);
	
	glutCreateWindow(get_title());

	initRendering();
	
	_terrain = loadTerrain(hmf, 20, is_bmp);
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);

    printf("Keyboard keys...\n");
    keyHelp();

	glutTimerFunc(25, update, 0);   // movement timer
	
	glutMainLoop();
	return 0;
}

// eof
