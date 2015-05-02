//---------------------------------------------------------------------------
//--- LED cube class ver: 1.00 ----------------------------------------------
// from : http://stackoverflow.com/questions/25057471/drawing-3d-sphere-in-c-c
//---------------------------------------------------------------------------
#ifndef _LED_cube_h
#define _LED_cube_h
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <math.h>
//---------------------------------------------------------------------------
const int _LED_cube_size=16;
//---------------------------------------------------------------------------
#ifndef BYTE
typedef unsigned char BYTE;
#endif
class LED_cube
{
public:
    int n,map[_LED_cube_size][_LED_cube_size][_LED_cube_size];

    LED_cube()              { n=_LED_cube_size; }
    LED_cube(LED_cube& a)   { *this=a; }
    ~LED_cube()             { }
    LED_cube* operator = (const LED_cube *a) { *this=*a; return this; }
    //LED_cube* operator = (const LED_cube &a) { /*...copy...*/ return this; }
    void cls(int col);                                  // clear cube with col 0x00BBGGRR
    void sphere(int x0,int y0,int z0,int r,int col);    // draws sphere surface with col 0x00BBGGRR
    void glDraw();                                      // render cube by OpenGL as 1x1x1 cube at 0,0,0
};
//---------------------------------------------------------------------------
void LED_cube::cls(int col)
{
    int x,y,z;
    for (x=0;x<n;x++)
     for (y=0;y<n;y++)
      for (z=0;z<n;z++)
       map[x][y][z]=col;
}
//---------------------------------------------------------------------------
void LED_cube::sphere(int x0,int y0,int z0,int r,int col)
{
    int x,y,z,xa,ya,za,xb,yb,zb,xr,yr,zr,xx,yy,zz,rr=r*r;
    // bounding box
    xa=x0-r;
    if (xa<0)
        xa=0;
    xb=x0+r;
    if (xb>n)
        xb=n;
    ya=y0-r;
    if (ya<0)
        ya=0;
    yb=y0+r; 
    if (yb>n) 
        yb=n;
    za=z0-r;
    if (za<0)
        za=0; 
    zb=z0+r;
    if (zb>n)
        zb=n;
    // project xy plane
    for (x=xa,xr=x-x0,xx=xr*xr ; x < xb ; x++,xr++,xx=xr*xr) {
        for (y=ya,yr=y-y0,yy=yr*yr ; y<yb ; y++,yr++,yy=yr*yr) {
            zz=rr-xx-yy;
            if (zz<0)
                continue;
            zr = sqrt((float)zz);
            z=z0-zr; 
            if ((z>0)&&(z<n))
                map[x][y][z]=col;
            z=z0+zr; 
            if ((z>0)&&(z<n)) 
                map[x][y][z]=col;
        }
    }
    // project xz plane
    for (x=xa,xr=x-x0,xx=xr*xr ; x<xb ; x++,xr++,xx=xr*xr) {
        for (z=za,zr=z-z0,zz=zr*zr ; z<zb ; z++,zr++,zz=zr*zr) {
            yy=rr-xx-zz; 
            if (yy<0)
                continue;
            yr=sqrt((float)yy);
            y=y0-yr;
            if ((y>0)&&(y<n))
                map[x][y][z]=col;
            y=y0+yr;
            if ((y>0)&&(y<n)) 
                map[x][y][z]=col;
        }
    }
    // project yz plane
    for (y=ya,yr=y-y0,yy=yr*yr ; y<yb ; y++,yr++,yy=yr*yr) {
        for (z=za,zr=z-z0,zz=zr*zr ; z<zb ; z++,zr++,zz=zr*zr) {
            xx=rr-zz-yy;
            if (xx<0)
                continue;
            xr=sqrt((float)xx);
            x=x0-xr;
            if ((x>0)&&(x<n))
                map[x][y][z]=col;
            x=x0+xr;
            if ((x>0)&&(x<n))
                map[x][y][z]=col;
        }
    }
}
//---------------------------------------------------------------------------
void LED_cube::glDraw()
{
#ifdef __gl_h_
    int x,y,z;
    float p[3],dp=1.0/float(n-1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);

    glPointSize(2.0);

    glBegin(GL_POINTS);

    for (p[0]=-0.5,x=0;x<n;x++,p[0]+=dp)
     for (p[1]=-0.5,y=0;y<n;y++,p[1]+=dp)
      for (p[2]=-0.5,z=0;z<n;z++,p[2]+=dp)
        {
        glColor4ubv((BYTE*)(&map[x][y][z]));
        glVertex3fv(p);
        }
    glEnd();
    glDisable(GL_BLEND);
    glPointSize(1.0);
#endif
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// eof
