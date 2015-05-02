/*\
    from : http://stackoverflow.com/questions/5988686/creating-a-3d-sphere-in-opengl-using-visual-c
\*/

#include <vector>
#include <math.h>
#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/glut.h> 

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

// Some more from the site
static int win_width, win_height;
//static std::vector<GLfloat> vertices;
//static std::vector<GLfloat> normals;
//static std::vector<GLfloat> texcoords;
//static std::vector<GLushort> indices;

class SolidSphere
{
protected:
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;

public:
    SolidSphere(float radius, unsigned int rings, unsigned int sectors)
    {
        float const R = 1./(float)(rings-1);
        float const S = 1./(float)(sectors-1);
        unsigned int r, s;

        vertices.resize(rings * sectors * 3);
        normals.resize(rings * sectors * 3);
        texcoords.resize(rings * sectors * 2);
        std::vector<GLfloat>::iterator v = vertices.begin();
        std::vector<GLfloat>::iterator n = normals.begin();
        std::vector<GLfloat>::iterator t = texcoords.begin();
        for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
                float const y = sin( -M_PI_2 + M_PI * r * R );
                float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
                float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

                *t++ = s*S;
                *t++ = r*R;

                *v++ = x * radius;
                *v++ = y * radius;
                *v++ = z * radius;

                *n++ = x;
                *n++ = y;
                *n++ = z;
        }

        indices.resize(rings * sectors * 4);
        std::vector<GLushort>::iterator i = indices.begin();
        for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++) {
                *i++ = r * sectors + s;
                *i++ = r * sectors + (s+1);
                *i++ = (r+1) * sectors + (s+1);
                *i++ = (r+1) * sectors + s;
        }
    }

    void draw(GLfloat x, GLfloat y, GLfloat z)
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(x,y,z);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
        glNormalPointer(GL_FLOAT, 0, &normals[0]);
        glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
        glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
        glPopMatrix();
    }
};

// SolidSphere sphere(1, 12, 24);
static SolidSphere *psp = 0;

