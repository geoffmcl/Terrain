// OpenGLProjectionExample.cpp : Defines the entry point for the application.
//

// #define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
//#pragma comment(lib, "glaux.lib")
//#pragma comment(linker, "/subsystem:windows")

// #include "stdafx.h"
#include <windows.h>                                    // standard Windows app include
#include <winuser.h>          // Windows constants
#include <gl/glut.h>                                       // OpenGL utilties
#include <gl/gl.h>                                              // standard OpenGL include
#include <gl/glu.h>                                             // OpenGL utilties

#define WND_CLASS_NAME  "OpenGL Window Class"


/*************************** Constants and Macros ***************************/
const int   SCREEN_WIDTH    = 500;
const int   SCREEN_HEIGHT   = 500;
const int   SCREEN_BPP      = 32;
const bool  USE_FULLSCREEN  = false;
const char  *APP_TITLE      = "Projections";


/********************************* Globals **********************************/
HDC       g_hdc;                                                                  // global device context
HGLRC     g_hrc;                  // global rendering context
BOOL      g_isFullscreen = TRUE;  // toggles fullscreen and windowed display
BOOL      g_isActive = TRUE;      // false if window is minimized
HWND      g_hwnd = NULL;          // handle of our window
HINSTANCE g_hInstance;            // application instance

/* CUBE */
static float ver[8][3] = 
{
    {-1.0,-1.0,1.0},
    {-1.0,1.0,1.0},
    {1.0,1.0,1.0},
    {1.0,-1.0,1.0},
    {-1.0,-1.0,-1.0},
    {-1.0,1.0,-1.0},
    {1.0,1.0,-1.0},
    {1.0,-1.0,-1.0},
};

static GLfloat color[8][3] = 
{
    {0.0,0.0,0.0},
    {1.0,0.0,0.0},
    {1.0,1.0,0.0},
    {0.0,1.0,0.0},
    {0.0,0.0,1.0},
    {1.0,0.0,1.0},
    {1.0,1.0,1.0},
    {0.0,1.0,1.0},
};

static void quad(int a,int b,int c,int d)
{
    glBegin(GL_QUADS);
    glColor3fv(color[a]);
    glVertex3fv(ver[a]);

    glColor3fv(color[b]);
    glVertex3fv(ver[b]);

    glColor3fv(color[c]);
    glVertex3fv(ver[c]);

    glColor3fv(color[d]);
    glVertex3fv(ver[d]);
    glEnd();
}

static void colorcube()
{
    quad(0,3,2,1);
    quad(2,3,7,6);
    quad(0,4,7,3);
    quad(1,2,6,5);
    quad(4,5,6,7);
    quad(0,1,5,4);
}


/******************************** Prototypes ********************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL    SetupWindow(const char *title, int width, int height, int bits, bool isFullscreen);
BOOL    KillWindow();

GLvoid  ResizeScene(GLsizei width, GLsizei height);
BOOL    InitializeScene();
BOOL    DisplayScene();
BOOL    Cleanup();

void    UpdateProjection(GLboolean toggle = GL_FALSE);

void init_glut()
{
    int argc = 1;
    char *argv[2];
    argv[0] = "dummy";
    argv[1] = 0;
    glutInit(&argc, argv);
}



/*****************************************************************************
 WinMain()

 Windows entry point
*****************************************************************************/
int WINAPI WinMain(HINSTANCE g_hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    MSG   msg;       // message
    BOOL  isDone;    // flag indicating when the app is done

    init_glut();
    // if the window is set up correctly, we can proceed with the message loop
    if (SetupWindow(APP_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, USE_FULLSCREEN)) {
        isDone = FALSE;
    } else {
        // otherwise, we need to never enter the loop and proceed to exit
        isDone = TRUE;
    }

  // main message loop
  while (!isDone)
  {
    if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)   // do we receive a WM_QUIT message?
      {
        isDone = TRUE;              // if so, time to quit the application
      }
      else
      {
        TranslateMessage(&msg);     // translate and dispatch to event queue
        DispatchMessage(&msg);
      }
    }

    // don't update the scene if the app is minimized
    if (g_isActive)
    {
      // update the scene every time through the loop
      DisplayScene();
      // switch the front and back buffers to display the updated scene
      SwapBuffers(g_hdc);
    }
  }

  Cleanup();
  KillWindow();

  return msg.wParam;
} // end WinMain()


