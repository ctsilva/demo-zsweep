/*--------------------------------------------------------------------------------
 * Program : pbr_RTInterf.c 
 * Date    : Nov 1998
 * Ricardo Farias   -  <rfarias@ams.sunysb.edu>
---------------------------------------------------------------------------------*/
//#include "togl.h"
#include <stdlib.h>
#include <string.h>
//#include <tcl.h>
//#include <tk.h>

#include "zs_debug.hh"
#include "zs_basic.hh"
#include "zs_viewpoint.hh"
#include "zs_scene.hh"
#include "zs_light.hh"

#include <math.h>
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
//#include <GL/gl.h>
#include <GL/glut.h>
#endif

#define RESOLUTION 128

Scene     *sceneptr,*mainScene,*s[10];
ViewPlane *pixptr;
Lighting  *lightptr;

/* Default options */
char     *inputname    = NULL;
char     *basenameFile = "pix";
char     *tfname       = "expanded.tf";
unsigned  xres   = RESOLUTION, yres = RESOLUTION;
float     px0 = 0.0, py0 = 0.0 , pz0 = 0.0;
float     px1 = 0.0, py1 = 0.0 , pz1 = 0.0;
unsigned  nf=1, iframe  = 0;
int       needps        = 0;
char     *psfile        = "grid.ps";
int       needtiming    = 0;
char     *timingfile    = "timing.pgm";
float     brightness    = 1.0;
unsigned  nActivePixels = 0; 
char      dispEdges     = 0;
char      saveAnim = false, saveImage = false, showLegend = false;
char      disp_Frame    = false;
//# int       flGhost       = false;
//# float     ghostLen      = 0;

//# vector<Point>    tf_points_VEC;
//# vector<Triangle> tf_triangles_VEC;
//# static int MMCell = 1;
//# static int OMMCell = 1;
//# extern int xMCell;
//# extern int yMCell;
//# extern bool useCoherency;
//# void integrate(  vector<Point> &tf_points_VEC, vector<Triangle> &tf_triangles_VEC,
//# 		 ViewPlane *v, Lighting *light, Scene *sceneptr );


/*--------------------------------------------------------------------------------
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 --------------------------------------------------------------------------------*/
#ifndef SGI
//extern "C" int matherr();
#endif
//int *tclDummyMathPtr = (int *) matherr;

// Definitions for the first OpenGL window
#define gl_PAULBUNIK 0
#define gl_LAYERS    1
#define gl_INTEGRAL  2
// Definitions for the second OpenGL window
#define gl_VISIBLE   0
#define gl_EXTERNAL  1
#define gl_ALL       2
// Constant for Axis Rotation
#define gl_RBoth   0
#define gl_RXOnly  1
#define gl_RYOnly  2

static GLuint FontBase;
static GLfloat xAngle  = 0.0, yAngle  = 0.0;
static GLfloat xDispAng= 0.0, yDispAng = 0.0;
static GLfloat xTAngle = 0.0, yTAngle = 0.0;
static GLfloat prevxTAngle = 0.0, prevyTAngle = 0.0;
static GLfloat CornerX, CornerY, CornerZ;  /* where to print strings */
static GLfloat xmin,xmax,ymin,ymax,zmin,zmax; // Bounding box
static GLfloat centerX, centerY, centerZ, rad, Rad;
static GLfloat scale = 1.0;
static GLint   dispmode = gl_ALL, disprendmode = gl_INTEGRAL, rotVar = gl_RBoth;
char   tmp_buf[20], gFlRendering = false, *rcFile = "./zs_interf.tcl";
char   *argvLocal[3];
char   flWaitDisplay = true, idleCalledDisp = false, mouseDown = false;
char   flStepByStepLayer = true;
int    stillIdle = 0; // Counts number of times idle is called before enabling it
struct Togl *toglSingle=NULL; // Will keep a local pointer to the rendering window
struct Togl *toglDouble=NULL; // Will keep a local pointer to the wireframe window

/*****************************************************************************/
void savePPMFile( char *base, char flAnim ) {
  // Save PPM file
  char fname[256];
  sprintf(fname,"%s_%u.ppm",base,iframe);
  if( showLegend ) lightptr->ShowLegend( pixptr );
  pixptr->SavePPM(fname);
  if( flAnim ) iframe++;
}
/*****************************************************************************/
void showImage( void ) 
{
  pixptr->textMapping();
  if( saveAnim ) savePPMFile(basenameFile,saveAnim);
}
/*****************************************************************************/
void frameRender( void ) 
{
  //  void RayTrace(ViewPlane *v, Lighting *light, char *timingfile);

  time_t Tstart = time(NULL);
  //  RayTrace( pixptr, lightptr, NULL);
  time_t raytime = time(NULL) - Tstart; 
#ifndef GEOMVIEW
  printf("main: Raytracing time: %lu sec\n", raytime);
#endif
}