void display_sphere()
{
    //int const win_width  = …; // retrieve window dimensions from
    //int const win_height = …; // framework of choice here
    float const win_aspect = (float)win_width / (float)win_height;

    glViewport(0, 0, win_width, win_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, win_aspect, 1, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#ifdef DRAW_WIREFRAME
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
    if (psp) {
        //sphere.draw(0, 0, -5);
        psp->draw(0,0,-5);
    }

    //swapBuffers();
    glFlush(); 

}


class vec3 
{
public:
    vec3(GLfloat x, GLfloat y, GLfloat z) { _x = x; _y = y; _z = z; }
    GLfloat _x,_y,_z;
    //vec3 & operator *= (GLfloat r) {  _x * r; _y * r; _z * r; }
    //vec3 & operator * (vec3 v, GLfloat r) {  v._x * r; v._y * r; v._z * r; }
    //friend vec3 & operator * (vec3 v, GLfloat r) {  v._x * r; v._y * r; v._z * r; return v; }
};

vec3 & operator * (vec3 &v, GLfloat &r) {  v._x * r; v._y * r; v._z * r; return v; }
//vec3 & operator * (vec3 v, GLfloat r) {  v._x * r; v._y * r; v._z * r; return v; }
//vec3 & operator * (vec3 v, GLfloat r) {  v._x * r; v._y * r; v._z * r; return *v; }
//vec3 & operator (vec3 v, GLfloat r) {  v._x * r; v._y * r; v._z * r; return v; }

class vec2
{
public:
    vec2( GLfloat x, GLfloat y ) { _x = x; _y = y; }
    GLfloat _x,_y;
};

static void push_indices(std::vector<GLushort>& indices, int sectors, int r, int s) {
    int curRow = r * sectors;
    int nextRow = (r+1) * sectors;

    indices.push_back(curRow + s);
    indices.push_back(nextRow + s);
    indices.push_back(nextRow + (s+1));

    indices.push_back(curRow + s);
    indices.push_back(nextRow + (s+1));
    indices.push_back(curRow + (s+1));
}

void createSphere(std::vector<vec3>& vertices,
                  std::vector<GLushort>& indices,
                  std::vector<vec2>& texcoords,
                  float radius, unsigned int rings, unsigned int sectors)
{
    float const R = 1./(float)(rings-1);
    float const S = 1./(float)(sectors-1);

    for(unsigned int r = 0; r < rings; ++r) {
        for(unsigned int s = 0; s < sectors; ++s) {
            float const y = sin( -M_PI_2 + M_PI * r * R );
            float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
            float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

            texcoords.push_back(vec2(s*S, r*R));
            vertices.push_back(vec3(x,y,z) * radius);
            push_indices(indices, sectors, r, s);
        }
    }
}

void createSphere2(
    std::vector<GLfloat> &vertices,
    std::vector<GLfloat> &normals,
    std::vector<GLfloat> &texcoords,
    std::vector<GLushort> &indices,
    float radius, unsigned int rings, unsigned int sectors)
{

    float const R = 1./(float)(rings-1);
    float const S = 1./(float)(sectors-1);
    unsigned int r, s;

    vertices.resize(rings * sectors * 3);
    normals.resize(rings * sectors * 3);
    texcoords.resize(rings * sectors * 2);
    std::vector<GLfloat>::iterator v = vertices.begin();
    std::vector<GLfloat>::iterator n = normals.begin();
    std::vector<GLfloat>::iterator t = texcoords.begin();
    for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {

        float const y = sin( -M_PI_2 + M_PI * r * R );
        float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
        float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

        *t++ = s*S;
        *t++ = r*R;

        *v++ = x * radius;
        *v++ = y * radius;
        *v++ = z * radius;

        *n++ = -x;
        *n++ = -y;
        *n++ = -z;
    }

    indices.resize(rings * sectors * 4);
    std::vector<GLushort>::iterator i = indices.begin();
    for(r = 0; r < rings-1; r++) {
        for(s = 0; s < sectors-1; s++) {
           /* 
            *i++ = r * sectors + s;
            *i++ = r * sectors + (s+1);
            *i++ = (r+1) * sectors + (s+1);
            *i++ = (r+1) * sectors + s;
            */
             *i++ = (r+1) * sectors + s;
             *i++ = (r+1) * sectors + (s+1);
             *i++ = r * sectors + (s+1);
             *i++ = r * sectors + s;
        }
    }

}


static void display(void) 
{ 
    glClear(GL_COLOR_BUFFER_BIT); 
    glColor3f(1.0,0.0,0.0); 
    glLoadIdentity(); 
    //glutSolidSphere( 5.0, 20.0, 20.0); 
    glutSolidSphere( 1.0, 20.0, 20.0); 
    glFlush(); 
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

static void myInit() 
{
    glClearColor(1.0,1.0,1.0,1.0); 
    glColor3f(1.0,0.0,0.0); 
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 
    //gluOrtho2D(0.0,499.0,0.0,499.0);  // BAD
    //gluOrtho2D(-5.0, 5.0, -5.0, 5.0); // get only equatorial band
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);   // ok, when radius changed to 1.0
    glMatrixMode(GL_MODELVIEW); 
} 

static void keyboard(unsigned char key, int x, int y) 
{
    if (key == 27)
        exit(0);
}


int main(int argc,char **argv) 
{ 
    //qobj = gluNewQuadric(); 
    glutInit(&argc,argv); 
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB); 
    glutInitWindowSize(500,500); 
    glutCreateWindow("pendulum");
    glutDisplayFunc(display);
   	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
    myInit();
    printf("Uses glutSolidSphere(1.0,20.0,20.0) and gluOrtho2D(-1.0,1.0,-1.0,1.0)...\n");
    printf("File contains so other scrasp for sphere generations, but NOT used.\n");
    glutMainLoop();
    return 0;
}

// eof
