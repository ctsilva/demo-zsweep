/*****************************************************************************/
/*                                                                           */
/* FILE: zs_ZSImage.cpp                                                      */
/*                                                                           */
/* Applied Mathematics and Statistics                                        */
/* State University of New York at Stony Brook                               */
/* Stony Brook, NY 11794                                                     */
/*                                                                           */
/* Created: 09/07/2000                                                       */
/* rfarias@ams.sunysb.edu                                                    */
/*****************************************************************************/

#if 0

#define false 0
#define true  1
/* Some window constant definitions                                          */
#define XOFF             28
#define YOFF            252
#define XOFFAUX         642
#define YOFFAUX         252
#define WINDOW_WIDTH    600
#define WINDOW_HEIGHT   600
#define ZOOMFACTOR      0.8         /* Zoom factor                           */

/* Some other constants                                                      */
#define BLOCKSIZE       512         /* Array block size                      */
#define LINESIZE        4096        /* Maximum number of characters per line */

/* Enable CALLBACK functions                                                 */
/*#define CALLBACK */

#define PARENT    1
#define AUXILIARY 2

/* Standard include files                                                    */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* OpenGL include files                                                      */
#include "view.h"
#include "GraphicsGems.h"

/* Window manager variables */
GLint currWin = 0; /* Current active window 1 == Parent, 2== Auxiliary */
char  auxCreated = false;  /* Indicates if the Auxiliary window exists or not */
char  synchronize = false; /* Enable/disable synchronism between windows */

/* Model related structure*/
struct stModel {
  GLfloat xmin, xmax, ymin, ymax, zmin, zmax;    /* Range of a data set        */
  GLfloat centerX, centerY, centerZ;             /* Estimated object center    */
  GLfloat rad, Rad;                              /* radius of a data set       */
  GLfloat angle, vx, vy;                         /* rotatoin angle and axis    */
};

/* View related structure */
struct _stView {
  GLint   view_width, view_height;               /* Range of viewport          */
  GLint   mouseX, mouseY;                        /* Initial mouse position     */
  GLint   mouseEnable;                           /* 0 for rot, 1 for trans     */
  GLint   solid;                                 /* Generate solid if nonzero  */
  GLint   spin, circling;                        /* Toggle spin on \ off       */
  GLint   shift, floating;                       /* Toggle shift on \ off      */
  char    bbFlag;                                /* Indicates if bb has been set properly */
} stView;

/* parModel -> Parent window model
   auxModel -> Ausiliary window model
   genModel -> General model for sychroniztion */
stModel parModel, auxModel, genModel;
stView  parView,  auxView,  genView;
GLint    objNum = 0;

/* Keep current MODELVIEW matrix                                             */
GLfloat *current_matrix,
aux_matrix[16] = 
        {1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 1.0},
par_matrix[16] = 
        {1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 1.0};

// Variables for the sweep plane animation
char sweepplaneFlag = TRUE;
Vector3  spColor = { 0.0, 0.0, 1.0 };
GLfloat  spAlpha = 0.50;
Vector3  spV1 = { -1.5, -1.5,  0.5 };
Vector3  spV2 = {  1.5, -1.5,  0.5 };
Vector3  spV3 = {  1.5,  1.5, -0.5 };
Vector3  spV4 = { -1.5,  1.5, -0.5 };
// Function pointer for glutDisplayFunc
void (*viewDisplayFunc)() = NULL;
// Function pointer for glutIdleFunc
void (*viewIdleFunc)() = NULL;
// Function pointer for keyboardFunc
void (*viewKeyboardFunc)(unsigned char, int, int) = NULL;
// Internal flags
int   disableNavDisplayFlag=false;

/* Objects */
GLfloat vertex[5][4] = {
  {3.0, 1.0, 2.0, 0.40},
  {2.7, 1.8, 1.0, 0.60},
  {2.8, 2.0, 2.3, 0.75},
  {2.3, 2.3, 2.1, 0.88},
  {2.5, 3.0, 2.2, 1.20}
};
GLint tetra[2][4] = {
  {0, 1, 2, 3},
  {1, 2, 3, 4}
};
 
/* Prototypes --------------------------------------------------------------------*/
void viewCreateAuxiliary( char *title );
static void CALLBACK menu_func_Auxiliary(int value);

/* Functions ---------------------------------------------------------------------*/