/*****************************************************************************/
void frameRot( float xAng, float yAng, ViewPlane *pixptr,
		       Lighting *lightptr, Scene *sceneptr ) 
{

  static float localXAng = 0;
#ifndef GEOMVIEW
  fprintf( stderr, "\n....................................................\n" );
#endif
  localXAng += yAng;
#ifndef GEOMVIEW
  fprintf( stderr, "Angle = %f\n", localXAng );
#endif

  //# pixptr->startTiming();
  //# sceneptr->replicatePoints( tf_points_VEC );
  //# sceneptr->replicateTriangles( tf_triangles_VEC );
  //# pixptr->stopTiming();
  //# pixptr->displayTimeDelay( "Duplicating the points and triangles took " );
  pixptr->Clear(); // Clear image buffer
  pixptr->UpdateViewRotMatrix( xAng, yAng, 0. );
  //# sceneptr->RotBoundaryXYZ( tf_points_VEC, tf_triangles_VEC, pixptr );
  //# sceneptr->MakeBBox( tf_points_VEC, tf_triangles_VEC );
  //# sceneptr->MarkVisibleBoundary( tf_points_VEC, tf_triangles_VEC, pixptr );
  //# sceneptr->ProjectBoundaryToVP( tf_points_VEC, tf_triangles_VEC, pixptr );

}
void frameIntegration( ViewPlane *pixptr,
		       Lighting *lightptr, Scene *sceneptr ) 
{

#ifndef GEOMVIEW
  pixptr->startTiming();
#endif

  sceneptr->render( pixptr, lightptr );
  //# integrate( tf_points_VEC, tf_triangles_VEC, pixptr, lightptr, sceneptr );

#ifndef GEOMVIEW
  pixptr->stopTiming();
  pixptr->displayTimeDelay( "ZSweep took: " );
#endif
  //# sceneptr->tfStat(tf_points_VEC, tf_triangles_VEC);
  pixptr->SavePPM("xxx1.ppm");
#ifndef GEOMVIEW
  printf("Pixels Processed = %u\n", pixptr->numPixels() );
#endif
  disp_Frame = true;
}
/*****************************************************************************/
void globalDisplayScene(void) {
  //# sceneptr->displayScene(tf_points_VEC,tf_triangles_VEC,MARK_ANY);
  sceneptr->oglDisplayScene(MARK_ANY); 
}
/*****************************************************************************/
void globalDisplayAllVisExternal(void) {
  sceneptr->oglDisplayScene( MARK_ANY );
  //# if( dispEdges ) sceneptr->displayVisEdges(tf_points_VEC,tf_triangles_VEC);
}
/*****************************************************************************/
void globalDisplayAllExternal(void) {
  //# sceneptr->displayScene(tf_points_VEC,tf_triangles_VEC, MARK_NOTVISIBLE_BOUNDARY);
  //# if( dispEdges ) sceneptr->displayVisEdges( tf_points_VEC,tf_triangles_VEC );
}
/*****************************************************************************/
void framePreProp( char layerFlag=true ) 
{
//    sceneptr->MakeBBox();
//    sceneptr->MarkVisibleBoundary();
//    sceneptr->ProjectBoundaryToVP(pixptr);
}
/*****************************************************************************/
void frameRotate( float angx=0.0, float angy=0.0 ) 
{
  // Angles are now accepted in degree
  //sceneptr->RotateXYZ( angx*M_PI/180.0, angy*M_PI/180.0, 0.0 );
}

/*--------------------------------------------------------------------------------
 * Togl widget create callback.  This is called by Tcl/Tk when the widget has
 * been realized.  Here's where one may do some one-time context setup or
 * initializations.
 --------------------------------------------------------------------------------*/
void setViewBB( GLfloat xi,GLfloat xf,GLfloat yi,GLfloat yf,GLfloat zi,GLfloat zf) {
  GLfloat w, h, t; // width, height, thickness of a box
  /* Find the center of the bounding box                                     */
  centerX = ( xi + xf ) / 2.0;
  centerY = ( yi + yf ) / 2.0;
  centerZ = ( zi + zf ) / 2.0;
  /* Create larger bounding box to allow object within view after transform  */
  w = xmax - xmin;
  h = ymax - ymin;
  t = zmax - zmin;
  Rad = rad = sqrt(w * w + h * h + t * t) / 2;
  xmin = centerX - rad;
  xmax = centerX + rad;
  ymin = centerY - rad;
  ymax = centerY + rad;
  zmin = centerZ - 3 * rad;
  zmax = centerZ + 3 * rad;

}

