//---------------------------------------------------------------
//
// FILE: zs_ZSweepDemo.c
// Applied Mathematics and Statistics
// State University of New York at Stony Brook
// Stony Brook, NY 11794-3600
//
// Created : 01/09/2000
// Modified: 11/09/2000
// Ricardo Farias : rfarias@ams.sunysb.edu
//---------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "zs_ZSweepDemo.hh"
#include "GraphicsGems.h"
#include "zs_debug.hh"
#include "zs_basic.hh"
#include "zs_scene.hh"
#include "zs_light.hh"
#include "zs_viewpoint.hh"

bool firstSave = true;
unsigned int MMAAXX = 0;

#define ENTER 13
int parWin   = 0;
int auxWin   = 0;
int lisWin = 0;

#define RESOLUTION        64
#define GRIDSIZE          64

unsigned int res_lists = 32 ;
unsigned int startXPix = 16 ;
unsigned int startZPix = 12 ;
unsigned int numPointsAtATime = 1;

// Constants 
#define false 0
#define true  1
/* Some window constant definitions                                          */
#define BORDER         12
#define XOFF            0
#define YOFF            0
#define WINDOW_WIDTH    504
#define WINDOW_HEIGHT   504
#define ZOOMFACTOR      0.8         /* Zoom factor                           */

/* Some other constants                                                      */
#define BLOCKSIZE       512         /* Array block size                      */
#define LINESIZE        4096        /* Maximum number of characters per line */

/* Model related variables                                                   */
GLfloat xmin, xmax, ymin, ymax, zmin, zmax;    /* Range of a data set        */
GLfloat centerX, centerY, centerZ;             /* Estimated object center    */
GLfloat rad, Rad;                              /* radius of a data set       */
GLfloat angle, vx, vy;                         /* rotatoin angle and axis    */

// Render Variables
Scene         *sceneptr;
ViewPlane     *pixptr;
Lighting      *lightptr;

/* Keep current MODELVIEW matrix                                             */
GLfloat current_matrix[16] = 
{ 1.0, 0.0, 0.0, 0.0, 
  0.0, 1.0, 0.0, 0.0,
  0.0, 0.0, 1.0, 0.0, 
  0.0, 0.0, 0.0, 1.0 
};

// Auxiliary Window Management Variables
GLint currWin = 0; // Current active window 1 == Parent, 2== Auxiliary
bool  auxCreated = false;  // Indicates if the image window exists or not
bool  listsCreated = false;  // Indicates if the lists window exists or not

/* View related variables                                                    */
GLint   view_width, view_height;               /* Range of viewport          */
GLint   mouseX, mouseY;                        /* Initial mouse position     */
GLint   mouseEnable;                           /* 0 for rot, 1 for trans     */
GLint   solid;                                 /* Generate solid if nonzero  */
GLint   spin, circling;                        /* Toggle spin on \ off       */
GLint   shift, floating;                       /* Toggle shift on \ off      */
bool    bbFlag=false;                          /* Indicates if bb has been set properly */
bool    renderPause  = true;
bool    renderStep   = false;
bool    dispAxisFlag = true;
bool    rotateX = false;
bool    rotateY = true;
int     sleepTime    = 0;
int     edgemode     = MARK_BOUNDARY;

float inc4ps = 0.05;

// Variables for the sweep plane animation
bool sweepPlaneFlag = TRUE,targetPlaneFlag = false;
Vector3  spColor = { 0.0, 0.4, 0.85 };
Vector3  tpColor = { 0.3, 0.3, 0.3 };
GLfloat  spAlpha = 0.8;
GLfloat  tpAlpha = 0.8;
Vector3  spV1 = { -1.0, -1.0, -0.5 };
Vector3  spV2 = {  1.0, -1.0, -0.5 };
Vector3  spV3 = {  1.0,  1.0, -0.5 };
Vector3  spV4 = { -1.0,  1.0, -0.5 };

Vector3  clearColor = { 0.0,  0.0, 0.0 };

// Eye position and direction of view
Vector3 EyePos = {0.0,0.0,-4.0},EyePosOld;
Vector3 EyeDir = {0,0,0}       ,EyeDirOld;
Vector3 ViewPt = {0,0,0}       ,ViewPtOld;
Vector3 ViewUp = {0.0,1.0,0.0}  ,ViewUpOld;

// Eye position and direction of view for Lists Window
Vector3 LEyePos = {0.0,0.0,-4.0},LEyePosOld;
Vector3 LEyeDir = {0,0,0}       ,LEyeDirOld;
Vector3 LViewPt = {0,0.1,0}       ,LViewPtOld;
Vector3 LViewUp = {0.0,1.0,0.0} ,LViewUpOld;

// Function pointer for glutDisplayFunc
void (*viewDisplayFunc)() = NULL;
// Function pointer for glutIdleFunc
void (*viewIdleFunc)() = NULL;
// Function pointer for keyboardFunc
void (*viewKeyboardFunc)(unsigned char, int, int) = NULL;

void viewCreateAuxiliary( const char *title );
void viewCreateLists( const char *title );

// Internal flags
int   splineDefined = false, tourIsGoningOn = false;
int   moveOverSpline= false, firstPoint = false, whereami = 0;
int   projectionFlag= true;
int   navDisplayFlag= true;
int   disableNavDisplayFlag=false;

/* Objects */
GLfloat vertex[7][4] = {
  { 1.0, 1.0, -4.0, 0.40},
  {-2.0, 1.8, -4.5, 0.60},
  {-1.0, 1.0, -4.0, 0.75},
  { 0.0, 0.0,  0.0, 0.88},
  { 1.0, 1.0,  1.0, 0.40},
  {-2.0, 1.8,  0.5, 0.60},
  {-1.0, 1.0,  1.0, 0.75},
};
GLint tetra[2][4] = {
  {0, 1, 2, 3},
  {3, 4, 5, 6}
};

void viewInternalInit(void);
void zsmain(int argc, char **argv);


