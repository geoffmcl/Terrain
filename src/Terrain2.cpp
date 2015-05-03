/*
 *		This Code Was Created By Ben Humphrey 2001
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit NeHe Productions At http://nehe.gamedev.net
 *
 * from : http://nehe.gamedev.net/tutorial/beautiful_landscapes_by_means_of_height_mapping/16006/
 *
 * Keys
 * ESC        = exit
 * up-arrrow  = zoom in
 * down-arrow = zoom out
 * F1         = toggle full screen
 *
 */

#include <Windows.h>									// Header File For Windows
//#include <Windowsx.h>
#include <stdio.h>										// Header file for standard input output (IE, "FILE") (NEW)
#include <time.h>
#include <sstream>
#include <GL/glut.h>
#include <GL/gl.h>										// Header File For The OpenGL32 Library
#include <GL/glu.h>										// Header File For The GLu32 Library
//#include <GL/glaux.h>									// Header File For The Glaux Library
#include "utils/sprtf.hxx"

#pragma comment(lib, "opengl32.lib")					// Link OpenGL32.lib
#pragma comment(lib, "glu32.lib")						// Link Glu32.lib

#ifndef DEF_TERRAIN_RAW
#define DEF_TERRAIN_RAW "../data/Terrain2.raw"
#endif

static const char *terrain_raw = DEF_TERRAIN_RAW;
static const char *def_log = "tempterr2.txt";

// vector min (0,55,0) max (1008,221,1008) 
// center (504,138,504) range (1008,166,1008) 
#define		MAP_SIZE	  1024							// Size Of Our .RAW Height Map (NEW)
#define		STEP_SIZE	  16							// Width And Height Of Each Quad (NEW)
#define		HEIGHT_RATIO  1.5f							// Ratio That The Y Is Scaled According To The X And Z (NEW)

HDC			hDC = NULL;									// Private GDI Device Context
HGLRC		hRC = NULL;									// Permanent Rendering Context
HWND		hWnd = NULL;									// Holds Our Window Handle
HINSTANCE	hInstance;									// Holds The Instance Of The Application

bool		keys[256];									// Array Used For The Keyboard Routine
bool		active = TRUE;								// Window Active Flag Set To TRUE By Default
bool		fullscreen = TRUE;							// Fullscreen Flag Set To TRUE By Default
bool		bRender = TRUE;								// Polygon Flag Set To TRUE By Default (NEW)

int xPos, yPos;    // mouse position on left click

BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];					// Holds The Height Map Data (NEW)

#define X_CENT 186.0
#define Y_CENT  55.0
#define Z_CENT 171.0

// vector min (0,55,0) max (1008,221,1008) 
// center (504,138,504) range (1008,166,1008) 
float scaleValue = 0.15f;								// Scale Value For The Terrain (NEW)
// HOW were these number derived?????????
static GLdouble eyex = 212; // 252; // 212.0;
static GLdouble eyey =  60; // 69; // 60.0; // 0 fails - off screen somewhere
static GLdouble eyez = 194; // 252; // 194.0;
static GLdouble centx = X_CENT; // 186.0;
static GLdouble centy = Y_CENT; // 55.0;
static GLdouble centz = Z_CENT; // 171.0;
static GLdouble upx = 0.0;
static GLdouble upy = 1.0;
static GLdouble upz = 0.0;
// reset values
float rscaleValue = 0.15f;								// Scale Value For The Terrain (NEW)
static GLdouble reyex = 212.0;
static GLdouble reyey = 60.0;
static GLdouble reyez = 194.0;
static GLdouble rcentx = 186.0;
static GLdouble rcenty = 55.0;
static GLdouble rcentz = 171.0;
static GLdouble rupx = 0.0;
static GLdouble rupy = 1.0;
static GLdouble rupz = 0.0;

static double sDelay = 30.0 / 1000.0;

static double m_xrot = 0;
static double m_yrot = 0;
static double m_zrot = 0;
static double x_increment = 0.2;
static double y_increment = 0.2;
static double z_increment = 0.2;