/*--------------------------------------------------------------------------------
 * Togl widget create callback.  This is called by Tcl/Tk when the widget has
 * been realized.  Here's where one may do some one-time context setup or
 * initializations.
 --------------------------------------------------------------------------------*
void create_cb( struct Togl *togl ) {
   FontBase = Togl_LoadBitmapFont( togl, TOGL_BITMAP_8_BY_13 );
   if (!FontBase) {
      printf("Couldn't load font!\n");
      exit(1);
   }
}
/*

/*--------------------------------------------------------------------------------
 * Togl widget reshape callback.  This is called by Tcl/Tk when the widget
 * has been resized.  Typically, we call glViewport and perhaps setup the
 * projection matrix.
 --------------------------------------------------------------------------------*
void reshape_cb( struct Togl *togl ) {
   int width = Togl_Width( togl );
   int height = Togl_Height( togl );
   GLfloat aspect = (GLfloat) width / (GLfloat) height;

   glViewport( 0, 0, width, height );

   /* Set up projection transform *
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //glOrtho( xmin, xmax, ymin, ymax, -zmin, -zmax);
   glOrtho(-aspect-.6, aspect+.6, -2.1, 2.1, -2.0, 5.0);
   //glFrustum(-aspect, aspect, -1.5, 1.5, -2.0, 5.0);
   //gluPerpective( 45, aspect, -2.0, 5.0);

   CornerX = xmin;
   CornerY = -1.0;
   CornerZ = -1.1;

   /* Change back to model view transform for rendering *
   glMatrixMode(GL_MODELVIEW);
}
*/


/*------------------------------------------------------------------------------
static void print_string( const char *s ) {
   glCallLists( strlen(s), GL_UNSIGNED_BYTE, s );
}
*/

/*--------------------------------------------------------------------------------
 * Togl widget display callback.  This is called by Tcl/Tk when the widget's
 * contents have to be redrawn.  Typically, we clear the color and depth
 * buffers, render our objects, then swap the front/back color buffers.
 --------------------------------------------------------------------------------*/
void displayDouble_cb( struct Togl *togl ) {

  if( !disp_Frame  ) return;
  disp_Frame = false;

  //#if 0

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();	/* Reset modelview matrix to the identity matrix */
  glRotatef( 180, 0.0, 1.0, 0.0);
  glTranslatef(0.0, 0.0, 2.0);      /* Move the camera back three units */
  glScalef( scale, scale, scale );

  if( toglDouble == NULL ) toglDouble = togl;
  /*
  if( (int)xDispAng!=0 || (int)yDispAng!=0 ) {

    if( xDispAng != 0 && ( rotVar==gl_RBoth || rotVar==gl_RYOnly ) )
      frameRotate( xDispAng, 0        );
    if( yDispAng != 0 && ( rotVar==gl_RBoth || rotVar==gl_RXOnly ) ) 
      frameRotate( 0       , yDispAng );
    xDispAng = yDispAng = 0.0;
    //pixptr->Clear();
    //framePreProp( true );
    gFlRendering = true;
    gFlRendering = false;
  }
  */

  switch( dispmode ) {
  case gl_VISIBLE : globalDisplayAllVisExternal();
                    break;
  case gl_EXTERNAL: globalDisplayAllExternal(); 
                    break;
  default         : globalDisplayScene();
  }

  //#endif 

}


/*--------------------------------------------------------------------------------
 * Togl widget display callback.  This is called by Tcl/Tk when the widget's
 * contents have to be redrawn.  Typically, we clear the color and depth
 * buffers, render our objects, then swap the front/back color buffers.
 --------------------------------------------------------------------------------*/
void displaySingle_cb( struct Togl *togl ) {

  if( xDispAng == 0 && yDispAng == 0 && !gFlRendering ) return;
  switch( rotVar ) {
  case gl_RBoth :
    frameRot( (float)xDispAng, (float)yDispAng, pixptr, lightptr, sceneptr );
    break;
  case gl_RYOnly :
    frameRot( (float)0, (float)yDispAng, pixptr, lightptr, sceneptr );
    break;
  case gl_RXOnly :
    frameRot( (float)xDispAng, (float)0, pixptr, lightptr, sceneptr );
    break;
  }
  xDispAng = yDispAng = 0.0;
  disp_Frame=true;
  
  // OpenGL preparation
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();	/* Reset modelview matrix to the identity matrix */
  glRotatef( 180, 0.0, 1.0, 0.0);
  glTranslatef(0.0, 0.0, 2.0);      /* Move the camera back three units */
  glScalef( scale, scale, scale );

  if( toglSingle == NULL ) 
    toglSingle = togl;

  frameIntegration( pixptr, lightptr, sceneptr );
  pixptr->textGen();
  showImage();
  flWaitDisplay = false;
  gFlRendering = false;

}