// Interface fuctions with zsweep --------------------------------------
void frameRot( float xAng = 0, float yAng = 0 ) 
{

  pixptr->Clear(); // Clear image buffer
  pixptr->UpdateViewRotMatrix( xAng, yAng, 0. );

}

void globalDisplayScene(void) {

  sceneptr->oglDisplayScene( edgemode );

}

void zsweepInterface( void )
{
  //static int ponto = 0;
  if( sceneptr->int_isRenderingGoingOn() ) {
    //cout << "ponto = " << ponto++ << "\n";
    //cout << "Disp points .......................\n";
    for( unsigned int k=0 ; k < numPointsAtATime ; k++ )
      sceneptr->int_RenderNextVertex( pixptr, lightptr );
    //cout << "ponto = " << ponto++ << "\n";
    //psIncX( -sceneptr->int_getAdjustX() );
    //psIncY( -sceneptr->int_getAdjustY() );
    psSetZ( sceneptr->int_getCurrentZ() );
    if( renderStep ) {
      renderPause = true;
      sceneptr->int_setRenderPause( renderPause );
    }
  } else {
    //cout << "MMAAXX = " << MMAAXX << endl;
    //cout << "Disp edges.....\n";
    //frameRot();
    if( firstSave )
      {
	pixptr->SavePPM("spx.ppm"); 
	firstSave = false;
      }
    globalDisplayScene();
  }
}


// Demo Functions start here -------------------------------------------

void setCenter( GLfloat centX, GLfloat centY, GLfloat centZ ) {
  centerX=centX; centerY=centY; centerZ=centZ;
}
void setViewBB( GLfloat xi,GLfloat xf,GLfloat yi,GLfloat yf,GLfloat zi,GLfloat zf) {
  float w, h, t; // width, height, thickness of a box
  xmin = xi; xmax = xf;
  ymin = yi; ymax = yf;
  zmin = zi; zmax = zf;
  /* Find the center of the bounding box                                     */
  centerX = (xmin + xmax) / 2.0;
  centerY = (ymin + ymax) / 2.0;
  centerZ = (zmin + zmax) / 2.0;
  /* Create larger bounding box to allow object within view after transform  */
  w = xmax - xmin; h = ymax - ymin; t = zmax - zmin;
  Rad = rad = sqrt(w * w + h * h + t * t) / 2;
  /*
  xmin = centerX - rad; xmax = centerX + rad;
  ymin = centerY - rad; ymax = centerY + rad;
  zmin = centerZ - 3 * rad; zmax = centerZ + 3 * rad;
  */
  bbFlag = true;
}

//---------------------------------------------------------------
// Plane Sweep seting Functions
//---------------------------------------------------------------
void psSetColor( GLfloat r,GLfloat g, GLfloat b ) {
  spColor.x = r;
  spColor.y = g;
  spColor.z = b;
}
void psAlpha( GLfloat alpha ) { spAlpha = alpha; }
GLfloat getAlpha( void ) { return spAlpha; }
void psDefine( GLfloat x1, GLfloat y1, GLfloat z1,
               GLfloat x2, GLfloat y2, GLfloat z2 ) {
  spV1.x = x1; spV1.y = y1; spV1.z = z1;
  spV2.x = x2; spV2.y = y1; spV2.z = z1;
  spV3.x = x2; spV3.y = y2; spV3.z = z1;
  spV4.x = x1; spV4.y = y2; spV4.z = z1;
}
void psSetX( GLfloat inc ) { 
  spV1.x = inc; spV2.x = inc; spV3.x = inc; spV4.x = inc;
}
void psSetY( GLfloat inc ) { 
  spV1.y = inc; spV2.y = inc; spV3.y = inc; spV4.y = inc;
}
void psSetZ( GLfloat inc ) { 
  spV1.z = inc; spV2.z = inc; spV3.z = inc; spV4.z = inc;
}
void psIncXM( GLfloat inc ) { 
  spV2.x += inc; spV3.x += inc;
}
void psIncXm( GLfloat inc ) { 
  spV1.x += inc; spV4.x += inc;
}
void psIncX( GLfloat inc ) { 
  spV1.x += inc; spV2.x += inc; spV3.x += inc; spV4.x += inc;
}
void psIncYM( GLfloat inc ) { 
  spV3.y += inc; spV4.y += inc;
}
void psIncYm( GLfloat inc ) { 
  spV1.y += inc; spV2.y += inc;
}
void psIncY( GLfloat inc ) { 
  spV1.y += inc; spV2.y += inc; spV3.y += inc; spV4.y += inc;
}
void psIncZ( GLfloat inc ) { 
  spV1.z += inc; spV2.z += inc; spV3.z += inc; spV4.z += inc;
}
void turnspOn( void ) { 
  sweepPlaneFlag = true;
}
void turnspOff( void ) { 
  sweepPlaneFlag = false;
}
void disableNavDisplay( void ) {
  disableNavDisplayFlag=true;
}
void enableNavDisplay( void ) {
  disableNavDisplayFlag=false;
}
//---------------------------------------------------------------
// Set functions pointer for glut internal calls
//---------------------------------------------------------------
void setViewDisplayFunction( void (*func)() ) {
  viewDisplayFunc = func;
}

//---------------------------------------------------------------
// Set functions pointer for glut internal calls
//---------------------------------------------------------------
void setViewKeyboardFunction( void (*func)(unsigned char, int, int) ) {
  viewKeyboardFunc = func;
}

//---------------------------------------------------------------
void setViewIdleFunction( void (*func)() ) {
  viewIdleFunc = func;
}


/* ------------------------------------------------------------------------- */
static void getXY(float *cx, float *pt, float *x, float *y)
{
  int w;

  *x = cx[0] * pt[0] + cx[4] * pt[1] + cx[8] * pt[2] + cx[12];
  *y = cx[1] * pt[0] + cx[5] * pt[1] + cx[9] * pt[2] + cx[13];
  w  = (int)( cx[3] * pt[0] + cx[7] * pt[1] + cx[11] * pt[2] + cx[15]);

  *x /= w; *y /= w;
}