static bool add_x_rot = false;
static bool add_y_rot = false;
static bool add_z_rot = false;


static int is_frozen = 0;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
		height=1;										// Making Height Equal One

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window.  Farthest Distance Changed To 500.0f (NEW)
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 500.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
    SPRTF("%s Resize width %d, height %d\n", get_time_stg(), (int)width, (int)height );
}

// Loads The .RAW File And Stores It In pHeightMap
int LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap)
{
	FILE *pFile = NULL;

	// Open The File In Read / Binary Mode.
	pFile = fopen( strName, "rb" );

	// Check To See If We Found The File And Could Open It
	if ( pFile == NULL )	
	{
		// Display Error Message And Stop The Function
		MessageBox(NULL, "Can't Find The Height Map!", "Error", MB_OK);
		return 1;
	}

	if (fread( pHeightMap, 1, nSize, pFile ) != nSize) 
    {
		MessageBox(NULL, "Read of Height Map FAILED!", "Error", MB_OK);
        fclose(pFile);
		return 1;
    }
	// After We Read The Data, It's A Good Idea To Check If Everything Read Fine
	int result = ferror( pFile );

	// Check If We Received An Error
	if (result)
	{
		MessageBox(NULL, "Failed To Get Data!", "Error", MB_OK);
        fclose(pFile);
		return 1;
	}

	// Close The File.
	fclose(pFile);
    return 0;   // SUCCESS
}

int Height(BYTE *pHeightMap, int X, int Y)				// This Returns The Height From A Height Map Index
{
	int x = X % MAP_SIZE;								// Error Check Our x Value
	int y = Y % MAP_SIZE;								// Error Check Our y Value

	if(!pHeightMap) return 0;							// Make Sure Our Data Is Valid

	return pHeightMap[x + (y * MAP_SIZE)];				// Index Into Our Height Array And Return The Height
}

// generate a COLOR TABLE
// ==========================
// max height FF 00 00
// ...
// mid height 00 FF 00
// ...
// min height 00 00 FF
// ===========================
typedef struct tagCOLORTABLE {
    int color;
    BYTE r,g,b;
}COLORTABLE, *PCOLORTABLE;
PCOLORTABLE pColorTable = 0;
int color_max = 0;
int color_min = 0;
int table_max = 0;
int get_color( int h, float *pr, float *pg, float *pb )
{
    int i;
    PCOLORTABLE pct;
    if (!pColorTable)
        return 0;   // no color table generated
    for (i = 0; i < table_max; i++) {
        pct = &pColorTable[i];
        if (pct->color == h) {
            // ******************************************
            // NOTE WELL OpenGL inversion of RED and BLUE
            // ******************************************
            *pb = (float)pct->r / 256.0f;
            *pg = (float)pct->g / 256.0f;
            *pr = (float)pct->b / 256.0f;
            return 1;
        }
    }
    return 0;
}

#define MEOL std::endl