/*--------------------------------------------------------------------------------
 * Togl widget display callback.  This is called by Tcl/Tk when the widget's
 * contents have to be redrawn.  Typically, we clear the color and depth
 * buffers, render our objects, then swap the front/back color buffers.
 --------------------------------------------------------------------------------*/
void display_cb( struct Togl *togl ) {

  char *ident;
//  ident = Togl_Ident( togl );

  if( *ident == 'S' ) 
    displaySingle_cb( togl );
  else 
    if( *ident == 'D' ) 
      displayDouble_cb( togl );

//  Togl_SwapBuffers( togl );

}


/*--------------------------------------------------------------------------------
 * Idle function
 --------------------------------------------------------------------------------*/
void idle_cb( struct Togl *togl ) {
/*
  //char *ident = Togl_Ident( togl );
  char *ident = ;
  return;
  if( *ident == 'D' ) {
//    Togl_PostRedisplay(togl);
    return;
  }
  if( mouseDown ) return;

  switch( disprendmode ) {
#if 0
  case gl_LAYERS :
    if( pixptr->isRenderingGoingOn() ) {
      if( flStepByStepLayer ) {
        if( pixptr->isRenderingGoingOn() ) {
	  //          processLayer( pixptr, lightptr );
          pixptr->textGen();
        }
      } else {
        while( pixptr->isRenderingGoingOn() )
	  //           processLayer( pixptr, lightptr );
        pixptr->textGen();
      }
      Togl_PostRedisplay(togl);
      if( toglDouble ) Togl_PostRedisplay(toglDouble);
    }
    break;
#endif
  case gl_INTEGRAL :

    gFlRendering = true;
//    Togl_PostRedisplay(togl);
    flWaitDisplay = true;
    break;
  
}
*/
}


/*------------------------------------------------------------------------------*/
/* Called when a new display mode is chossen for the secondery window           */
/*------------------------------------------------------------------------------*/
int displayMode_cb( struct Togl *togl, int argc, char *argv[] ) {
/*
   Tcl_Interp *interp = Togl_Interp(togl);
   /* error checking *
   if (argc != 3) {
      Tcl_SetResult( interp,
                     "wrong # args: should be \"pathName dispmode ?mode?\"",
                     TCL_STATIC );
      return TCL_ERROR;
   }

   if( strcmp(argv[2],"gl_VISIBLE" ) == 0 ) {
      dispmode = gl_VISIBLE;
   } else if( strcmp(argv[2],"gl_EXTERNAL" ) == 0 ) {
      dispmode = gl_EXTERNAL;
   } else  if( strcmp(argv[2],"gl_ALL") == 0 ) {
      dispmode = gl_ALL;
   } else {
      Tcl_SetResult( interp, "unknown dispmode value", TCL_STATIC );
      return TCL_ERROR;
   }
   disp_Frame = true;
   Togl_PostRedisplay(togl);

   /* Let result string equal value *
   strcpy( interp->result, argv[2] );
   return TCL_OK;
*/
  }

/*------------------------------------------------------------------------------*/
/* Called when a new render mode is chosen for the main window                  */
/*------------------------------------------------------------------------------*/
int displayRendMode_cb( struct Togl *togl, int argc, char *argv[] ) {
/*
  Tcl_Interp *interp = Togl_Interp(togl);
  /* error checking *
  if (argc != 3) {
     Tcl_SetResult( interp,
                    "wrong # args: should be \"pathName rendmode ?mode?\"",
                     TCL_STATIC );
     return TCL_ERROR;
  }

  gFlRendering = true;

  Togl_PostRedisplay(togl);
  /* Let result string equal value *
  strcpy( interp->result, argv[2] );
  return TCL_OK;
*/
  }