void setMatrix( GLfloat *a, GLfloat *b ) {
  GLint i;
  for( i=0; i<16 ; i++ ) a[i] = b[i];
}
void setModelAB( stModel& a, stModel& b ) {
  a.xmin=b.xmin; a.xmax=b.xmax;
  a.ymin=b.ymin; a.ymax=b.ymax;
  a.zmin=b.zmin; a.zmax=b.zmax;
  a.centerX=b.centerX; a.centerY=b.centerY; a.centerZ=b.centerZ;
  a.rad=b.rad; a.Rad=b.Rad;
  a.angle=b.angle; a.vx=b.vx; a.vy=b.vy;
}
void setViewAB( stView& a, stView& b) {
  a.view_width=b.view_width; a.view_height=b.view_height;
  a.mouseX=b.mouseX; a.mouseY=b.mouseY;
  a.solid =b.solid;
  a.spin  =b.spin;   a.circling=b.circling;
  a.shift =b.shift;  a.floating=b.floating;
  a.bbFlag=b.bbFlag;
  a.mouseEnable=b.mouseEnable;
}
void execute( void (* func)() ) {
  func();
}
void setCenter( GLfloat centX, GLfloat centY, GLfloat centZ ) {
  parModel.centerX=centX; parModel.centerY=centY; parModel.centerZ=centZ;
}
void setViewBB( GLfloat xi,GLfloat xf,GLfloat yi,GLfloat yf,GLfloat zi,GLfloat zf) {
  float w, h, t; // width, height, thickness of a box
  genModel.xmin = xi; genModel.xmax = xf;
  genModel.ymin = yi; genModel.ymax = yf;
  genModel.zmin = zi; genModel.zmax = zf;
  /* Find the center of the bounding box                                     */
  genModel.centerX = (genModel.xmin + genModel.xmax) / 2.0;
  genModel.centerY = (genModel.ymin + genModel.ymax) / 2.0;
  genModel.centerZ = (genModel.zmin + genModel.zmax) / 2.0;
  /* Create larger bounding box to allow object within view after transform  */
  w = genModel.xmax - genModel.xmin; h = genModel.ymax - genModel.ymin;
  t = genModel.zmax - genModel.zmin;
  genModel.Rad = genModel.rad = sqrt(w * w + h * h + t * t) / 2;
  genModel.xmin = genModel.centerX - genModel.rad;
  genModel.xmax = genModel.centerX + genModel.rad;
  genModel.ymin = genModel.centerY - genModel.rad;
  genModel.ymax = genModel.centerY + genModel.rad;
  genModel.zmin = genModel.centerZ - 3 * genModel.rad;
  genModel.zmax = genModel.centerZ + 3 * genModel.rad;
  genView.bbFlag = true;
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
  spV3.x = x2; spV3.y = y2; spV3.z = z2;
  spV4.x = x1; spV4.y = y2; spV4.z = z2;
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
void psIncX( GLfloat inc ) { 
  spV1.x += inc; spV2.x += inc; spV3.x += inc; spV4.x += inc;
}
void psIncY( GLfloat inc ) { 
  spV1.y += inc; spV2.y += inc; spV3.y += inc; spV4.y += inc;
}
void psIncZ( GLfloat inc ) { 
  spV1.z += inc; spV2.z += inc; spV3.z += inc; spV4.z += inc;
}
void turnspOn( void ) { 
  sweepplaneFlag = true;
}
void turnspOff( void ) { 
  sweepplaneFlag = false;
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

/* DISPLAY */

/* ------------------------------------------------------------------------- */
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
void disp_sub_function() {

  int i,j;
  glColor3f(1.0, 0.0, 1.0);
  glBegin(GL_LINES);
  for( i=objNum ; i<=objNum ; i++ ) {
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
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
void display_objects() {

  int i,j;
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
void display_axis( int calledby ) {

  stModel *m;
  switch( calledby ) {
  case PARENT :
    m = &parModel;
    break;
  case AUXILIARY :
    m = &auxModel;
    break;
  default : return;
  }
  glBegin(GL_LINES);
  /* X axis */
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f( -m->xmax , m->centerY, m->centerZ );
    glVertex3f(  m->xmax , m->centerY, m->centerZ );

    glVertex3f( m->xmax , m->centerY, m->centerZ );
    glVertex3f( m->xmax-(m->xmax-m->centerX)/20, m->centerY-(m->xmax-m->centerX)/40, m->centerZ );
    glVertex3f( m->xmax , m->centerY, m->centerZ );
    glVertex3f( m->xmax-(m->xmax-m->centerX)/20, m->centerY+(m->xmax-m->centerX)/40, m->centerZ );
  /* Y axis */
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f( m->centerX, -m->ymax, m->centerZ );
    glVertex3f( m->centerX,  m->ymax, m->centerZ );

    glVertex3f( m->centerX,  m->ymax, m->centerZ );
    glVertex3f( m->centerX-(m->ymax-m->centerY)/40,  m->ymax-(m->ymax-m->centerY)/20, m->centerZ );
    glVertex3f( m->centerX,  m->ymax, m->centerZ );
    glVertex3f( m->centerX+(m->ymax-m->centerY)/40,  m->ymax-(m->ymax-m->centerY)/20, m->centerZ );
  /* Z axis */
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f( m->centerX, m->centerY, -m->zmax );
    glVertex3f( m->centerX, m->centerY,  m->zmax );

    glVertex3f( m->centerX, m->centerY,  m->zmax );
    glVertex3f( m->centerX-(m->xmax-m->centerX)/40, m->centerY,  m->zmax-(m->zmax-m->centerZ)/20 );
    glVertex3f( m->centerX, m->centerY,  m->zmax );
    glVertex3f( m->centerX+(m->xmax-m->centerX)/40, m->centerY,  m->zmax-(m->zmax-m->centerZ)/20 );
    glVertex3f( m->centerX, m->centerY,  m->zmax );
    glVertex3f( m->centerX, m->centerY-(m->xmax-m->centerX)/40,  m->zmax-(m->zmax-m->centerZ)/20 );
    glVertex3f( m->centerX, m->centerY,  m->zmax );
    glVertex3f( m->centerX, m->centerY+(m->xmax-m->centerX)/40,  m->zmax-(m->zmax-m->centerZ)/20 );
  glEnd();

}

/* ------------------------------------------------------------------------- */
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
static void display_func( int calledby ) {

  stModel *m;
  stView  *v;

  switch( calledby ) {
  case PARENT :
    current_matrix = &par_matrix[0];
    m = &parModel;
    v = &parView;
    if( !parView.bbFlag ) return;
    break;
  case AUXILIARY :
    current_matrix = &aux_matrix[0];
    m = &auxModel;
    v = &auxView;
    if( !auxView.bbFlag ) return;
    break;
  default : return;
  }
  /* Clear the window                                                        */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  display_axis(calledby);

  /* Allow spinning along an axis through the center of the bounding box     */
  if(v->spin && v->circling) {
    glLoadIdentity();
    glTranslatef(m->centerX, m->centerY, m->centerZ);
    glRotatef(m->angle, m->vx, m->vy, 0.0);
    glTranslatef(-m->centerX, -m->centerY, -m->centerZ);
    glMultMatrixf(current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
    glutPostRedisplay();
  }

  /* Allow shifting due to inertia                                           */
  if(v->shift && v->floating) {
    glLoadIdentity();
    glTranslatef(m->vx, m->vy, 0.0);
    glMultMatrixf(current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
    glutPostRedisplay();
  }

  if(v->solid == 0) /* Wireframe */
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else /* Solid display */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Call the external display function if any
  if( viewDisplayFunc ) viewDisplayFunc();

}

/* ------------------------------------------------------------------------- */
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
static void CALLBACK display_Parent() {
  GLint w = PARENT;
  if( glutGetWindow() != PARENT ) {
    w = glutGetWindow();
    glutSetWindow( PARENT );
  }
  display_func(PARENT);
  display_objects();
  glutSwapBuffers();
  if( w != PARENT ) glutSetWindow( w );
}

/* ------------------------------------------------------------------------- */
/* display_func() sends the compiled display lists to screen                 */
/* ------------------------------------------------------------------------- */
static void CALLBACK display_Auxiliary() {
  GLint w = AUXILIARY;
  if( glutGetWindow() != AUXILIARY ) {
    w = glutGetWindow();
    glutSetWindow( AUXILIARY );
  }
  display_func(AUXILIARY);
  disp_sub_function();
  glutSwapBuffers();
  if( w != AUXILIARY ) glutSetWindow( w );
}

/* RESHAPE */

/* ------------------------------------------------------------------------- */
/* reshape_func() resets viewport and projection                             */
/* ------------------------------------------------------------------------- */
static void reshape_func(int calledby, int width, int height) {

  switch( calledby ) {
  case PARENT: /* Parent Window */
    if( !parView.bbFlag ) return;
    parView.view_width = width; parView.view_height = height;
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( parModel.xmin, parModel.xmax, parModel.ymin, parModel.ymax,
            -parModel.zmin, -parModel.zmax);
    break;
  case AUXILIARY: /* Auxiliary Window */
    if( !auxView.bbFlag ) return;
    auxView.view_width = width; auxView.view_height = height;
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( auxModel.xmin, auxModel.xmax, auxModel.ymin, auxModel.ymax,
            -auxModel.zmin, -auxModel.zmax);
    break;
  default: return; /* Do nothing */
  }
  glMatrixMode(GL_MODELVIEW);
}

/* ------------------------------------------------------------------------- */
/* reshape_func() resets viewport and projection                             */
/* ------------------------------------------------------------------------- */
static void CALLBACK reshape_Parent(int width, int height) {

  reshape_func( PARENT, width, height );

}

/* ------------------------------------------------------------------------- */
/* reshape_func() resets viewport and projection                             */
/* ------------------------------------------------------------------------- */
static void CALLBACK reshape_Auxiliary(int width, int height) {

  reshape_func( AUXILIARY, width, height );

}

/* KEYBOARD */

/* ------------------------------------------------------------------------- */
/* keyboard_func() defines the keyboard controls of the graphics             */
/* ------------------------------------------------------------------------- */
static void CALLBACK keyboard_Parent(unsigned char key, int mx, int my) {
  GLint   i;
  GLfloat x, y;

  switch(key) {
  case 'h': /* Help */
    printf("<r> : Enable/Disable Rotation\n");
    printf("<t> : Enable/Disable Translation\n");
    printf("<i> : Zoom In\n");
    printf("<o> : Zoom Out\n");
    printf("<d> : Wireframe/Solid Faces\n");
    printf("<s> : Enable/Disable Continuous Spin\n");
    printf("<f> : Enable/Disable Continuous Translation\n");
    printf("<q> : Exit\n");
    break;
  case 'r':                     /* Rotation                                */
    parView.mouseEnable = 0;
    break;
  case 't':                     /* Translation                             */
    parView.mouseEnable = 1;
    break;
  case 'i':                     /* Zoom in                                 */
    parModel.rad *= ZOOMFACTOR;
    parModel.xmin = parModel.centerX - parModel.rad;
    parModel.xmax = parModel.centerX + parModel.rad;
    parModel.ymin = parModel.centerY - parModel.rad;
    parModel.ymax = parModel.centerY + parModel.rad;
    reshape_func(PARENT, parView.view_width, parView.view_height);
    break;
  case 'o':                     /* Zoom out                                */
    parModel.rad /= ZOOMFACTOR;
    parModel.xmin = parModel.centerX - parModel.rad;
    parModel.xmax = parModel.centerX + parModel.rad;
    parModel.ymin = parModel.centerY - parModel.rad;
    parModel.ymax = parModel.centerY + parModel.rad;
    reshape_func(PARENT, parView.view_width, parView.view_height);
    break;
  case 'd':
    parView.solid = !parView.solid;
    break;
  case 's':
    parView.spin  = !parView.spin;
    break;
  case 'f':
    parView.shift = !parView.shift;
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
static void CALLBACK keyboard_Auxiliary(unsigned char key, int mx, int my) {
  GLint   i;
  GLfloat x, y;

  switch(key) {
  case 'h': /* Help */
    printf("<r> : Enable/Disable Rotation\n");
    printf("<t> : Enable/Disable Translation\n");
    printf("<i> : Zoom In\n");
    printf("<o> : Zoom Out\n");
    printf("<d> : Wireframe/Solid Faces\n");
    printf("<s> : Enable/Disable Continuous Spin\n");
    printf("<f> : Enable/Disable Continuous Translation\n");
    printf("<q> : Exit\n");
    break;
  case 'r':                     /* Rotation                                */
    auxView.mouseEnable = 0;
    break;
  case 't':                     /* Translation                             */
    auxView.mouseEnable = 1;
    break;
  case 'i':                     /* Zoom in                                 */
    auxModel.rad *= ZOOMFACTOR;
    auxModel.xmin = auxModel.centerX - auxModel.rad;
    auxModel.xmax = auxModel.centerX + auxModel.rad;
    auxModel.ymin = auxModel.centerY - auxModel.rad;
    auxModel.ymax = auxModel.centerY + auxModel.rad;
    reshape_func(AUXILIARY, auxView.view_width, auxView.view_height);
    break;
  case 'o':                     /* Zoom out                                */
    auxModel.rad /= ZOOMFACTOR;
    auxModel.xmin = auxModel.centerX - auxModel.rad;
    auxModel.xmax = auxModel.centerX + auxModel.rad;
    auxModel.ymin = auxModel.centerY - auxModel.rad;
    auxModel.ymax = auxModel.centerY + auxModel.rad;
    reshape_func(AUXILIARY, auxView.view_width, auxView.view_height);
    break;
  case 'd':
    auxView.solid = !auxView.solid;
    break;
  case 's':
    auxView.spin  = !auxView.spin;
    break;
  case 'f':
    auxView.shift = !auxView.shift;
    break;
  case 'n':
    if( objNum == 1 ) objNum = 0;
    else objNum++;
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

/* MOUSE */

/* ------------------------------------------------------------------------- */
/* mouse_func() defines the mouse controls of the graphics                   */
/* ------------------------------------------------------------------------- */
static void CALLBACK mouse_Parent(int button, int state, int x, int y) {
  if(button == GLUT_LEFT_BUTTON) {
    if(state == GLUT_DOWN) {
      parView.mouseX = x;
      parView.mouseY = y;
      parView.circling = 0;
      parView.floating = 0;
    }
  }
}

/* ------------------------------------------------------------------------- */
/* mouse_func() defines the mouse controls of the graphics                   */
/* ------------------------------------------------------------------------- */
static void CALLBACK mouse_Auxiliary(int button, int state, int x, int y) {
  if(button == GLUT_LEFT_BUTTON) {
    if(state == GLUT_DOWN) {
      auxView.mouseX = x;
      auxView.mouseY = y;
      auxView.circling = 0;
      auxView.floating = 0;
    }
  }
}

/* ------------------------------------------------------------------------- */
/* motion_func() defines the movements with one or more mouse button pressed */
/* ------------------------------------------------------------------------- */
static void motion_func(int calledby, int x, int y) {
  stModel *m;
  stView  *v;
  switch( calledby ) {
  case PARENT :
    current_matrix = &par_matrix[0];
    m = &parModel;
    v = &parView;
    break;
  case AUXILIARY :
    current_matrix = &aux_matrix[0];
    m = &auxModel;
    v = &auxView;
    break;
  default : return;
  }
  if(v->spin  && (v->mouseEnable == 0))
    v->circling = 1;
  if(v->shift  && (v->mouseEnable == 1))
    v->floating = 1;

  if(v->mouseEnable == 0) { /* Rotation */
    m->vx = y - v->mouseY;
    m->vy = x - v->mouseX;
    v->mouseX = x;
    v->mouseY = y;
    if((m->angle = fabs(m->vx)) < fabs(m->vy)) m->angle = fabs(m->vy);

    glLoadIdentity();
    glTranslatef(m->centerX, m->centerY, m->centerZ);
    glRotatef(m->angle, m->vx, m->vy, 0.0);
    glTranslatef(-m->centerX, -m->centerY, -m->centerZ);
    glMultMatrixf(current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
  } else { /* Translation */
    m->vx = (x - v->mouseX) * m->rad / v->view_width;
    m->vy = (v->mouseY - y) * m->rad / v->view_height;
    v->mouseX = x;
    v->mouseY = y;

    glLoadIdentity();
    glTranslatef(m->vx, m->vy, 0.0);
    glMultMatrixf(current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
  }
  glutPostRedisplay();
  if( synchronize ) {
    switch( calledby ) {
    case PARENT :
      setModelAB( auxModel, parModel );
      setViewAB(  auxView,  parView  );
      setMatrix(  aux_matrix, par_matrix );
      glutSetWindow( AUXILIARY );
      glutPostRedisplay();
      display_func( AUXILIARY );
      glutSetWindow( PARENT );
      break;
    case AUXILIARY :
      setModelAB( parModel, auxModel );
      setViewAB(  parView,  auxView  );
      setMatrix(  par_matrix, aux_matrix );
      glutSetWindow( PARENT );
      glutPostRedisplay();
      display_func( PARENT );
      glutSetWindow( AUXILIARY );
      break;
    default : return;
    }
  }
}

/* ------------------------------------------------------------------------- */
/* motion_func() defines the movements with one or more mouse button pressed */
/* ------------------------------------------------------------------------- */
static void CALLBACK mouse_motion_Parent(int x, int y) {
  motion_func(PARENT, x, y);
}

/* ------------------------------------------------------------------------- */
/* motion_func() defines the movements with one or more mouse button pressed */
/* ------------------------------------------------------------------------- */
static void CALLBACK mouse_motion_Auxiliary(int x, int y) {
  motion_func(AUXILIARY, x, y);
}

/* ------------------------------------------------------------------------- */
/* mouse-entry() : seletc as current window the one the mouse just entered   */
/* ------------------------------------------------------------------------- */
static void CALLBACK mouse_entry_Parent( int status ) {

  if( status == GLUT_ENTERED ) {
    currWin = glutGetWindow();
    /*glutSetWindow( currWin );*/
    glutPostRedisplay();
  } else {
    if( glutGetWindow() ) {
      currWin = glutGetWindow();
    }
  }
}

/* ------------------------------------------------------------------------- */
/* mouse-entry() : seletc as current window the one the mouse just entered   */
/* ------------------------------------------------------------------------- */
static void CALLBACK mouse_entry_Auxiliary( int status ) {

  if( status == GLUT_ENTERED ) {
    currWin = glutGetWindow();
    /*glutSetWindow( currWin );*/
    glutPostRedisplay();
  } else {
    if( glutGetWindow() ) {
      currWin = glutGetWindow();
    }
  }
}

/* MENU */

/* ------------------------------------------------------------------------- */
/* menu_func() defines the menu controls of the graphics                     */
/* ------------------------------------------------------------------------- */
static void CALLBACK menu_func_Parent(int value) {
  GLint   i;
  GLfloat x, y;
  current_matrix = &par_matrix[0];
  switch(value) {
  case 1:                       /* Rotation                                */
    parView.mouseEnable = 0;
    break;
  case 2:                       /* Translation                             */
    parView.mouseEnable = 1;
    break;
  case 3:                       /* Zoom in                                 */
    parModel.rad *= ZOOMFACTOR;
    parModel.xmin = parModel.centerX - parModel.rad;
    parModel.xmax = parModel.centerX + parModel.rad;
    parModel.ymin = parModel.centerY - parModel.rad;
    parModel.ymax = parModel.centerY + parModel.rad;
    reshape_func(PARENT, parView.view_width, parView.view_height);
    break;
  case 4:                       /* Zoom out                                */
    parModel.rad /= ZOOMFACTOR;
    parModel.xmin = parModel.centerX - parModel.rad;
    parModel.xmax = parModel.centerX + parModel.rad;
    parModel.ymin = parModel.centerY - parModel.rad;
    parModel.ymax = parModel.centerY + parModel.rad;
    reshape_func(PARENT, parView.view_width, parView.view_height);
    break;
  case 5:                       /* Create a new window */
    if( auxCreated ) {
      printf("Auxiliary already created\n");
      break;
    }
    viewCreateAuxiliary( "Auxiliary Window" );
    auxCreated = true;
    break;
  case 6:                       /* Delete current window */
    if( !auxCreated ) break;
    glutDestroyWindow( AUXILIARY );
    currWin = glutGetWindow();
    auxCreated = false;
    synchronize = false;
    break;
  case 7:                       /* Toggle between solid and wireframe      */
    parView.solid = !parView.solid;
    break;
  case 8:                       /* Toggle spin ON or OFF                   */
    parView.spin = !parView.spin;
    break;
  case 9:                       /* Toggle shift ON or OFF                  */
    parView.shift = !parView.shift;
    break;
  case 10:                      /* Reset display                           */
    parModel.rad = parModel.Rad;
    parModel.xmin = parModel.centerX - parModel.rad;
    parModel.xmax = parModel.centerX + parModel.rad;
    parModel.ymin = parModel.centerY - parModel.rad;
    parModel.ymax = parModel.centerY + parModel.rad;
    reshape_func(AUXILIARY, parView.view_width, parView.view_height);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
    parView.circling = 0;
    parView.floating = 0;
    if( synchronize ) {
      setModelAB( auxModel, parModel );
      setViewAB(  auxView,  parView  );
      setMatrix(  aux_matrix, par_matrix );
    }
    break;
  case 11:                      /* Exit the program                        */
    exit(-1);
    break;
  case 12: /* Synchronize windows */
    if( !auxCreated ) { /* Do nothing */
      printf("Auxiliary Window doesn't exist.\n");
      break;
    }
    synchronize = !synchronize;
    if( !synchronize ) {
      printf("Assincronous movements\n");
      break;
    }
    printf("Sincronous movements\n");
    setModelAB( auxModel, parModel );
    setViewAB(  auxView,  parView  );
    setMatrix(  aux_matrix, par_matrix );
    break;
  default:
    break;
  }
  glutPostRedisplay();

}

/* ------------------------------------------------------------------------- */
/* menu_func() defines the menu controls of the graphics                     */
/* ------------------------------------------------------------------------- */
static void CALLBACK menu_func_Auxiliary(int value) {
  GLint   i;
  GLfloat x, y;
  current_matrix = &aux_matrix[0];
  switch(value) {
  case 1:                       /* Rotation                                */
    auxView.mouseEnable = 0;
    break;
  case 2:                       /* Translation                             */
    auxView.mouseEnable = 1;
    break;
  case 3:                       /* Zoom in                                 */
    auxModel.rad *= ZOOMFACTOR;
    auxModel.xmin = auxModel.centerX - auxModel.rad;
    auxModel.xmax = auxModel.centerX + auxModel.rad;
    auxModel.ymin = auxModel.centerY - auxModel.rad;
    auxModel.ymax = auxModel.centerY + auxModel.rad;
    reshape_func(AUXILIARY, auxView.view_width, auxView.view_height);
    break;
  case 4:                       /* Zoom out                                */
    auxModel.rad /= ZOOMFACTOR;
    auxModel.xmin = auxModel.centerX - auxModel.rad;
    auxModel.xmax = auxModel.centerX + auxModel.rad;
    auxModel.ymin = auxModel.centerY - auxModel.rad;
    auxModel.ymax = auxModel.centerY + auxModel.rad;
    reshape_func(AUXILIARY, auxView.view_width, auxView.view_height);
    break;
  case 5: /* Synchronize windows */
    if( !auxCreated ) break; /* Do nothing */
    synchronize = !synchronize;
    if( !synchronize ) {
      printf("Assincronous movements\n");
      break;
    }
    printf("Sincronous movements\n");
    setModelAB( parModel, auxModel );
    setViewAB(  parView,  auxView  );
    setMatrix(  par_matrix, aux_matrix );
    break;
  case 6:                       /* Delete current window */
    if( !auxCreated ) break;
    glutDestroyWindow( AUXILIARY );
    glutSetWindow( PARENT );
    auxCreated = false;
    synchronize = false;
    currWin = PARENT;
    break;
  case 7:                       /* Toggle between solid and wireframe      */
    auxView.solid = !auxView.solid;
    break;
  case 8:                       /* Toggle spin ON or OFF                   */
    auxView.spin  = !auxView.spin;
    break;
  case 9:                       /* Toggle shift ON or OFF                  */
    auxView.shift = !auxView.shift;
    break;
  case 10:                      /* Reset display                           */
    auxModel.rad = auxModel.Rad;
    auxModel.xmin = auxModel.centerX - auxModel.rad;
    auxModel.xmax = auxModel.centerX + auxModel.rad;
    auxModel.ymin = auxModel.centerY - auxModel.rad;
    auxModel.ymax = auxModel.centerY + auxModel.rad;
    reshape_func(AUXILIARY, auxView.view_width, auxView.view_height);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
    auxView.circling = 0;
    auxView.floating = 0;
    if( synchronize ) {
      setModelAB( parModel, auxModel );
      setViewAB(  parView,  auxView  );
      setMatrix(  par_matrix, aux_matrix );
    }
    break;
  case 11:                      /* Exit the program                        */
    exit(-1);
    break;
  default:
    break;
  }
  glutPostRedisplay();

}

/* SETUP */

//---------------------------------------------------------------------
void viewIdle_Parent(void) {

  Vector3 vp,vd,vu,vp1;
  if( viewIdleFunc ) viewIdleFunc();
  if( parView.spin  || parView.circling || parView.shift || parView.floating )
    display_Parent();

}

//---------------------------------------------------------------------
void viewIdle_Auxiliary(void) {

  Vector3 vp,vd,vu,vp1;
  if( !auxCreated ) return;
  if( viewIdleFunc ) viewIdleFunc();
  if( auxView.spin  || auxView.circling || auxView.shift || auxView.floating )
    display_Auxiliary();

}

//---------------------------------------------------------------------
void viewInternalInit(void) {

  current_matrix = &par_matrix[0];
  /* General initialization */
  setViewBB( 2.1 , 3.1 , 0.9 , 3.1 , 0.9 , 2.1 );
  genView.solid = genView.spin  = genView.circling = genView.shift = genView.floating = false;
  setModelAB( parModel, genModel);
  setViewAB(  parView, genView);
  setModelAB( auxModel, genModel);
  setViewAB(  auxView, genView);

}

//---------------------------------------------------------------------
void viewSetCallBacksParent() {

  /* General Display function                                                */
  glutDisplayFunc( display_Parent );

  /* Set the current buffer clearing color to be opaque black in RGBA mode   */
  glClearColor( 0.0, 0.0, 0.0, 1.0 );

  /* Specify the function called when the window is resized or moved         */
  glutReshapeFunc( reshape_Parent );

  /* Specify the function called when a key is pressed                       */
  glutKeyboardFunc( keyboard_Parent );

  /* Specify the Idle function                                               */
  glutIdleFunc( viewIdle_Parent );

  /* Specify the function called when a mouse button is pressed or released  */
  glutMouseFunc( mouse_Parent );
  /* Specify the function called when a mouse moves withing the window and   */
  /* one or more mouse buttons is pressed                                    */
  glutMotionFunc( mouse_motion_Parent );
  glutEntryFunc( mouse_entry_Parent );

}

//---------------------------------------------------------------------
void viewCreateParent( char *title ) {

  /* Request a window to have an initial position and size                   */
  glutInitWindowPosition( XOFF, YOFF );
  glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );

  currWin = glutCreateWindow( title );

  viewSetCallBacksParent();

  /* Create a control menu                                                   */
  glutCreateMenu( menu_func_Parent );
  glutAddMenuEntry("Rotate (r)"              , 1);
  glutAddMenuEntry("Translate (t)"           , 2);
  glutAddMenuEntry("------------------------", 0);
  glutAddMenuEntry("Zoom in (i)"             , 3);
  glutAddMenuEntry("Zoom out (o)"            , 4);
  glutAddMenuEntry("------------------------", 0);
  glutAddMenuEntry("Synchronize Windows"     ,12);
  glutAddMenuEntry("Create Auxiliary Window" , 5);
  glutAddMenuEntry("Delete Auxiliary Window" , 6);
  glutAddMenuEntry("------------------------", 0);
  glutAddMenuEntry("Toggle SOLID / WIREFRAME (d)", 7);
  glutAddMenuEntry("Toggle SPIN ON / OFF (s)"    , 8);
  glutAddMenuEntry("Toggle SHIFT ON / OFF (f)"   , 9);
  glutAddMenuEntry("------------------------"    , 0);
  glutAddMenuEntry("Reset" , 10);
  glutAddMenuEntry("Exit"  , 11);
  glutAttachMenu( GLUT_RIGHT_BUTTON );

}

//---------------------------------------------------------------------
void viewSetCallBacksAuxiliary() {

  /* General Display function                                                */
  glutDisplayFunc( display_Auxiliary );

  /* Set the current buffer clearing color to be opaque black in RGBA mode   */
  glClearColor( 0.0, 0.0, 0.0, 1.0 );

  /* Specify the function called when the window is resized or moved         */
  glutReshapeFunc( reshape_Auxiliary );

  /* Specify the function called when a key is pressed                       */
  glutKeyboardFunc( keyboard_Auxiliary );

  /* Specify the Idle function                                               */
  glutIdleFunc( viewIdle_Auxiliary );

  /* Specify the function called when a mouse button is pressed or released  */
  glutMouseFunc( mouse_Auxiliary );
  /* Specify the function called when a mouse moves withing the window and   */
  /* one or more mouse buttons is pressed                                    */
  glutMotionFunc( mouse_motion_Auxiliary );
  glutEntryFunc( mouse_entry_Auxiliary );

}

//---------------------------------------------------------------------
void viewCreateAuxiliary( char *title ) {

  /* Request a window to have an initial position and size                   */
  glutInitWindowPosition( XOFFAUX, YOFFAUX );
  glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );

  currWin = glutCreateWindow( title );
  viewSetCallBacksAuxiliary();

  /* Create a control menu                                                   */
  glutCreateMenu( menu_func_Auxiliary );
  glutAddMenuEntry("Rotate (r)"              , 1);
  glutAddMenuEntry("Translate (t)"           , 2);
  glutAddMenuEntry("------------------------", 0);
  glutAddMenuEntry("Zoom in (i)"             , 3);
  glutAddMenuEntry("Zoom out (o)"            , 4);
  glutAddMenuEntry("------------------------", 0);
  glutAddMenuEntry("Synchronize Windows"     , 5);
  glutAddMenuEntry("Delete Auxiliary Window" , 6);
  glutAddMenuEntry("------------------------", 0);
  glutAddMenuEntry("Toggle SOLID / WIREFRAME (d)", 7);
  glutAddMenuEntry("Toggle SPIN ON / OFF (s)"    , 8);
  glutAddMenuEntry("Toggle SHIFT ON / OFF (f)"   , 9);
  glutAddMenuEntry("------------------------"    , 0);
  glutAddMenuEntry("Reset" , 10);
  glutAddMenuEntry("Exit"  , 11);
  glutAttachMenu( GLUT_RIGHT_BUTTON );

}

/* ------------------------------------------------------------------------- */
/* Main routine                                                              *
/* ------------------------------------------------------------------------- */
#ifdef STANDALONE
void main( int argc, char **argv ) {
#else
void initView( int argc, char **argv ) {
#endif

  viewInternalInit();
  /* Initialize the GLUT library and process command line options            */
  glutInit( &argc, argv );

  /* Initialize the display to be a double-buffered and RGBA-mode window     */
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );

  /* Create Main Window */
  viewCreateParent( "Multi Windows Interface" );

  /* Enter the GLUT processing loop                                          */
  glutMainLoop();

}
/*------------------------------------------------------------------------------------*/
#endif