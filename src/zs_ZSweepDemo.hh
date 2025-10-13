#ifndef __VIEW__
#define __VIEW__
/**********************************************************************************/
/* FILE: view.h                                                                   */
/*                                                                                */
/* Based on the code stripview.c by: Xinyu Xiang                                  */
/* Email: xxiang@ams.sunysb.edu                                                   */
/*                                                                                */
/* Created: 11/24/1997                                                            */
/* Modified: 03/26/1998  by rfarias@ams.sunysb.edu                                */
/**********************************************************************************/
/* OpenGL include files */
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#ifdef LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "GL/glut.h"
#endif

extern void setDisplayFunc( void (*func)() );
extern void setCenter( GLfloat centX, GLfloat centY, GLfloat centZ );
extern void setViewBB( GLfloat xi,GLfloat xf,GLfloat yi,GLfloat yf,GLfloat zi,GLfloat zf);
extern void setViewDisplayFunction( void (*func)() );
extern void setViewIdleFunction( void (*func)() );
extern void setViewKeyboardFunction( void (*func)(unsigned char, int, int) );
extern void psSetColor( GLfloat r,GLfloat g, GLfloat b );
extern void psAlpha( GLfloat alpha );
extern GLfloat getAlpha( void );
extern void psDefine( GLfloat x1, GLfloat y1, GLfloat z1,
                      GLfloat x2, GLfloat y2, GLfloat z2 );
extern void psSetX( GLfloat inc );
extern void psSetY( GLfloat inc );
extern void psSetZ( GLfloat inc );
extern void psIncX( GLfloat inc );
extern void psIncY( GLfloat inc );
extern void psIncZ( GLfloat inc );
extern void turnspOn( void );
extern void turnspOff( void );
extern void enableNavDisplay( void );
extern void disableNavDisplay( void );
extern void initView( int argc, char **argv );
#endif
/**********************************************************************************/