/* ------------------------------------------------------------------------- */
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
void display_objects() {

  int i;
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
  for( i=0 ; i<2 ; i++ ) {
    glVertex3f( vertex[tetra[i][0]][0], vertex[tetra[i][0]][1], vertex[tetra[i][0]][2]);
    glVertex3f( vertex[tetra[i][1]][0], vertex[tetra[i][1]][1], vertex[tetra[i][1]][2]);
    glVertex3f( vertex[tetra[i][0]][0], vertex[tetra[i][0]][1], vertex[tetra[i][0]][2]);
    glVertex3f( vertex[tetra[i][2]][0], vertex[tetra[i][2]][1], vertex[tetra[i][2]][2]);
    glVertex3f( vertex[tetra[i][0]][0], vertex[tetra[i][0]][1], vertex[tetra[i][0]][2]);
    glVertex3f( vertex[tetra[i][3]][0], vertex[tetra[i][3]][1], vertex[tetra[i][3]][2]);
    glVertex3f( vertex[tetra[i][1]][0], vertex[tetra[i][1]][1], vertex[tetra[i][1]][2]);
    glVertex3f( vertex[tetra[i][2]][0], vertex[tetra[i][2]][1], vertex[tetra[i][2]][2]);
    glVertex3f( vertex[tetra[i][1]][0], vertex[tetra[i][1]][1], vertex[tetra[i][1]][2]);
    glVertex3f( vertex[tetra[i][3]][0], vertex[tetra[i][3]][1], vertex[tetra[i][3]][2]);
    glVertex3f( vertex[tetra[i][2]][0], vertex[tetra[i][2]][1], vertex[tetra[i][2]][2]);
    glVertex3f( vertex[tetra[i][3]][0], vertex[tetra[i][3]][1], vertex[tetra[i][3]][2]);
  }
  glEnd();

}

/* ------------------------------------------------------------------------- */
/* Display the three axis for position guidance purpose                      */
/* ------------------------------------------------------------------------- */
void display_axis   () {

  glBegin(GL_LINES);

  /* X axis */
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glVertex3f( -xmax , 0, 0 );
    glVertex3f(  xmax , 0, 0 );
    glVertex3f( xmax        ,        0, 0 );
    glVertex3f( xmax-xmax/20, -xmax/40, 0 );
    glVertex3f( xmax        ,        0, 0 );
    glVertex3f( xmax-xmax/20,  xmax/40, 0 );

  /* Y axis */
    glColor4f(0.0, 1.0, 0.0, 1.0);
    glVertex3f( 0, -ymax, 0 );
    glVertex3f( 0,  ymax, 0 );
    glVertex3f(        0,  ymax        , 0 );
    glVertex3f( -ymax/40,  ymax-ymax/20, 0 );
    glVertex3f(        0,  ymax        , 0 );
    glVertex3f(  ymax/40,  ymax-ymax/20, 0 );

  /* Z axis */
    glColor4f(0.0, 0.0, 1.0, 1.0);
    glVertex3f( 0, 0, -zmax );
    glVertex3f( 0, 0,  zmax );
    glVertex3f(        0,        0,  zmax         );
    glVertex3f( -zmax/40,        0,  zmax-zmax/20 );
    glVertex3f(        0,        0,  zmax         );
    glVertex3f(  zmax/40,        0,  zmax-zmax/20 );
    glVertex3f(        0,        0,  zmax         );
    glVertex3f(        0, -zmax/40,  zmax-zmax/20 );
    glVertex3f(        0,        0,  zmax         );
    glVertex3f(        0,  zmax/40,  zmax-zmax/20 );

  glEnd();

}

void dispSweepPlane( void ) 
{
  if( sweepPlaneFlag ) {
    if(solid == 0) // Wireframe
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else // Solid display
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f( spColor.x, spColor.y, spColor.z, spAlpha);
    glBegin(GL_QUADS);
    glVertex3f( spV1.x, spV1.y,  spV1.z);
    glVertex3f( spV2.x, spV2.y,  spV2.z);
    glVertex3f( spV3.x, spV3.y,  spV3.z);
    glVertex3f( spV4.x, spV4.y,  spV4.z);
    glEnd();
  }
  if( targetPlaneFlag ) {
    if(solid == 0) // Wireframe
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else // Solid display
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    GLfloat z;
    sceneptr->int_getTargetZ( z );
    glColor4f( tpColor.x, tpColor.y, tpColor.z, tpAlpha);
    glBegin(GL_QUADS);
    glVertex3f( spV1.x, spV1.y,  z);
    glVertex3f( spV2.x, spV2.y,  z);
    glVertex3f( spV3.x, spV3.y,  z);
    glVertex3f( spV4.x, spV4.y,  z);
    glEnd();
  }
}

static void f_auxDisplay( void )
{
  if( sceneptr->int_newImage() ) {
#if 0
    glutSetWindow( auxWin );
    glutPostRedisplay();
    glutSetWindow( parWin );
#endif
    glutSetWindow( auxWin );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pixptr->textGen();
    pixptr->textMapping();
    glutSwapBuffers();
    sceneptr->int_setOldIMage();
    glutSetWindow( parWin );
  }
}

/* ------------------------------------------------------------------------- */
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
static void aux_display_func() {

  if( !bbFlag ) {
    return;
    //printf( "The VIEW bounding box has not been appropriate set.\n");
    //exit(1);
  }
  glutSetWindow( auxWin );
  glutSetWindow( parWin );
#if 0
  glutSetWindow( auxWin );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  pixptr->textGen();
  pixptr->textMapping();
  glutSwapBuffers();
  sceneptr->int_setOldIMage();
  glutSetWindow( parWin );
#endif
}