/*****************************************************************************
 WndProc()

 Windows message handler
*****************************************************************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
  case WM_ACTIVATE:  // watch for the window being minimized and restored
    {
      if (!HIWORD(wParam))
      {
        // program was restored or maximized
        g_isActive = TRUE;
      }
      else
      {
        // program was minimized
        g_isActive=FALSE;
      }

      return 0;
    }

  case WM_SYSCOMMAND:  // look for screensavers and powersave mode
    {
      switch (wParam)
      {
      case SC_SCREENSAVE:     // screensaver trying to start
      case SC_MONITORPOWER:   // monitor going to powersave mode
        // returning 0 prevents either from happening
        return 0;
      default:
        break;
      }
    } break;

  case WM_CLOSE:    // window is being closed
    {
      // send WM_QUIT to message queue
      PostQuitMessage(0);

      return 0;
    }

  case WM_SIZE:
    {
      // update perspective with new width and height
      ResizeScene(LOWORD(lParam), HIWORD(lParam));
      return 0;
    }

  case WM_CHAR:
    {
      switch (toupper(wParam))
      {
      case VK_SPACE:
        {
          UpdateProjection(GL_TRUE);
          return 0;
        }
      case VK_ESCAPE:
        {
          // send WM_QUIT to message queue
          PostQuitMessage(0);
          return 0;
        }
      default:
        break;
      };
    } break;

  default:
    break;
  }

  return (DefWindowProc(hwnd, message, wParam, lParam));
} // end WndProc()


/*****************************************************************************
 SetupWindow()

 Create the window and everything else we need, including the device and
 rendering context. If a fullscreen window has been requested but can't be
 created, the user will be prompted to attempt windowed mode. Finally,
 InitializeScene is called for application-specific setup.

 Returns TRUE if everything goes well, or FALSE if an unrecoverable error
 occurs. Note that if this is called twice within a program, KillWindow needs
 to be called before subsequent calls to SetupWindow.
*****************************************************************************/
BOOL SetupWindow(const char *title, int width, int height, int bits, bool isFullscreen)
{
  // set the global flag
  g_isFullscreen = isFullscreen;

  // get our instance handle
  g_hInstance = GetModuleHandle(NULL);

  WNDCLASSEX  wc;    // window class

  // fill out the window class structure
  wc.cbSize         = sizeof(WNDCLASSEX);
  wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc    = WndProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = 0;
  wc.hInstance      = g_hInstance;
  wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);  // default icon
  wc.hIconSm        = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon
  wc.hCursor        = LoadCursor(NULL, IDC_ARROW);      // default arrow
  wc.hbrBackground  = NULL;     // no background needed
  wc.lpszMenuName   = NULL;     // no menu
  wc.lpszClassName  = WND_CLASS_NAME;

  // register the windows class
  if (!RegisterClassEx(&wc))
  {
    MessageBox(NULL,"Unable to register the window class", "Error", MB_OK | MB_ICONEXCLAMATION);

    // exit and return FALSE
    return FALSE;
  }

  // if we're in fullscreen mode, set the display up for it
  if (g_isFullscreen)
  {
    // set up the device mode structure
    DEVMODE screenSettings;
    memset(&screenSettings,0,sizeof(screenSettings));

    screenSettings.dmSize       = sizeof(screenSettings);
    screenSettings.dmPelsWidth  = width;    // screen width
    screenSettings.dmPelsHeight = height;   // screen height
    screenSettings.dmBitsPerPel = bits;     // bits per pixel
    screenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // attempt to switch to the resolution and bit depth we've selected
    if (ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
      // if we can't get fullscreen, let them choose to quit or try windowed mode
      if (MessageBox(NULL, "Cannot run in the fullscreen mode at the selected resolution\n"
                           "on your video card. Try windowed mode instead?",
                           "OpenGL Game Programming",
                           MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
      {
        g_isFullscreen = FALSE;
      }
      else
      {
        return FALSE;
      }
    }
  }

  DWORD dwExStyle;
  DWORD dwStyle;

  // set the window style appropriately, depending on whether we're in fullscreen mode
  if (g_isFullscreen)
  {
    dwExStyle = WS_EX_APPWINDOW;
    dwStyle = WS_POPUP;           // simple window with no borders or title bar
    ShowCursor(FALSE);            // hide the cursor for now
  }
  else
  {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    dwStyle = WS_OVERLAPPEDWINDOW;
  }

  // set up the window we're rendering to so that the top left corner is at (0,0)
  // and the bottom right corner is (height,width)
  RECT  windowRect;
  windowRect.left = 0;
  windowRect.right = (LONG) width;
  windowRect.top = 0;
  windowRect.bottom = (LONG) height;

  // change the size of the rect to account for borders, etc. set by the style
  AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

  // class registered, so now create our window
  g_hwnd = CreateWindowEx(dwExStyle,          // extended style
                          WND_CLASS_NAME,     // class name
                          title,              // app name
                          dwStyle |           // window style
                          WS_CLIPCHILDREN |   // required for
                          WS_CLIPSIBLINGS,    // using OpenGL
                          0, 0,               // x,y coordinate
                          windowRect.right - windowRect.left, // width
                          windowRect.bottom - windowRect.top, // height
                          NULL,               // handle to parent
                          NULL,               // handle to menu
                          g_hInstance,        // application instance
                          NULL);              // no extra params

  // see if our window handle is valid
  if (!g_hwnd)
  {
    MessageBox(NULL, "Unable to create window", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // get a device context
  if (!(g_hdc = GetDC(g_hwnd)))
  {
    MessageBox(NULL,"Unable to create device context", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // set the pixel format we want
  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),  // size of structure
    1,                              // default version
    PFD_DRAW_TO_WINDOW |            // window drawing support
    PFD_SUPPORT_OPENGL |            // OpenGL support
    PFD_DOUBLEBUFFER,               // double buffering support
    PFD_TYPE_RGBA,                  // RGBA color mode
    bits,                           // 32 bit color mode
    0, 0, 0, 0, 0, 0,               // ignore color bits, non-palettized mode
    0,                              // no alpha buffer
    0,                              // ignore shift bit
    0,                              // no accumulation buffer
    0, 0, 0, 0,                     // ignore accumulation bits
    16,                             // 16 bit z-buffer size
    8,                              // no stencil buffer
    0,                              // no auxiliary buffer
    PFD_MAIN_PLANE,                 // main drawing plane
    0,                              // reserved
    0, 0, 0 };                      // layer masks ignored

  GLuint  pixelFormat;

  // choose best matching pixel format
  if (!(pixelFormat = ChoosePixelFormat(g_hdc, &pfd)))
  {
    MessageBox(NULL, "Can't find an appropriate pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // set pixel format to device context
  if(!SetPixelFormat(g_hdc, pixelFormat,&pfd))
  {
    MessageBox(NULL, "Unable to set pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // create the OpenGL rendering context
  if (!(g_hrc = wglCreateContext(g_hdc)))
  {
    MessageBox(NULL, "Unable to create OpenGL rendering context", "Error",MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // now make the rendering context the active one
  if(!wglMakeCurrent(g_hdc, g_hrc))
  {
    MessageBox(NULL,"Unable to activate OpenGL rendering context", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  // show the window in the forground, and set the keyboard focus to it
  ShowWindow(g_hwnd, SW_SHOW);
  SetForegroundWindow(g_hwnd);
  SetFocus(g_hwnd);

  // set up the perspective for the current screen size
  ResizeScene(width, height);

  // do one-time initialization
  if (!InitializeScene())
  {
    MessageBox(NULL, "Initialization failed", "Error", MB_OK | MB_ICONEXCLAMATION);
    return FALSE;
  }

  return TRUE;
} // end SetupWindow()


/*****************************************************************************
 KillWindow()

 Deletes the DC, RC, and Window, and restores the original display.
*****************************************************************************/
BOOL KillWindow()
{
  // restore the original display if we're in fullscreen mode
  if (g_isFullscreen)
  {
    ChangeDisplaySettings(NULL, 0);
    ShowCursor(TRUE);
  }

  // if we have an RC, release it
  if (g_hrc)
  {
    // release the RC
    if (!wglMakeCurrent(NULL,NULL))
    {
      MessageBox(NULL, "Unable to release rendering context", "Error", MB_OK | MB_ICONINFORMATION);
    }

    // delete the RC
    if (!wglDeleteContext(g_hrc))
    {
      MessageBox(NULL, "Unable to delete rendering context", "Error", MB_OK | MB_ICONINFORMATION);
    }

    g_hrc = NULL;
  }

  // release the DC if we have one
  if (g_hdc && !ReleaseDC(g_hwnd, g_hdc))
  {
    MessageBox(NULL, "Unable to release device context", "Error", MB_OK | MB_ICONINFORMATION);
    g_hdc = NULL;
  }

  // destroy the window if we have a valid handle
  if (g_hwnd && !DestroyWindow(g_hwnd))
  {
    MessageBox(NULL, "Unable to destroy window", "Error", MB_OK | MB_ICONINFORMATION);
    g_hwnd = NULL;
  }

  // unregister our class so we can create a new one if we need to
  if (!UnregisterClass(WND_CLASS_NAME, g_hInstance))
  {
    MessageBox(NULL, "Unable to unregister window class", "Error", MB_OK | MB_ICONINFORMATION);
    g_hInstance = NULL;
  }

  return TRUE;
} // end KillWindow()


/*****************************************************************************
 ResizeScene()

 Called once when the application starts and again every time the window is
 resized by the user.
*****************************************************************************/
GLvoid ResizeScene(GLsizei width, GLsizei height)
{
  // avoid divide by zero
  if (height==0)
  {
    height=1;
  }

  // reset the viewport to the new dimensions
  glViewport(0, 0, width, height);

  // set up the projection, without toggling the projection mode
  UpdateProjection();
} // end ResizeScene()


/*****************************************************************************
 InitializeScene()

 Performs one-time application-specific setup. Returns FALSE on any failure.
*****************************************************************************/
BOOL InitializeScene()
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);

  return TRUE;
} // end InitializeScene()


/*****************************************************************************
 DisplayScene()

 The work of the application is done here. This is called every frame, and
 handles the actual rendering of the scene.
*****************************************************************************/
BOOL DisplayScene()
{
  GLfloat yellow[4] = { 1.0f, 1.0f, 0.2f, 1.0f };
  GLfloat blue[4] = { 0.2f, 0.2f, 1.0f, 1.0f };
  GLfloat green[4] = { 0.2f, 1.0f, 0.2f, 1.0f };

  glLoadIdentity();
  gluLookAt(-0.5, 1.0, 7.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, yellow);
  glPushMatrix();
  glTranslatef(0.3, 0.0, 1.0);
  //glutSolidCube(0.5);
  colorcube();
  glPopMatrix();

#if 0 // 0000000000000000000000000000000000000000000000000000000
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
  glPushMatrix();
  //glutSolidCube(0.5);
  colorcube();
  glPopMatrix();

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
  glPushMatrix();
  glTranslatef(-0.3, 0.0, -1.0);
  //glutSolidCube(0.5);
  colorcube();
  glPopMatrix();
#endif // 00000000000000000000000000000000000000000000000000000
  return TRUE;
} // end DisplayScene()


/*****************************************************************************
 Cleanup()

 Called at the end of successful program execution.
*****************************************************************************/
BOOL Cleanup()
{
  return TRUE;
} // end Cleanup()


/****************************************************************************
 UpdateProjection()

 Sets the current projection mode. If toggle is set to GL_TRUE, then the
 projection will be toggled between perspective and orthograpic. Otherwise,
 the previous selection will be used again.
*****************************************************************************/
void UpdateProjection(GLboolean toggle)
{
  static GLboolean s_usePerspective = GL_TRUE;

  // toggle the control variable if appropriate
  if (toggle)
    s_usePerspective = !s_usePerspective;

  // select the projection matrix and clear it out
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // choose the appropriate projection based on the currently toggled mode
  if (s_usePerspective)
  {
    // set the perspective with the appropriate aspect ratio
    glFrustum(-1.0, 1.0, -1.0, 1.0, 5, 100);
  }
  else
  {
    // set up an orthographic projection with the same near clip plane
    glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
  }

  // select modelview matrix and clear it out
  glMatrixMode(GL_MODELVIEW);
} // end UpdateProjection

// eof
