/*\
 * Terrain2-scraps.cpp
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * Just some code scraps not used but may be useful somewhere, sometime
 * especially the crossproduct, scalarproduct, normalize, etc... 
 * and draw_strokestring, ...
 * 
\*/

#include <stdio.h>
// other includes

static const char *module = "Terrain2-scraps";

#if 0 // 000000000000000000000000000000000000000000000000000000
// this did not look so good
static float ORG[3] = {X_CENT,Y_CENT,Z_CENT};

static float XP[3] = {1,0,0};
static float XN[3] = {-1,0,0};
static float YP[3] = {0,1,0};
static float YN[3] = {0,-1,0};
static float ZP[3] = {0,0,1};
static float ZN[3] = {0,0,-1};

static void Draw_Axes()
{
    glPushMatrix ();

    //glTranslatef (-2.4, -1.5, -5);
    glRotatef (x_axis, 1,0,0);
    glRotatef (y_axis, 0,1,0);
    glRotatef (z_axis, 0,0,1);

    glScalef (4.25, 4.25, 4.25);

    glLineWidth (2.0);

    glBegin (GL_LINES);

        glColor3f (1,0,0); // X axis is red.
        glVertex3fv (ORG);
        glVertex3fv (XP ); 

        glColor3f (0,1,0); // Y axis is green.
        glVertex3fv (ORG);
        glVertex3fv (YP );

        glColor3f (0,0,1); // z axis is blue.
        glVertex3fv (ORG);
        glVertex3fv (ZP ); 
    glEnd();

    glPopMatrix ();
}
#endif // 00000000000000000000000000000000000000000000000

#if 0 // 0000000000000000000000000000000000000000000000000

// is OK, but chose simpler implmentation
#define RADPERDEG 0.0174533

void Arrow(GLdouble x1, GLdouble y1, GLdouble z1,
           GLdouble x2, GLdouble y2, GLdouble z2,
           GLdouble D)
{
    double x = x2-x1;
    double y = y2-y1;
    double z = z2-z1;
    double L = sqrt(x*x + y*y + z*z);

    GLUquadricObj *quadObj;

    glPushMatrix ();

      glTranslated(x1,y1,z1);

      if( (x != 0.0) || (y != 0.0) ) {
            glRotated(atan2(y,x)/RADPERDEG,0.,0.,1.);
            glRotated(atan2(sqrt(x*x+y*y),z)/RADPERDEG,0.,1.,0.);
      } else if ( z < 0 ) {
            glRotated(180,1.,0.,0.);
      }

      glTranslatef(0,0,L-4*D);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluCylinder(quadObj, 2*D, 0.0, 4*D, 32, 1);
      gluDeleteQuadric(quadObj);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluDisk(quadObj, 0.0, 2*D, 32, 1);
      gluDeleteQuadric(quadObj);

      glTranslatef(0,0,-L+4*D);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluCylinder(quadObj, D, D, L-4*D, 32, 1);
      gluDeleteQuadric(quadObj);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluDisk(quadObj, 0.0, D, 32, 1);
      gluDeleteQuadric(quadObj);

    glPopMatrix ();

}

double diameter = 6.0;  // 3.0;  // 2.0;  // 0.2;

void drawAxes(GLdouble length)
{
    glPushMatrix();
    glColor3f (1,0,0); // X axis is red.
    glTranslatef(-length,0,0);
    Arrow( 0, 0, 0, 2*length, 0, 0, diameter);
    glPopMatrix();

    glPushMatrix();
    glColor3f (0,1,0); // Y axis is green.
    glTranslatef(0,-length,0);
    Arrow( 0, 0, 0, 0, 2*length, 0, diameter);
    glPopMatrix();

    glPushMatrix();
    glColor3f (0,0,1); // Z axis is blue.
    glTranslatef(0,0,-length);
    Arrow( 0, 0, 0, 0, 0, 2*length, diameter);
    glPopMatrix();
}
#endif // 00000000000000000000000000000000000000000000

#if 0 // 0000000000000000000000000000000000000000000000000000000
// NOT TRIED! looks far too complicated!!!
// from : http://stackoverflow.com/questions/19332668/drawing-the-axis-with-its-arrow-using-opengl-in-visual-studio-2010-and-c
// from : https://github.com/datenwolf/codesamples/blob/master/samples/OpenGL/frustum/frustum.c
/* == basic Q^3 vector math functions == */

static void crossproduct(
	double ax, double ay, double az,
	double bx, double by, double bz,
	double *rx, double *ry, double *rz )
{
	*rx = ay*bz - az*by;
	*ry = az*bx - ax*bz;
	*rz = ax*by - ay*bx;
}

static void crossproduct_v(
	double const * const a,
	double const * const b,
	double * const c )
{
	crossproduct(
		a[0], a[1], a[2],
		b[0], b[1], b[2],
		c, c+1, c+2 );
}