/*------------------------------------------------------------------------------*/
int setXrot_cb( struct Togl *togl, int argc, char *argv[] ) {
/*
	Tcl_Interp *interp = Togl_Interp(togl);
  char *ident;

  ident = Togl_Ident( togl );

  if( strcmp( ident, "Single" )==0 ) {

    /* error checking *
    if (argc != 3) {
       Tcl_SetResult( interp,
                      "wrong # args: should be \"pathName setXrot ?angle?\"",
                      TCL_STATIC );
       return TCL_ERROR;
    }

    xAngle  = (GLfloat)atof( argv[2] );
    prevxTAngle = xTAngle;
    xTAngle = xTAngle + xAngle;

    if ( xTAngle < 0.0 ) {
      xTAngle += 360.0;
    } else if ( xTAngle > 360.0 ) {
      xTAngle -= 360.0;
    }

    /* Let result string equal value *
    strcpy( interp->result, argv[2] );
    xDispAng = xAngle;
    xAngle = 0.0;

  }
  Togl_PostRedisplay( togl );
  return TCL_OK;
*/
}

/*------------------------------------------------------------------------------*/
int setTXrot_cb( struct Togl *togl, int argc, char *argv[] ) {
/*
  Tcl_Interp *interp = Togl_Interp(togl);
  char *ident;

  ident = Togl_Ident( togl );

  if( strcmp( ident, "Single" )==0 ) {

    /* error checking *
    if (argc != 3) {
       Tcl_SetResult( interp,
                      "wrong # args: should be \"pathName setXrot ?angle?\"",
                      TCL_STATIC );
       return TCL_ERROR;
    }

    xTAngle = (GLfloat)atof( argv[2] );
    xAngle  = xTAngle - prevxTAngle;
    if ( xTAngle < 0.0 ) {
      xTAngle += 360.0;
    } else if ( xTAngle > 360.0 ) {
      xTAngle -= 360.0;
    }

    /* Let result string equal value *
    sprintf( tmp_buf , "%f", xAngle );
    strcpy( interp->result, tmp_buf );
    xDispAng = xAngle;
    xAngle = 0.0;
    prevxTAngle = xTAngle;

  }

  Togl_PostRedisplay( togl );

  return TCL_OK;
*/
}


/*------------------------------------------------------------------------------*/
int setYrot_cb( struct Togl *togl, int argc, char *argv[] ) {
/*
	Tcl_Interp *interp = Togl_Interp(togl);
  char *ident;

  ident = Togl_Ident( togl );

  if( strcmp( ident, "Single" )==0 ) {

    /* error checking *
    if (argc != 3) {
       Tcl_SetResult( interp,
                      "wrong # args: should be \"pathName setYrot ?angle?\"",
                      TCL_STATIC );
       return TCL_ERROR;
    }

    yAngle  = (GLfloat)atof( argv[2] );
    prevyTAngle = yTAngle;
    yTAngle = yTAngle + yAngle;

    if ( yTAngle < 0.0 ) {
      yTAngle += 360.0;
    } else if ( yTAngle > 360.0 ) {
      yTAngle -= 360.0;
    }

    /* Let result string equal value *
    strcpy( interp->result, argv[2] );
    yDispAng = yAngle;
    yAngle = 0.0;

  }
  Togl_PostRedisplay( togl );
  return TCL_OK;
*/
  }

/*------------------------------------------------------------------------------*/
int setTYrot_cb( struct Togl *togl, int argc, char *argv[] ) {
/*
  Tcl_Interp *interp = Togl_Interp(togl);
  char *ident;

  ident = Togl_Ident( togl );

  if( strcmp( ident, "Single" )==0 ) {

    /* error checking *
    if (argc != 3) {
       Tcl_SetResult( interp,
                      "wrong # args: should be \"pathName setYrot ?angle?\"",
                      TCL_STATIC );
       return TCL_ERROR;
    }

    yTAngle = (GLfloat)atof( argv[2] );
    yAngle  = yTAngle - prevyTAngle;
    if ( yTAngle < 0.0 ) {
      yTAngle += 360.0;
    } else if ( yTAngle > 360.0 ) {
      yTAngle -= 360.0;
    }

    /* Let result string equal value *
    sprintf( tmp_buf , "%f", yAngle );
    strcpy( interp->result, tmp_buf );
    yDispAng = yAngle;
    yAngle = 0.0;
    prevyTAngle = yTAngle;

  }

  Togl_PostRedisplay(togl);

  return TCL_OK;
*/
}