void  gen_color_table( int min_h, int max_h )
{
    int i, j, half, qtr, qtr3;
    int diff = max_h - min_h;
    BYTE r,g,b;
    double eight, sixteen;
    PCOLORTABLE pct;
    int sixteenth[16];
    if (diff < 16)
        return;
    diff++;
    table_max = diff;
    color_max = max_h;
    color_min = min_h;
    pColorTable = (PCOLORTABLE) new COLORTABLE[diff];
    half = diff / 2;
    qtr  = half / 2;
    qtr3 = qtr *  3;
    eight = (double)diff / 8.0;
    sixteen = (double)diff / 16.0;
    for (i = 0; i < 16; i++) {
        sixteenth[i] = (int)((sixteen * i) + 0.5);
    }
    for (i = 0; i < diff; i++) {
        pct = &pColorTable[i];
        pct->color = min_h + i;
        if (pct->color == 211)
            j = 0;
        if (i == 0) {
            r = 255;
            g = 0;
            b = 0;
        } else if ((i + 1) == diff) {
            r = 0;
            g = 0;
            b = 255;
        } else if (i == half) {
            r = 0;
            g = 255;
            b = 0;
        } else if (i == qtr) {
            r = 128;
            g = 128;
            b = 0;
        } else if (i == qtr3) {
            r = 0;
            g = 128;
            b = 128;
        } else if (i == (int)(eight+0.5)) {
            r = 192;
            g = 64;
            b = 0;
        } else if (i == (int)((eight * 3.0)+0.5)) {
            r = 64;
            g = 192;
            b = 0;
        } else if (i == (int)((eight * 5.0)+0.5)) {
            r = 0;
            g = 192;
            b = 64;
        } else if (i == (int)((eight * 7.0)+0.5)) {
            r = 0;
            g = 64;
            b = 192;
        } else {
            for (j = 0; j < 16; j++) {
                if (i == sixteenth[j]) {
                    if (i < half) {
                        // progressing RED towards GREEN
                        // decrease RED increase GREEN
                        g = 16 * (j * 2);
                        r = 256 - g;
                        b = 0;
                    } else {
                        // progressing GREEN toward BLUE
                        // decrease GREEN increase BLUE
                        b = 16 * (j - 7) * 2;
                        if (b == 0)
                            b = 256 - 16;
                        g = 256 - b;
                        r = 0;
                    }
                    break;
                }
            }

        }
        pct->r = r;
        pct->g = g;
        pct->b = b;
    }
    j = 0;
    std::stringstream ss;
    ss << "<table>" << MEOL;
    char *cp = GetNxtBuf();
    for (i = 0; i < diff; i++) {
        pct = &pColorTable[i];
        if (j == 0)
            ss << "<tr>" << MEOL;
        sprintf(cp,"<td bgcolor=\"#%02x%02x%02x\">%3d (%02X %02X %02X)</td>", pct->r, pct->g, pct->b,
            pct->color, pct->r, pct->g, pct->b);
        ss << cp << MEOL;
        j++;
        if (j == 10) {
            ss << "</tr>" << MEOL;
            j = 0;

        }
    }
    if (j && (j < 10)) {
        while (j < 10) {
            ss << "<td>&nbsp;</td>";
            j++;
        }
        ss << "</tr>" << MEOL;
    }
    ss << "</table>" << MEOL;
    FILE *fp = fopen("temptable.html", "w");
    if (fp) {
        fwrite(ss.str().c_str(),ss.str().size(),1,fp);
        fclose(fp);
        SPRTF("Table written to 'temptable.html'\n");
    } else {
        direct_out_it((char *)ss.str().c_str());
        SPRTF("Write to 'temptable.html' FAILED\n");
    }
}

typedef struct tagVEC3 {
    int x,y,z;
}VEC3;

#define SETMM {  \
            if (x > max_v.x) max_v.x = x; \
            if (y > max_v.y) max_v.y = y; \
            if (z > max_v.z) max_v.z = z; \
            if (x < min_v.x) min_v.x = x; \
            if (y < min_v.y) min_v.y = y; \
            if (z < min_v.z) min_v.z = z; \
}