static double scalarproduct(
	double ax, double ay, double az,
	double bx, double by, double bz )
{
	return ax*bx + ay*by + az*bz;
}

static double scalarproduct_v(
	double const * const a,
	double const * const b )
{
	return scalarproduct(
		a[0], a[1], a[2],
		b[0], b[1], b[2] );
}

static double length(
	double ax, double ay, double az )
{
	return sqrt(
		scalarproduct(
			ax, ay, az,
			ax, ay, az ) );
}

static double length_v( double const * const a )
{
	return sqrt( scalarproduct_v(a, a) );
}

//static double normalize(
static void normalize(
	double *x, double *y, double *z)
{
	double const k = 1./length(*x, *y, *z);

	*x *= k;
	*y *= k;
	*z *= k;
}

//static double normalize_v( double *a )
static void normalize_v( double *a )
{
	double const k = 1./length_v(a);
	a[0] *= k;
	a[1] *= k;
	a[2] *= k;
}

/* == annotation drawing functions == */

void draw_strokestring(void *font, float const size, char const *string)
{
	glPushMatrix();
	float const scale = size * 0.01; /* GLUT character base size is 100 units */
	glScalef(scale, scale, scale);

	char const *c = string;
	for(; c && *c; c++) {
		glutStrokeCharacter(font, *c);
	}
	glPopMatrix();
}

void draw_arrow(
    float ax, float ay, float az,  /* starting point in local space */
    float bx, float by, float bz,  /* starting point in local space */
    float ah, float bh,            /* arrow head size start and end */
    char const * const annotation, /* annotation string */
    float annot_size               /* annotation string height (local units) */ )
{
    //int i;

    GLdouble mv[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, mv);

    /* We're assuming the modelview RS part is (isotropically scaled)
     * orthonormal, so the inverse is the transpose.
     * The local view direction vector is the 3rd column of the matrix;
     * assuming the view direction to be the normal on the arrows tangent
     * space  taking the cross product of this with the arrow direction
     * yields the binormal to be used as the orthonormal base to the 
     * arrow direction to be used for drawing the arrowheads */

    double d[3] = {
          bx - ax,
          by - ay,
          bz - az
    };
    normalize_v(d);

    double r[3] = { mv[0], mv[4], mv[8] };
    int rev = scalarproduct_v(d, r) < 0.;

    double n[3] = { mv[2], mv[6], mv[10] };
    {
        double const s = scalarproduct_v(d,n);
        for(int i = 0; i < 3; i++)
            n[i] -= d[i]*s;
    }
    normalize_v(n);

    double b[3];
    crossproduct_v(n, d, b);

    /* Make a 60° arrowhead ( sin(60°) = 0.866... ) */
    GLfloat const pos[][3] = {
        {ax, ay, az},
        {bx, by, bz},
        { ax + (0.866*d[0] + 0.5*b[0])*ah,
          ay + (0.866*d[1] + 0.5*b[1])*ah,
          az + (0.866*d[2] + 0.5*b[2])*ah },
        { ax + (0.866*d[0] - 0.5*b[0])*ah,
          ay + (0.866*d[1] - 0.5*b[1])*ah,
          az + (0.866*d[2] - 0.5*b[2])*ah },
        { bx + (-0.866*d[0] + 0.5*b[0])*bh,
          by + (-0.866*d[1] + 0.5*b[1])*bh,
          bz + (-0.866*d[2] + 0.5*b[2])*bh },
        { bx + (-0.866*d[0] - 0.5*b[0])*bh,
          by + (-0.866*d[1] - 0.5*b[1])*bh,
          bz + (-0.866*d[2] - 0.5*b[2])*bh }
    };
    GLushort const idx[][2] = {
        {0, 1},
        {0, 2}, {0, 3},
        {1, 4}, {1, 5}
    };
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, pos);

    glDrawElements(GL_LINES, 2*5, GL_UNSIGNED_SHORT, idx);
    glDisableClientState(GL_VERTEX_ARRAY);

    if(annotation) {
        float w = 0;
        for(char const *c = annotation; *c; c++)
            w += glutStrokeWidth(GLUT_STROKE_ROMAN, *c);
        w *= annot_size / 100.;

        float tx = (ax + bx)/2.;
        float ty = (ay + by)/2.;
        float tz = (az + bz)/2.;

        GLdouble r[16] = {
            d[0], d[1], d[2], 0,
            b[0], b[1], b[2], 0,
            n[0], n[1], n[2], 0,
               0,    0,    0, 1
        };
        glPushMatrix();
        glTranslatef(tx, ty, tz);
        glMultMatrixd(r);
        if(rev)
            glScalef(-1, -1, 1);
        glTranslatef(-w/2., annot_size*0.1, 0);
        draw_strokestring(GLUT_STROKE_ROMAN, annot_size, annotation);
        glPopMatrix();
    }
}

#endif // 000000000000000000000000000000000000000000000000


void show_module()
{
    printf("Module %s\n", module);
}
// eof = Terrain2-scraps.cpp