/*------------------------------------------------------------------------------*
int getTXrot_cb( ClientData clientData, Tcl_Interp *interp,
		int argc, char *argv[]) {
  sprintf( interp->result, "%d", (int)xTAngle );
  return TCL_OK;
}

/*------------------------------------------------------------------------------*
int getTYrot_cb( ClientData clientData, Tcl_Interp *interp,
		int argc, char *argv[]) {
  sprintf( interp->result, "%d", (int)yTAngle );
  return TCL_OK;
}

/*------------------------------------------------------------------------------*
int setMCell_cb( struct Togl *togl, int argc, char *argv[] ) {

  Tcl_Interp *interp = Togl_Interp(togl);
  /* error checking *
  if (argc != 3) {
     Tcl_SetResult( interp,
                    "wrong # args: should be \"pathName setYrot ?angle?\"",
                    TCL_STATIC );
    return TCL_ERROR;
  }
  gFlRendering = true;
  Togl_PostRedisplay(togl);
  strcpy( interp->result, argv[2] );
  return TCL_OK;

}

/*------------------------------------------------------------------------------*
int setRotAxis_cb( struct Togl *togl, int argc, char *argv[] ) {

  Tcl_Interp *interp = Togl_Interp(togl);
  /* error checking *
  if (argc != 3) {
     Tcl_SetResult( interp,
                    "wrong # args: should be \"pathName setYrot ?angle?\"",
                    TCL_STATIC );
    return TCL_ERROR;
  }
  switch( *argv[2] ) {
  case 'b' : rotVar = gl_RBoth;
             break;
  case 'x' : rotVar = gl_RXOnly;
             break;
  case 'y' : rotVar = gl_RYOnly;
             break;
  }
  return TCL_OK;

}

/*------------------------------------------------------------------------------*
int setCoherency_cb( struct Togl *togl, int argc, char *argv[] ) {

  Tcl_Interp *interp = Togl_Interp(togl);
  /* error checking *
  if (argc != 3) {
     Tcl_SetResult( interp,
                    "wrong # args: should be \"pathName setYrot ?angle?\"",
                    TCL_STATIC );
    return TCL_ERROR;
  }
  gFlRendering = true;
  Togl_PostRedisplay(togl);
  strcpy( interp->result, argv[2] );
  return TCL_OK;

}
/*------------------------------------------------------------------------------*
int setSaveAnim_cb( struct Togl *togl, int argc, char *argv[] ) {

  Tcl_Interp *interp = Togl_Interp(togl);
  /* error checking *
  if (argc != 3) {
     Tcl_SetResult( interp,
                    "wrong # args: should be \"pathName setYrot ?angle?\"",
                    TCL_STATIC );
    return TCL_ERROR;
  }
  switch( *argv[2] ) {
  case '1' : saveAnim = true;
             break;
  case '0' : saveAnim = false;
             break;
  }
  return TCL_OK;

}
/*------------------------------------------------------------------------------*
int saveCurr_cb( struct Togl *togl, int argc, char *argv[] ) {

  // Save current image with the name current.ppm
  // The parameter 'false' avoids to counter for animation to
  // be incremented  ;-)
  savePPMFile("current",false);
  return TCL_OK;

}
/*------------------------------------------------------------------------------*
int scale_cb( struct Togl *togl, int argc, char *argv[] ) {

   Tcl_Interp *interp = Togl_Interp(togl);
   /* error checking *
   if (argc != 3) {
      Tcl_SetResult( interp,
                     "wrong # args: should be \"pathName scale ?value?\"",
                     TCL_STATIC );
      return TCL_ERROR;
   }

   scale = atof( argv[2] );

   Togl_PostRedisplay(togl);

   /* Let result string equal value *
   strcpy( interp->result, argv[2] );
   return TCL_OK;
}

/*------------------------------------------------------------------------------*
int setMouseDn_cb( struct Togl *togl, int argc, char *argv[] ) {

  Tcl_Interp *interp = Togl_Interp(togl);
  /* error checking *
  if (argc != 3) {
     Tcl_SetResult( interp,
                    "wrong # args: should be \"pathName scale ?value?\"",
                    TCL_STATIC );
     return TCL_ERROR;
  }
  if( strcmp( argv[2], "true") == 0 ) {
    mouseDown = true;
    framePreProp( true );
    gFlRendering = true;
    Togl_PostRedisplay(togl);
    /* Let result string equal value *
  } else {
    mouseDown = false;
    gFlRendering = false;
  }
  strcpy( interp->result, argv[2] );
  return TCL_OK;

}

/*------------------------------------------------------------------------------*
int setStepLayer_cb( struct Togl *togl, int argc, char *argv[] ) {

   Tcl_Interp *interp = Togl_Interp(togl);
  /* error checking *
  if (argc != 3) {
     Tcl_SetResult( interp,
                    "wrong # args: should be \"pathName scale ?value?\"",
                    TCL_STATIC );
     return TCL_ERROR;
  }
  if( strcmp( argv[2], "true") == 0 )
    flStepByStepLayer = true;
  else
    flStepByStepLayer = false;

  strcpy( interp->result, argv[2] );
  return TCL_OK;

}

/*------------------------------------------------------------------------------*
int setMesh_cb( struct Togl *togl, int argc, char *argv[] ) {

   Tcl_Interp *interp = Togl_Interp(togl);
  /* error checking *
  if (argc != 3) {
     Tcl_SetResult( interp,
                    "wrong # args: should be \"pathName scale ?value?\"",
                    TCL_STATIC );
     return TCL_ERROR;
  }
  if( strcmp( argv[2], "1") == 0 ) {
    sceneptr = s[0];
  } else {
    if( s[1] )
      sceneptr = s[1];
  }
  Togl_PostRedisplay(togl);

  strcpy( interp->result, argv[2] );
  return TCL_OK;

}

#if defined(WIN32)
EXTERN int		TkConsoleInit(Tcl_Interp *interp);
#endif /* WIN32 */