void show_raw( BYTE *pHeightMap )
{
    int X,Y,h, count = 0;
    int min_h = 99999;
    int max_h = -1;
    int diff;
    int x,y,z;
    std::stringstream ss, hts;
    hts << "Heights sampled..." << MEOL;
    ss << "Vectors generated..." << MEOL;
    VEC3 min_v = {99999,99999,99999};
    VEC3 max_v = {-99999,-99999,-99999};
	for ( Y = 0; Y < (MAP_SIZE-STEP_SIZE); Y += STEP_SIZE ) 
    {
        //SPRTF("%4d: ", Y + 1);
        hts << (Y + 1) << ": ";
		for ( X = 0; X < (MAP_SIZE-STEP_SIZE); X += STEP_SIZE )
		{
            h = pHeightMap[X + (Y * MAP_SIZE)] & 0xff;				// Index Into Our Height Array And Return The Height
            //SPRTF("%d ", h);
            hts << h << " ";
            if (h < min_h)
                min_h = h;
            if (h > max_h)
                max_h = h;
            count++;
			// Get The (X, Y, Z) Value For The Bottom Left Vertex
			x = X;							
			y = Height(pHeightMap, X, Y );	
			z = Y;
            ss << "(" << x << "," << y << "," << z << ") ";
            SETMM;
 			// Get The (X, Y, Z) Value For The Top Left Vertex
			x = X;										
			y = Height(pHeightMap, X, Y + STEP_SIZE );  
			z = Y + STEP_SIZE ;							
            ss << "(" << x << "," << y << "," << z << ") ";
            SETMM;
 			// Get The (X, Y, Z) Value For The Top Right Vertex
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE ); 
			z = Y + STEP_SIZE ;
            ss << "(" << x << "," << y << "," << z << ") ";
            SETMM;
			// Get The (X, Y, Z) Value For The Bottom Right Vertex
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y ); 
			z = Y;
            ss << "(" << x << "," << y << "," << z << ")" << MEOL;
            SETMM;
        }
        // SPRTF("\n");
        hts << MEOL;
    }
    diff = max_h - min_h;
    // show min, max and range
    // vector min (0,55,0) max (1008,221,1008) 
    // center (504,138,504) range (1008,166,1008) 

    ss << "vector min (" << min_v.x << "," << min_v.y << "," << min_v.z << ") ";
    ss << "max (" << max_v.x << "," << max_v.y << "," << max_v.z << ") " << MEOL;
    ss << "center (" << ((min_v.x + max_v.x) / 2) << ",";
    ss << ((min_v.y + max_v.y) / 2) << ",";
    ss << ((min_v.z + max_v.z) / 2) << ") ";
    ss << "range (" << (max_v.x - min_v.x) << ",";
    ss << (max_v.y - min_v.y) << ",";
    ss << (max_v.z - min_v.z) << ") " << MEOL;

    SPRTF("Sampled %d points, min %d, max %d, %d steps\n", count, min_h, max_h, diff);
    direct_out_it( (char *) hts.str().c_str() );
    direct_out_it( (char *) ss.str().c_str() );

    if (diff >= 16) {
        gen_color_table( min_h, max_h );
    }
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	if (LoadRawFile((LPSTR)terrain_raw, MAP_SIZE * MAP_SIZE, g_HeightMap))	// (NEW)
        return FALSE;

    SPRTF("Loaded height map '%s', %d bytes\n", terrain_raw, MAP_SIZE * MAP_SIZE);
    show_raw(g_HeightMap);
	return TRUE;										// Initialization Went OK
}

														
void SetVertexColor(BYTE *pHeightMap, int x, int y)		// Sets The Color Value For A Particular Index, Depending On The Height Index
{
    float fR,fG,fB;
	if(!pHeightMap) return;								// Make Sure Our Height Data Is Valid

    int h = Height(pHeightMap, x, y );

    if (get_color(h, &fR, &fG, &fB)) {
	    glColor3f(fR, fG, fB );
    } else {
    	fB = -0.15f + (h / 256.0f);
	    // Assign This Blue Shade To The Current Vertex
	    glColor3f(0, 0, fB );
    }
}

void RenderHeightMap(BYTE pHeightMap[])					// This Renders The Height Map As Quads
{
	int X = 0, Y = 0;									// Create Some Variables To Walk The Array With.
	int x, y, z;										// Create Some Variables For Readability

	if(!pHeightMap) return;								// Make Sure Our Height Data Is Valid

	if(bRender)											// What We Want To Render
		glBegin( GL_QUADS );							// Render Polygons
	else 
		glBegin( GL_LINES );							// Render Lines Instead

	for ( X = 0; X < (MAP_SIZE-STEP_SIZE); X += STEP_SIZE ) 
    {
		for ( Y = 0; Y < (MAP_SIZE-STEP_SIZE); Y += STEP_SIZE )
		{
			// Get The (X, Y, Z) Value For The Bottom Left Vertex
			x = X;							
			y = Height(pHeightMap, X, Y );	
			z = Y;							

			// Set The Color Value Of The Current Vertex
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);						// Send This Vertex To OpenGL To Be Rendered (Integer Points Are Faster)

			// Get The (X, Y, Z) Value For The Top Left Vertex
			x = X;										
			y = Height(pHeightMap, X, Y + STEP_SIZE );  
			z = Y + STEP_SIZE ;							
			
			// Set The Color Value Of The Current Vertex
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);						// Send This Vertex To OpenGL To Be Rendered

			// Get The (X, Y, Z) Value For The Top Right Vertex
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE ); 
			z = Y + STEP_SIZE ;

			// Set The Color Value Of The Current Vertex
			SetVertexColor(pHeightMap, x, z);
			
			glVertex3i(x, y, z);						// Send This Vertex To OpenGL To Be Rendered

			// Get The (X, Y, Z) Value For The Bottom Right Vertex
			x = X + STEP_SIZE; 
			y = Height(pHeightMap, X + STEP_SIZE, Y ); 
			z = Y;

			// Set The Color Value Of The Current Vertex
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);						// Send This Vertex To OpenGL To Be Rendered
		}
    }
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);					// Reset The Color
}