/* ------------------------------------------------------------------------- */
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
void sendCube( float x, float z, unsigned int length, float delt ) {

  float len = (float)length/20.0;
  float cr,cg,cb;
  if( len <= 0.2 ) {
    cr = 0.0;
    cg = (len+0.1)/0.6+0.3;
    cb = 0.0;
  } else {
    if( len <= 0.5 ) {
      cr = len+0.5;
      cg = len+0.5;
      cb = 0.0;
    } else {
      cr = len;
      cg = 0.0;
      cb = 0.0;
      //cout << length << " : " << cr << " " << cg << " " << cb << endl;
    }
  }
  //cout << length << " : " << cr << " " << cg << " " << cb << endl;

  len *= 4;
  if( length != 0 )
    glColor3f( cr,cg,cb );
  else
    glColor3f( 0.0,0.0,1.0 );
  glBegin(GL_QUADS);
  glVertex3f( x     , len, z );
  glVertex3f( x+delt, len, z );
  glVertex3f( x+delt, len, z+delt );
  glVertex3f( x     , len, z+delt );
  glEnd();

  glBegin(GL_LINES);
  glVertex3f( x     ,    0.0, z );
  glVertex3f( x     , len, z );
  glVertex3f( x+delt,    0.0, z );
  glVertex3f( x+delt, len, z );
  glVertex3f( x+delt,    0.0, z+delt );
  glVertex3f( x+delt, len, z+delt );
  glVertex3f( x     ,    0.0, z+delt );
  glVertex3f( x     , len, z+delt );
  glEnd();

  /*
  glColor3f( 0.5,0.5,0.5 );
  glBegin(GL_QUADS);
  glVertex3f( -1.0 ,  0.0, -1.0 );
  glVertex3f(  1.0 ,  0.0, -1.0 );
  glVertex3f(  1.0 ,  0.0,  1.0 );
  glVertex3f( -1.0 ,  0.0,  1.0 );
  glEnd();
  */
}
static void lis_display_func() {

  glutSetWindow( lisWin );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f( 1.0, 0.0, 0.0 );
  glLoadIdentity();
  gluLookAt( 2.0, 3.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  unsigned int length;
  float x,z,delt = 2.0/res_lists;
  unsigned int zp = startZPix, max = 0;
  for( z = -1 ; z < 1 ; z = z + delt )
    {
      unsigned int xp = startXPix;
      for( x = -1 ; x < 1 ; x = x + delt )
	{
	  length = pixptr->_scrList->getListLength( xp, zp );
	  if( max < length ) max = length; 
	  if( MMAAXX < length ) MMAAXX = length; 
	  xp++;
	}
      zp++;
    }
  zp = startZPix;
  for( z = -1 ; z < 1 ; z = z + delt )
    {
      unsigned int xp = startXPix;
      for( x = -1 ; x < 1 ; x = x + delt )
	{
	  //cout << "x=" << x << "  z=" << z << "   delta = " << delt << endl;
	  length = (pixptr->_scrList->getListLength( xp, zp ))/2;
	  sendCube( x, z, length, delt );
	  xp++;
	}
      zp++;
    }
  glutSwapBuffers();

}

/* ------------------------------------------------------------------------- */
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
static void display_func() {

  if( !bbFlag ) {
    return;
    //printf( "The VIEW bounding box has not been appropriate set.\n");
    //exit(1);
  }

  glutSetWindow( parWin );

  // Clear the window
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Set model parameters
  //glLoadIdentity();
  gluLookAt(EyePos.x,EyePos.y,EyePos.z,ViewPt.x,ViewPt.y,ViewPt.z,ViewUp.x,ViewUp.y,ViewUp.z);
  //glPushMatrix();

  //display_axis();

  glLoadIdentity();
  // Allow spinning along an axis through the center of the bounding box
  if(spin && circling) {
    if( rotateX && rotateY )
	  glRotatef(angle, vx, vy, 0.0);
    else {
		//cout << "  rotateX = ";
		//if(rotateX) cout << "True\n"; else cout << "False\n";
		//cout << "  rotateY = ";
		//if(rotateY) cout << "True\n"; else cout << "False\n";
      if( rotateX ) glRotatef(angle,  vx, 0.0, 0.0);
      if( rotateY ) glRotatef(angle, 0.0,  vy, 0.0);
	}
  }

  // Allow shifting due to inertia
  if(shift && floating) {
    glTranslatef(vx, vy, 0.0);
  }

  glMultMatrixf(current_matrix);
  glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
  glutPostRedisplay();

  if(solid == 0) // Wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else // Solid display
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Call the external display function if any
  if( dispAxisFlag ) display_axis();
  if( viewDisplayFunc ) viewDisplayFunc();
  dispSweepPlane();

  glutSwapBuffers();

  if( auxCreated   ) f_auxDisplay();

  if( listsCreated ) lis_display_func();
  
#ifdef LINUX
  sleep( sleepTime );
#else
//  Sleep( sleepTime );
#endif

}

/* ------------------------------------------------------------------------- */
/* reshape_func() resets viewport and projection                             */
/* ------------------------------------------------------------------------- */
static void reshape_func(int width, int height) {
  if( !bbFlag ) {
    return;
    //printf( "The VIEW bounding box has not been appropriate set.\n");
    //exit(1);
  }
  view_width = width; view_height = height;
  glViewport(0, 0, (GLsizei) width, (GLsizei) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(xmin, xmax, ymin, ymax, -zmin, -zmax);
  glMatrixMode(GL_MODELVIEW);
}

/* ------------------------------------------------------------------------- */
/* keyboard_func() defines the keyboard controls of the graphics             */
/* ------------------------------------------------------------------------- */
static void keyboard_func(unsigned char key, int mx, int my)
{

  switch(key) {
  case '\\':
    if( numPointsAtATime>1 ) numPointsAtATime--;
    break;
  case '|':
    numPointsAtATime++;
    break;
  case 'I':
    if( auxCreated ) {
      glutDestroyWindow( auxWin );
      currWin = glutGetWindow();
      auxCreated = false;
    } else {
      viewCreateAuxiliary( "ZSweep Image Window" );
      auxCreated = true;
    }
    break;
  case 'L':
    if( listsCreated ) {
      glutDestroyWindow( lisWin );
      currWin = glutGetWindow();
      listsCreated = false;
    } else {
      viewCreateLists( "ZSweep Lists Window" );
      listsCreated = true;
    }
    break;
  case 'j':
    inc4ps = (inc4ps == 0.01)? 0.05:0.01;
    break;
  case 'v':
    psIncXM( -inc4ps );
    break;
  case 'V':
    psIncXM(  inc4ps );
    break;
  case 'b':
    psIncXm(  inc4ps );
    break;
  case 'B':
    psIncXm( -inc4ps );
    break;
  case 'n':
    psIncYM(  inc4ps );
    break;
  case 'N':
    psIncYM( -inc4ps );
    break;
  case 'm':
    psIncYm( -inc4ps );
    break;
  case 'M':
    psIncYm(  inc4ps );
    break;

  case 'g':
    sceneptr->int_RenderInit( pixptr );
    renderPause = false;
    sceneptr->int_setRenderPause( renderPause );
    break;
  case '[':
    if( sleepTime > 0 ) sleepTime-=50;
    cout << "Sleep Time = " << sleepTime << endl;
    break;
  case ']':
    sleepTime+=50;
    cout << "Sleep Time = " << sleepTime << endl;
    break;
  case 'z':
    
    if( !sceneptr->int_switchCentralize() ) {
      GLfloat x0,x1,y0,y1,z0,z1;
      sceneptr->Center();
      sceneptr->MakeBBox();
      sceneptr->getSceneBB( x0, x1, y0, y1,z0, z1 );
      psDefine( x0, x1, y0, y1,z0, z1 );
    }
    
  break;
  case 'a':
    dispAxisFlag = !dispAxisFlag;
    break;
  case ENTER:
    if( !renderStep ) {
      renderStep = renderPause = true;
    } else {
      renderStep = renderPause = false;
    }
    sceneptr->int_setRenderPause( renderPause );
    break;
  case ' ':
    renderPause = !renderPause;
    sceneptr->int_setRenderPause( renderPause );
    break;
  case '=':
    sweepPlaneFlag = !sweepPlaneFlag;
    break;
  case '-':
    targetPlaneFlag = !targetPlaneFlag;
    break;
  case '1':                     // Disable Vertices
    sceneptr->int_switchVertexDisp();
    break;
  case '2':                   // Disable swept vertices
    sceneptr->int_switchNonSweptVertex();
    break;
  case '3':                   // Disable swept vertices
    sceneptr->int_switchSweptVertex();
    break;
  case '4':                     // Displays only forward faces incident to the curr vertex
    sceneptr->int_switchNewfaceDisp();
    break;
  case '5':                  // Displays only forward edges incident to the curr vertex
    sceneptr->int_switchNewedgeDisp();
    break;
  case '6':                     // Display backwards faces incident to the curr vertex
    sceneptr->int_switchOldfaceDisp();
    break;
  case '7':                   // Displays only backward edges incident to the curr vertex
    sceneptr->int_switchOldedgeDisp();
    break;
  case '8':                     // Displays Edges
      edgemode = MARK_ANY;
	  sceneptr->int_switchEdgeDisp(MARK_ANY);
    break;
  case '9':                     // Displays Edges
      edgemode = MARK_BOUNDARY;
	  sceneptr->int_switchEdgeDisp(MARK_BOUNDARY);
    break;
  case ',':                     // Decrease Sweep Plane Alpha
    if( spAlpha > 0.0 ) spAlpha -= .1;
    break;
  case '.':                     // Increase Sweep Plane Alpha
    if( spAlpha < 1.0 ) spAlpha += .1;
    break;
  case '<':                     // Decrease Target Plane Alpha
    if( tpAlpha > 0.0 ) tpAlpha -= .1;
    break;
  case '>':                     // Increase Target Plane Alpha
    if( tpAlpha < 1.0 ) tpAlpha += .1;
    break;
  case 'r':                     /* Rotation                                */
    mouseEnable = 0;
    break;
  case 't':                     /* Translation                             */
    mouseEnable = 1;
    break;
  case 'i':                     /* Zoom in                                 */
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    break;
  case 'o':                     /* Zoom out                                */
    rad /= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    break;
  case 'p' : // Toggle between perpective/orthogonal projection
    projectionFlag = (projectionFlag)? FALSE:TRUE;
    break;
  case 'd':
    solid = !solid;
    break;
  case 'c':
    circling = !circling;
    break;
  case 's':
    spin = !spin;
    break;
  case 'f':
    shift = !shift;
    break;
  case 'R':
    viewInternalInit();
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
    break;
  case 'h':
    cout << "g : Start/Restart Sweeping\n";
    cout << "-----------------------------------------------------------\n";
    cout << "1 : Switch Vertices Display On/Off\n";
    cout << "2 : Switch NonSwept Vertices Display On/Off\n";
    cout << "3 : Switch Swept    Vertices Display On/Off\n";
    cout << "4 : Switch New Faces Display On/Off\n";
    cout << "5 : Switch Edges of New Faces Display On/Off\n";
    cout << "6 : Switch Old Faces Display On/Off\n";
    cout << "7 : Switch Edges of Old Faces Display On/Off\n";
    cout << "9 : Switch Edges Display On/Off\n";
    cout << "a : Switch axis Display On/Off\n";
    cout << "i : Zoom IN\n";
    cout << "o : Zoom OUT\n";
    cout << "r : Rotate Mode\n";
    cout << "s : Switch Spin On/Off\n";
    cout << "t : Translation Mode\n";
    cout << "z : Display centered on current swept vertex\n";
    cout << "-----------------------------------------------------------\n";
    cout << "Sweep Plane Comands\n";
    cout << "= : Switch Sweep   Plane Display On/Off\n";
    cout << "- : Switch TargetZ Plane Display On/Off\n";
    cout << ", : Decrease Sweep  Plane Opacity\n";
    cout << ". : Increase Sweep  Plane Opacity\n";
    cout << "< : Decrease Target Plane Opacity\n";
    cout << "> : Increase Target Plane Opacity\n";
    cout << "[ : Slow down Sweeping\n";
    cout << "] : Speed up Sweeping\n";
    cout << "j : Switch between 0.01 and 0.05 inc/dec steps for SP size.\n";
    cout << "v/V : Dec/Inc positive x sides of the SP\n";
    cout << "b/B : Dec/Inc negative x sides of the SP\n";
    cout << "n/N : Dec/Inc positive y sides of the SP\n";
    cout << "m/N : Dec/Inc negative y sides of the SP\n";
    cout << "-----------------------------------------------------------\n";
    cout << "SPACE : Pause Sweeping\n";
    cout << "ENTER : Step by Step Sweeping\n";
    cout << "h : Show this help\n";
    cout << "q : Quit\n";
    break;
  case 'q':                     /* Quit                                    */
    exit(0);
    break;
  default:
    break;
  }
  if( viewKeyboardFunc ) viewKeyboardFunc( key, mx, my);
  glutPostRedisplay();

}

/* ------------------------------------------------------------------------- */
/* keyboard_func() defines the keyboard controls of the graphics             */
/* ------------------------------------------------------------------------- */
static void lis_keyboard_func(unsigned char key, int mx, int my)
{

  switch(key) {
  case ENTER:
    if( !renderStep ) {
      renderStep = renderPause = true;
    } else {
      renderStep = renderPause = false;
    }
    sceneptr->int_setRenderPause( renderPause );
    break;
  case 'R':
    if( res_lists < RESOLUTION ) res_lists += 4;
    break;
  case 'r':
    if( res_lists > 4 ) res_lists -= 4;
    break;
  case 'i':
    if( startZPix <= RESOLUTION - 5 ) startZPix += 4;
    break;
  case 'm':
    if( startZPix >= 4 ) startZPix -= 4;
    break;
  case 'j':
    if( startXPix >= 4 ) startXPix -= 4;
    break;
  case 'k':
    if( startXPix <= RESOLUTION - 5 ) startXPix += 4;
    break;
  case 'q':
    exit(0);
    break;
  default:
    break;
  }
  glutPostRedisplay();

}

/* ------------------------------------------------------------------------- */
/* mouse_func() defines the mouse controls of the graphics                   */
/* ------------------------------------------------------------------------- */
static void mouse_func(int button, int state, int x, int y) {
  if(button == GLUT_LEFT_BUTTON) {
    if(state == GLUT_DOWN) {
      mouseX = x;
      mouseY = y;
      circling = 0;
      floating = 0;
      angle = 0;
    }
  }
}

/* ------------------------------------------------------------------------- */
/* motion_func() defines the movements with one or more mouse button pressed */
/* ------------------------------------------------------------------------- */
static void motion_func(int x, int y) {
  if(spin  && (mouseEnable == 0))
    circling = 1;
  if(shift  && (mouseEnable == 1))
    floating = 1;

  if(mouseEnable == 0) { /* Rotation */
    vx = mouseY - y;
    vy = mouseX - x;
    mouseX = x;
    mouseY = y;
    if( rotateX && rotateY ) {
      if((angle = fabs(vx)) < fabs(vy)) angle = fabs(vy);
	} else {
		if( rotateX ) angle = fabs(vx);
        if( rotateY ) angle = fabs(vy);
	}

    glLoadIdentity();
    if( rotateX && rotateY )
	  glRotatef(angle, vx, vy, 0.0);
    else {
		//cout << "  rotateX = ";
		//if(rotateX) cout << "True\n"; else cout << "False\n";
		//cout << "  rotateY = ";
		//if(rotateY) cout << "True\n"; else cout << "False\n";
		if( rotateX ) glRotatef(angle,  (vx>0)? 1.0:-1.0, 0.0, 0.0);
        if( rotateY ) glRotatef(angle, 0.0,  (vy>0)? 1.0:-1.0, 0.0);
	}
    //glRotatef(angle, vx, vy, 0.0);
    glMultMatrixf(current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
  } else { /* Translation */
    vx = (x - mouseX) * rad / view_width;
    vy = (mouseY - y) * rad / view_height;
    mouseX = x;
    mouseY = y;

    glLoadIdentity();
    glTranslatef(vx, vy, 0.0);
    glMultMatrixf(current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
  }

  glutPostRedisplay();

}

/* ------------------------------------------------------------------------- */
/* menu_func() defines the menu controls of the graphics                     */
/* ------------------------------------------------------------------------- */
static void menu_func(int value)
{

  switch(value) {
  case 1:                       /* Rotation                                */
    mouseEnable = 0;
    break;
  case 2:                       /* Translation                             */
    mouseEnable = 1;
    break;
  case 3:                       /* Zoom in                                 */
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    break;
  case 4:                       /* Zoom out                                */
    rad /= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    break;
  case 5:
    rotateX = !rotateX;
    /* Create a control menu                                                   */
    glutCreateMenu( menu_func );
    glutAddMenuEntry("Rotate (r)",                   1);
    glutAddMenuEntry("Translate (t)",                2);
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Zoom in (i)",                  3);
    glutAddMenuEntry("Zoom out (o)",                 4);
    glutAddMenuEntry("------------------------",     0);
    if( rotateX )
      glutAddMenuEntry("Toggle Rotation Around X OFF", 5);
    else
      glutAddMenuEntry("Toggle Rotation Around X ON",  5);
    if( rotateY )
      glutAddMenuEntry("Toggle Rotation Around Y OFF", 6);
    else
      glutAddMenuEntry("Toggle Rotation Around Y ON",  6);
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Toggle SOLID / WIREFRAME (d)", 7);
    glutAddMenuEntry("Toggle SPIN ON / OFF (s)",     8);
    glutAddMenuEntry("Toggle SHIFT ON / OFF (f)",    9);
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Create Image Window" ,        10);
    glutAddMenuEntry("Delete Image Window" ,        11);
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Create Lists Window" ,        14); 
    glutAddMenuEntry("Delete Lists Window" ,        15); 
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Reset", 12);
    glutAddMenuEntry("Exit",  13);
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    break;
  case 6:
    rotateY = !rotateY;
    /* Create a control menu                                                   */
    glutCreateMenu( menu_func );
    glutAddMenuEntry("Rotate (r)",                   1);
    glutAddMenuEntry("Translate (t)",                2);
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Zoom in (i)",                  3);
    glutAddMenuEntry("Zoom out (o)",                 4);
    glutAddMenuEntry("------------------------",     0);
    if( rotateX )
      glutAddMenuEntry("Toggle Rotation Around X OFF", 5);
    else
      glutAddMenuEntry("Toggle Rotation Around X ON",  5);
    if( rotateY )
      glutAddMenuEntry("Toggle Rotation Around Y OFF", 6);
    else
      glutAddMenuEntry("Toggle Rotation Around Y ON",  6);
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Toggle SOLID / WIREFRAME (d)", 7);
    glutAddMenuEntry("Toggle SPIN ON / OFF (s)",     8);
    glutAddMenuEntry("Toggle SHIFT ON / OFF (f)",    9);
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Create Image Window" ,        10);
    glutAddMenuEntry("Delete Image Window" ,        11);
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Create Lists Window" ,        14); 
    glutAddMenuEntry("Delete Lists Window" ,        15); 
    glutAddMenuEntry("------------------------",     0);
    glutAddMenuEntry("Reset", 12);
    glutAddMenuEntry("Exit",  13);
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    break;
  case 7:                       /* Toggle between solid and wireframe      */
    solid = !solid;
    break;
  case 8:                       /* Toggle spin ON or OFF                   */
    spin = !spin;
    break;
  case 9:                       /* Toggle shift ON or OFF                  */
    shift = !shift;
    break;
  case 10:                       /* Create image window */
    if( auxCreated ) {
      printf("Auxiliary already created\n");
      break;
    }
    viewCreateAuxiliary( "ZSweep Image Window" );
    auxCreated = true;
    break;
  case 11:                       /* Delete image window */
    if( !auxCreated ) break;
    glutDestroyWindow( auxWin );
    currWin = glutGetWindow();
    auxCreated = false;
    break;
  case 14:                       /* Create lists window */
    if( listsCreated ) {
      printf("Lists Window already created\n");
      break;
    }
    viewCreateLists( "ZSweep Lists Window" );
    listsCreated = true;
    break;
  case 15:                       /* Delete lists window */
    if( !listsCreated ) break;
    glutDestroyWindow( lisWin );
    currWin = glutGetWindow();
    listsCreated = false;
    break;
  case 12:                      /* Reset display                           */
    viewInternalInit();
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
    break;
  case 13:                      /* Exit the program                        */
    exit(-1);
    break;
  default:
    break;
  }
  glutPostRedisplay();

}

//---------------------------------------------------------------------
void viewInternalIdle(void) {

  if( viewIdleFunc ) viewIdleFunc();
  display_func();

}

GLfloat bbSize( void ) {
  float lmax = fabs(spV1.x);
  if( lmax < fabs(spV1.y) ) lmax = fabs(spV1.y);
  if( lmax < fabs(spV1.z) ) lmax = fabs(spV1.z);
  if( lmax < fabs(spV2.x) ) lmax = fabs(spV2.x);
  if( lmax < fabs(spV2.y) ) lmax = fabs(spV2.y);
  if( lmax < fabs(spV2.z) ) lmax = fabs(spV2.z);
  if( lmax < fabs(spV3.x) ) lmax = fabs(spV3.x);
  if( lmax < fabs(spV3.y) ) lmax = fabs(spV3.y);
  if( lmax < fabs(spV3.z) ) lmax = fabs(spV3.z);
  if( lmax < fabs(spV4.x) ) lmax = fabs(spV4.x);
  if( lmax < fabs(spV4.y) ) lmax = fabs(spV4.y);
  if( lmax < fabs(spV4.z) ) lmax = fabs(spV4.z);
  return lmax;
}

/* ------------------------------------------------------------------------- */
/* reshape_func() resets viewport and projection                             */
/* ------------------------------------------------------------------------- */
static void reshape_lists(int width, int height) {

  if( !bbFlag ) {
    return;
    //printf( "The VIEW bounding box has not been appropriate set.\n");
    //exit(1);
  }
  view_width = width; view_height = height;
  glViewport(0, 0, (GLsizei) width, (GLsizei) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glFrustum( -1.0, 1.0, -1.0, 1.0, 1.5, 20.0 );
  //glOrtho(xmin, xmax, ymin, ymax, -zmin, -zmax);
  glOrtho( -1.7, 1.7, -1.7, 2.5, -5.0, 15.0 );
  glMatrixMode(GL_MODELVIEW);

}

//--------------------------------------------------------------------- 
void viewCreateLists( const char *title ) { 
 
  /* Request a window to have an initial position and size                   */ 
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
  glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT ); 
  glutInitWindowPosition( XOFF+WINDOW_WIDTH+BORDER, YOFF+WINDOW_HEIGHT-7 ); 
  lisWin = glutCreateWindow( title ); 
  glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
  glutDisplayFunc( lis_display_func ); 
  glutReshapeFunc( reshape_lists ); 
  glutKeyboardFunc( lis_keyboard_func );
  glutSetWindow( parWin ); 
 
} 
 
/* ------------------------------------------------------------------------- */
/* reshape_func() resets viewport and projection                             */
/* ------------------------------------------------------------------------- */
static void reshape_auxiliary(int width, int height) {

  if( !bbFlag ) {
    return;
    //printf( "The VIEW bounding box has not been appropriate set.\n");
    //exit(1);
  }
  view_width = width; view_height = height;
  glViewport(0, 0, (GLsizei) width, (GLsizei) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(xmin, xmax, ymin, ymax, -zmin, -zmax);
  glMatrixMode(GL_MODELVIEW);

}

//--------------------------------------------------------------------- 
void viewCreateAuxiliary( const char *title ) { 
 
  /* Request a window to have an initial position and size                   */ 
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
  glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT ); 
  glutInitWindowPosition( XOFF+WINDOW_WIDTH+BORDER, YOFF ); 
  auxWin = glutCreateWindow( title ); 
  glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
  glutDisplayFunc( aux_display_func ); 
  glutReshapeFunc( reshape_func ); 
  glutSetWindow( parWin ); 
 
} 
 
//---------------------------------------------------------------------
void viewInternalInit(void) {

  GLfloat bbsize;
  /* General initialization */
  //setViewBB( -bbsize , bbsize , -bbsize , bbsize , -bbsize , bbsize );
  //setViewDisplayFunction( display_objects );
  setViewDisplayFunction( zsweepInterface );
  sceneptr->int_setRenderPause( renderPause );

  spin  = circling = shift = floating = false;
  solid = true;

  // Defining the sweep plane
  GLfloat x0,x1,y0,y1,z0,z1;
  sceneptr->getSceneBB( x0, x1, y0, y1,z0, z1 );
  psDefine( x0, x1, y0, y1,z0, z1 );
  bbsize = bbSize(); 
  setViewBB( -2*bbsize , 2*bbsize , -2*bbsize , 2*bbsize , -2*bbsize , 2*bbsize ); 
  turnspOn();

  if( 0 ) {

// Ambient light
//    GLfloat light_position[]={0.0, 0.0, 5.0, 0.0};
    GLfloat light_ambient[] ={0.0, 0.0, 1.0, 1.};
// Spot
    GLfloat light_position1[]={ 0.0f , 0.0f , 5.0f , 0.0f };
    GLfloat light_ambient1[] ={0.3, 0.3, 0.3, 1.0};
    GLfloat light_diffuse1[] ={1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular1[]={1.0, 1.0, 1.0, 1.0};
    //GLfloat light_s_ang1[]   ={20.0,20.0,20.0, 1.0};

    glEnable(GL_LIGHTING); /* enable lighting */
    glEnable(GL_LIGHT0);   /* enable light 0 */
    glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse1);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);

    glEnable(GL_DEPTH_TEST); /* enable z buffer */
    //glEnable(GL_SMOOTH); /*enable smooth shading */

  }

}

/* ------------------------------------------------------------------------- */
/* Main routine                                                              */
/* ------------------------------------------------------------------------- */
int main( int argc, char **argv ) {

  zsmain( argc, argv );
  sceneptr->int_RenderInit( pixptr );
  //while ( sceneptr->int_RenderNextVertex( pixptr, lightptr ) );

  viewInternalInit();
  /* Initialize the GLUT library and process command line options            */
  glutInit( &argc, argv );

  /* Initialize the display to be a double-buffered and RGBA-mode window     */
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );

  /* Request a window to have an initial position and size                   */
  glutInitWindowPosition( XOFF, YOFF );
  glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );

  /* Create a window with a prescribed name                                  */
  parWin = glutCreateWindow( "ZSweep Algorithm Mechanism" );

  /* Create a control menu                                                   */
  glutCreateMenu( menu_func );
  glutAddMenuEntry("Rotate (r)",                   1);
  glutAddMenuEntry("Translate (t)",                2);
  glutAddMenuEntry("------------------------",     0);
  glutAddMenuEntry("Zoom in (i)",                  3);
  glutAddMenuEntry("Zoom out (o)",                 4);
  glutAddMenuEntry("------------------------",     0);
  glutAddMenuEntry("Toggle Rotation Around X OFF", 5);
  glutAddMenuEntry("Toggle Rotation Around Y OFF", 6);
  glutAddMenuEntry("------------------------",     0);
  glutAddMenuEntry("Toggle SOLID / WIREFRAME (d)", 7);
  glutAddMenuEntry("Toggle SPIN ON / OFF (s)",     8);
  glutAddMenuEntry("Toggle SHIFT ON / OFF (f)",    9);
  glutAddMenuEntry("------------------------",     0);
  glutAddMenuEntry("Create Image Window" ,        10); 
  glutAddMenuEntry("Delete Image Window" ,        11); 
  glutAddMenuEntry("------------------------",     0);
  glutAddMenuEntry("Create Lists Window" ,        14); 
  glutAddMenuEntry("Delete Lists Window" ,        15); 
  glutAddMenuEntry("------------------------",     0);
  glutAddMenuEntry("Reset", 12);
  glutAddMenuEntry("Exit",  13);
  glutAttachMenu( GLUT_RIGHT_BUTTON );

  // Set the current buffer clearing color to be opaque black in RGBA mode
  glShadeModel(GL_FLAT);
  glClearColor( clearColor.x, clearColor.y, clearColor.z, 0.5 );
  glEnable(GL_FLAT); //enable smooth shading
  glEnable(GL_DEPTH_TEST); // enable z buffer
  glEnable(GL_BLEND);
  glDepthMask(GL_TRUE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc ( GL_LEQUAL );

  // Specify the function when the window contents need to be redrawn
  glutDisplayFunc( display_func );

  // Specify the function called when the window is resized or moved
  glutReshapeFunc( reshape_func );

  // Specify the function called when a key is pressed
  glutKeyboardFunc( keyboard_func );

  // Specify the Idle function
  glutIdleFunc( viewInternalIdle );

  // Specify the function called when a mouse button is pressed or released
  glutMouseFunc( mouse_func );

  // Specify the function called when a mouse moves withing the window and
  // one or more mouse buttons is pressed
  glutMotionFunc( motion_func );
/*
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
    rad *= ZOOMFACTOR;
    xmin = centerX - rad; xmax = centerX + rad;
    ymin = centerY - rad; ymax = centerY + rad;
    reshape_func(view_width, view_height);
*/
  // Enter the GLUT processing loop
  glutMainLoop();

}
//------------------------------------------------------------------------------------