/*--------------------------------------------------------------------------------
 * Called by Tk_Main() to let me initialize the modules (Togl) I will need.
 --------------------------------------------------------------------------------*
int my_init( Tcl_Interp *interp ) {

   /*
    * Initialize Tcl, Tk, and the Togl widget module.
    *
   if (Tcl_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
   }
   if (Tk_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
   }

#ifdef WIN32
    /*
     * Set up a console window. Delete the following statement if you do not need that.
     *
    if (TkConsoleInit(interp) == TCL_ERROR) {
	   return TCL_ERROR;
    }
#endif /* WIN32 *

   if (Togl_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
   }

   /*
    * Specify the C callback functions for widget creation, display,
    * and reshape.
    *
   Togl_CreateFunc( create_cb );
   Togl_DisplayFunc( display_cb );
   Togl_ReshapeFunc( reshape_cb );
   Togl_TimerFunc( idle_cb );

   /*
    * Make a new Togl widget command so the Tcl code can set a C variable.
    *
   Togl_CreateCommand( "scale" , scale_cb  );
   Togl_CreateCommand( "setRotAxis", setRotAxis_cb );
   Togl_CreateCommand( "setXrot", setXrot_cb );
   Togl_CreateCommand( "setTYrot", setTYrot_cb );
   Togl_CreateCommand( "setTXrot", setTXrot_cb );
   Togl_CreateCommand( "setYrot", setYrot_cb );
   Tcl_CreateCommand( interp, "getTXrot", getTXrot_cb, (ClientData)NULL,
                               (Tcl_CmdDeleteProc *)NULL );
   Tcl_CreateCommand( interp, "getTYrot", getTYrot_cb, (ClientData)NULL,
                               (Tcl_CmdDeleteProc *)NULL );

   /*
    * Call Tcl_CreateCommand for application-specific commands, if
    * they weren't already created by the init procedures called above.
    *
   Togl_CreateCommand( "displayMode", displayMode_cb );
   Togl_CreateCommand( "displayRendMode", displayRendMode_cb );
   Togl_CreateCommand( "setMCell", setMCell_cb );
   Togl_CreateCommand( "setCoherency", setCoherency_cb );
   Togl_CreateCommand( "setSaveAnim", setSaveAnim_cb );
   Togl_CreateCommand( "saveCurr", saveCurr_cb );
   Togl_CreateCommand( "setMouseDn", setMouseDn_cb );
   Togl_CreateCommand( "setStepLayer", setStepLayer_cb );
   Togl_CreateCommand( "setMesh", setMesh_cb );

   /*
    * Specify a user-specific startup file to invoke if the application
    * is to run interactively.  Typically the startup file is "~/.apprc"
    * where "app" is the name of the application.  If this line is deleted
    * then no user-specific startup file will be run under any conditions.
    *
#if (TCL_MAJOR_VERSION * 100 + TCL_MINOR_VERSION) >= 705
   Tcl_SetVar( interp, "tcl_rcFileName", rcFile, TCL_GLOBAL_ONLY );
#else
   tcl_RcFileName = rcFile;
#endif

   return TCL_OK;
}

/* Error function to handle X Protocol
static int XErrorProc( ClientData data, XErrorEvent *errEventPtr ) {
  //Tk_Window w = (Tk_Window)data;
#ifndef GEOMVIEW
  fprintf( stderr, "X protocol error: ");
  fprintf( stderr, "Error=%d  request=%d  minor=%d\n",
           errEventPtr->error_code,
           errEventPtr->request_code,
           errEventPtr->minor_code );
#endif
  // Put special error handler here
  return 0;
}*

// Global TK window variable
Tk_Window mainWindow;
static char *display  = NULL;
//static char *geometry = NULL;
static int   debug    = 0;
Tk_ArgvInfo argTable[] = {
  { "-display", TK_ARGV_STRING  , (char *)NULL, (char *)&display, 
                "Display to use" },
  { "-debug"  , TK_ARGV_CONSTANT, (char *)1   , (char *)&debug  , 
                "Set thing for gdb-type debugging" },
  { ""        , TK_ARGV_END }
};
*/
//---------------------------------------------------------------------------
void usage( char *fileName ) {
#ifndef GEOMVIEW
  fprintf(stderr,"Usage: %s [options] <file.off>\n", fileName );
  fprintf(stderr,"  Where [options] can be:\n" );
  fprintf(stderr,"     -t <transfer-file.tf>\n" );
  fprintf(stderr,"     -b <basename to save the images>\n" );
  fprintf(stderr,"     -x xres -y yres\n" );
  fprintf(stderr,"     -s px0:py0:pz0\n" );
  fprintf(stderr,"     -f px1:py1:pz1\n" );
  fprintf(stderr,"     -n nframes\n" );
  fprintf(stderr,"     -T timing.pgm -P grid.ps file.scalar\n" );
  fprintf(stderr,"     -P grid.ps\n" );
#endif
  exit(1);
}