static float x_axis = 0.0f;
static float y_axis = 0.0f;
static float z_axis = 0.0f;

// real simple x, y, z axis!
// but the 'model' is offset from 0,0,0 in all three axis
void draw_axes( GLfloat cx, GLfloat cy, GLfloat cz, GLfloat len )
{
    glPushMatrix();
    // Draw a red x-axis, a green y-axis, and a blue z-axis.  Each of the
    // axes are 2 x len units long.
    glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3f(cx, cy, cz); glVertex3f(len, 0, 0);
        glVertex3f(cx, cy, cz); glVertex3f(-len, 0, 0);
        glColor3f(0, 1, 0);
        glVertex3f(cx, cy, cz); glVertex3f(0, len, 0);
        glVertex3f(cx, cy, cz); glVertex3f(0, -len, 0);
        glColor3f(0, 0, 1);
        glVertex3f(cx, cy, cz); glVertex3f(0, 0, len);
        glVertex3f(cx, cy, cz); glVertex3f(0, 0, -len);
    glEnd();
    glPopMatrix();
}


int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The Matrix
	
	// 			 Position	      View		Up Vector
	//gluLookAt(212, 60, 194,  186, 55, 171,  0, 1, 0);	// This Determines Where The Camera's Position And View Is
	gluLookAt(eyex, eyey, eyez, centx, centy, centz, upx, upy, upz);	// This Determines Where The Camera's Position And View Is

    //       x           y                          z
	glScalef(scaleValue, scaleValue * HEIGHT_RATIO, scaleValue); // zoom in/out

	glRotatef(m_xrot, 1.0f, 0.0f, 0.0f);   // x
    glRotatef(m_yrot, 0.0f, 1.0f, 0.0f);   // y
    glRotatef(m_zrot, 0.0f, 0.0f, 1.0f);   // z

	RenderHeightMap(g_HeightMap);						// Render The Height Map

    //Draw_Axes();
    //drawAxes( 500.0 );
    //draw_axes( centx, centy, centz, 450.0f );
    draw_axes( 0.0f, 0.0f, 0.0f, 450.0f );
	return TRUE;										// Keep Going
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;							// Holds The Results After Searching For A Match
	WNDCLASS	wc;										// Windows Class Structure
	DWORD		dwExStyle;								// Window Extended Style
	DWORD		dwStyle;								// Window Style
	RECT		WindowRect;								// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;							// Set Left Value To 0
	WindowRect.right=(long)width;						// Set Right Value To Requested Width
	WindowRect.top=(long)0;								// Set Top Value To 0
	WindowRect.bottom=(long)height;						// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;							// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);		// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;			// WndProc Handles Messages
	wc.cbClsExtra		= 0;							// No Extra Window Data
	wc.cbWndExtra		= 0;							// No Extra Window Data
	wc.hInstance		= hInstance;					// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);	// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);	// Load The Arrow Pointer
	wc.hbrBackground	= NULL;							// No Background Required For GL
	wc.lpszMenuName		= NULL;							// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";						// Set The Class Name

	if (!RegisterClass(&wc))							// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									// Return FALSE
	}
	
	if (fullscreen)										// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;						// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);	// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;		// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;		// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;			// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;						// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;							// Return FALSE
			}
		}
	}

	if (fullscreen)										// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;						// Window Extended Style
		dwStyle=WS_POPUP;								// Windows Style
		ShowCursor(FALSE);								// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;					// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,				// Extended Style For The Window
								"OpenGL",				// Class Name
								title,					// Window Title
								dwStyle |				// Defined Window Style
								WS_CLIPSIBLINGS |		// Required Window Style
								WS_CLIPCHILDREN,		// Required Window Style
								0, 0,					// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,					// No Parent Window
								NULL,					// No Menu
								hInstance,				// Instance
								NULL)))					// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();									// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=					// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
		1,												// Version Number
		PFD_DRAW_TO_WINDOW |							// Format Must Support Window
		PFD_SUPPORT_OPENGL |							// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,								// Must Support Double Buffering
		PFD_TYPE_RGBA,									// Request An RGBA Format
		bits,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,								// Color Bits Ignored
		0,												// No Alpha Buffer
		0,												// Shift Bit Ignored
		0,												// No Accumulation Buffer
		0, 0, 0, 0,										// Accumulation Bits Ignored
		16,												// 16Bit Z-Buffer (Depth Buffer)  
		0,												// No Stencil Buffer
		0,												// No Auxiliary Buffer
		PFD_MAIN_PLANE,									// Main Drawing Layer
		0,												// Reserved
		0, 0, 0											// Layer Masks Ignored
	};

	if (!(hDC=GetDC(hWnd)))								// Did We Get A Device Context?
	{
		KillGLWindow();									// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))		// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();									// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))			// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();									// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))					// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();									// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))						// Try To Activate The Rendering Context
	{
		KillGLWindow();									// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);							// Show The Window
	SetForegroundWindow(hWnd);							// Slightly Higher Priority
	SetFocus(hWnd);										// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);						// Set Up Our Perspective GL Screen

	if (!InitGL())										// Initialize Our Newly Created GL Window
	{
		KillGLWindow();									// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;									// Return FALSE
	}

	return TRUE;										// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,				// Handle For This Window
							UINT	uMsg,				// Message For This Window
							WPARAM	wParam,				// Additional Message Information
							LPARAM	lParam)				// Additional Message Information
{
	switch (uMsg)										// Check For Windows Messages
	{
		case WM_ACTIVATE:								// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))						// Check Minimization State
			{
				active=TRUE;							// Program Is Active
			}
			else
			{
				active=FALSE;							// Program Is No Longer Active
			}

			return 0;									// Return To The Message Loop
		}

		case WM_SYSCOMMAND:								// Intercept System Commands
		{
			switch (wParam)								// Check System Calls
			{
				case SC_SCREENSAVE:						// Screensaver Trying To Start?
				case SC_MONITORPOWER:					// Monitor Trying To Enter Powersave?
				return 0;								// Prevent From Happening
			}
			break;										// Exit
		}

		case WM_CLOSE:									// Did We Receive A Close Message?
		{
			PostQuitMessage(0);							// Send A Quit Message
			return 0;									// Jump Back
		}

		case WM_LBUTTONDOWN:							// Did We Receive A Left Mouse Click?
		{
            xPos = ((int)(short)((WORD)(((DWORD_PTR)(lParam)) & 0xffff))); // ((int)(short)LOWORD(lParam)); // GET_X_LPARAM(lParam); 
            yPos = ((int)(short)((WORD)((((DWORD_PTR)(lParam)) >> 16) & 0xffff))); // ((int)(short)HIWORD(lParam)); // GET_Y_LPARAM(lParam); 
			bRender = !bRender;							// Change The Rendering State Between Fill And Wire Frame
            SPRTF("%s Left button at x,y %d,%d. bRender=%s\n", get_time_stg(),
                xPos, yPos,
                (bRender ? "On" : "Off"));
			return 0;									// Jump Back
		}

		case WM_KEYDOWN:								// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;						// If So, Mark It As TRUE
            SPRTF("%s Key %d down\n", get_time_stg(), (int)wParam);
			return 0;									// Jump Back
		}

		case WM_KEYUP:									// Has A Key Been Released?
		{
			keys[wParam] = FALSE;						// If So, Mark It As FALSE
            SPRTF("%s Key %d up\n", get_time_stg(), (int)wParam);
			return 0;									// Jump Back
		}

		case WM_SIZE:									// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;									// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
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
void show_FPS()
{
    static int framecnt[MX_FPS];
    static int currFrame = 0;
    static time_t currSec, lastSec = 0, nextSec;
    static int frames = 0;

    frames++;
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
            SPRTF("%s Av. FPS %d\n", get_time_stg(), total);
        }
    }
}