//------------------------------------------------------------------------------
void sceneInit(int argc, char **argv) 
{
  int opt=1;
  while (opt) {
    
    switch (getopt(argc, argv, "b:s:f:n:t:x:y:h:T:P:I:"))
      {
      case 'b':
	basenameFile = optarg;
	break;
      case 's':
	if (sscanf(optarg,"%f:%f:%f", &px0, &py0, &pz0)!=3)
	  usage(argv[1]);
	break;
      case 'f':
	if (sscanf(optarg,"%f:%f:%f", &px1, &py1, &pz1)!=3)
	  usage(argv[1]);
	break;
      case 'n':
	if (sscanf(optarg,"%u", &nf)!=1)
	  usage(argv[1]);
	break;
      case 't': 
	tfname = optarg;
	break;
      case 'x':
	if (sscanf(optarg,"%u", &xres)!=1)
	  usage(argv[1]);
	break;
      case 'y':
	if (sscanf(optarg,"%u", &yres)!=1)
	  usage(argv[1]);
	break;
      case 'T':
	needtiming = 1;
	if (optarg) timingfile=optarg;
	break;
      case 'P':
	needps = 1;
	if (optarg) psfile=optarg;
	break;
      case 'I':
	if (sscanf(optarg,"%f", &brightness)!=1)
	  usage(argv[1]);
	break;
      case EOF:
	inputname = argv[optind];
	opt=0;
	break;
      case 'h':
      case '?':
	usage( argv[1] );
      }
  }
  
  if (inputname==NULL) 
    throw ("Please supply an input filename");

#ifndef GEOMVIEW
  printf( "Input File = %s\n", inputname );
  printf( "Transfer   = %s\n", tfname );
  printf( "Resolution = (%d,%d)\n", xres, yres );
  printf( "BaseName   = %s\n", basenameFile );
#endif

  time_t Tstart = time(NULL);

  /* Read and scale scene */
  float scenesize;
  mainScene = new Scene( inputname );
  //mainScene->Center();
  scenesize = mainScene->getSceneSize();
  //mainScene->ShiftValue();

  sceneptr = s[0] = mainScene;
  s[1] = NULL;

  /* Create viewplane */
  ViewPlane *pix = new ViewPlane(xres, yres);
  pixptr = pix;
  //Point pLow(-scenesize,-scenesize,-scenesize);
  //Point pHigh(scenesize,scenesize,scenesize);
  //pix->ScaleTo(pLow, pHigh);
  pix->ScaleTo( scenesize );
  //   signal(SIGINT, SaveWhatWeHave);

  /* Create lighting tables */
  Lighting *light = new Lighting(tfname, brightness);
  lightptr = light;
  light->PrintTables();

  time_t readtime = time(NULL) - Tstart;
#ifndef GEOMVIEW
  printf("main: Scene reading time: %lu sec\n", readtime);
#endif

}


//------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {

  GLfloat xi,xf,yi,yf,zi,zf; // Bounding box
  /* Initializa the rendering Scene */
  //srandom(17399);
  //srandom( random() );
  sceneInit( argc, argv );
  
  // pixptr->setMaxDelay( 20000000 ); // 20 sec is the default
  sceneptr->getSceneBB( xi, xf, yi, yf, zi, zf );
  setViewBB( xi, xf, yi, yf, zi, zf );
  Tk_Main( argc, argv, my_init );
  return 0;

}

//------------------------------------------------------------------------------