void animate()
{
    if (!is_frozen) {
        if (check_clock()) {
            if (add_x_rot) {
                m_xrot += x_increment;
                if (m_xrot >= 360.0)
                    m_xrot = 0.0;
            }
            if (add_y_rot) {
                m_yrot += y_increment;
                if (m_yrot >= 360.0)
                    m_yrot = 0.0;
            }
            if (add_z_rot) {
                m_zrot += z_increment;
                if (m_zrot >= 360.0)
                    m_zrot = 0.0;

            }
        }
    }
    show_FPS();
}

int WINAPI WinMain(	HINSTANCE	hInstance,				// Instance
					HINSTANCE	hPrevInstance,			// Previous Instance
					LPSTR		lpCmdLine,				// Command Line Parameters
					int			nCmdShow)				// Window Show State
{
	MSG		msg;										// Windows Message Structure
	BOOL	done=FALSE;									// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	// if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;								// Windowed Mode
	}

    add_std_out(0); // kill standard out - is a WIN32 app - no console
    set_log_file((char *)def_log,false);

	// Create Our OpenGL Window
	if (!CreateGLWindow("NeHe & Ben Humphrey's Height Map Tutorial", 640, 480, 16, fullscreen))
	{
		return 0;										// Quit If Window Was Not Created
	}

	while(!done)										// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))		// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)					// Have We Received A Quit Message?
			{
				done=TRUE;								// If So done=TRUE
			}
			else										// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);					// Translate The Message
				DispatchMessage(&msg);					// Dispatch The Message
			}
		}
		else											// If There Are No Messages
		{
            animate();
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;								// ESC or DrawGLScene Signalled A Quit
			}
			else if (active)							// Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);						// Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1])							// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;						// If So Make Key FALSE
				KillGLWindow();							// Kill Our Current Window
				fullscreen=!fullscreen;					// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("NeHe & Ben Humphrey's Height Map Tutorial", 640, 480, 16, fullscreen))
				{
					return 0;							// Quit If Window Was Not Created
				}
			}

			if (keys[VK_UP]) {							// Is the UP ARROW key Being Pressed?
				scaleValue += 0.001f;					// Increase the scale value to zoom in
                keys[VK_UP] = FALSE;
            }
			if (keys[VK_DOWN])	{						// Is the DOWN ARROW key Being Pressed?
				scaleValue -= 0.001f;					// Decrease the scale value to zoom out
                keys[VK_DOWN] = FALSE;
            }
            if (keys['X']) {
                keys['X'] = FALSE;
                add_x_rot = !add_x_rot;
            }
            if (keys['Y']) {
                keys['Y'] = FALSE;
                add_y_rot = !add_y_rot;
            }
            if (keys['Z']) {
                keys['Z'] = FALSE;
                add_z_rot = !add_z_rot;
            }
            if (keys['R']) {
                keys['R'] = FALSE;
                m_xrot = 0;
                m_yrot = 0;
                m_zrot = 0;
                scaleValue = 0.15f;		// Scale Value For The Terrain (NEW)
                add_x_rot = false;
                add_y_rot = false;
                add_z_rot = false;
            }
            if (keys['F']) {
                keys['F'] = FALSE;
                is_frozen = !is_frozen;
            }

		}
	}

	// Shutdown
	KillGLWindow();										// Kill The Window
	return (msg.wParam);								// Exit The Program
}

// eof
