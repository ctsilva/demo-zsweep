#ifndef _ZS_VIEWPOINT_H
#define _ZS_VIEWPOINT_H

/* -*-C++-*-
*******************************************************************************
*
* File:         zs_viewpoint.h
* Description:  
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Jan 28 2000
* Modified:     Fri Jan 28 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* (C) Copyright 2000, Ricardo Farias
*
*******************************************************************************/
#ifdef LINUX
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <sys/time.h>
#include <unistd.h>
#ifdef OPENGL
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif
#include "zs_basic.hh"
#include "zs_debug.hh"
#include "zs_Matrix.hh"
#include "zs_screenLists.hh"
using namespace std;

#define REND_OFF    0
#define REND_ON     1
#define REND_PAUSED 2

class ViewPlane {

public:

  unsigned _xres,   _yres;	/* resolution */
  float    _xOrig,  _yOrig;
  float    _xScale, _yScale;	/* Screen coords <-> world coords scaling... */ 
  float    _dx,     _dy;	/* ... and pixel size */
  float    _coord_scale;

  ScreenList *_scrList;          // This list is used by the render function

private:

  unsigned _timingOn;		/* Indicates if timing is running ou not */
  long     _delay;
  long     _maxDelay;		/* In nano-sec. 1sec==1,000,000nano-sec */
// struct timeval _tv1;
// struct timeval _tv2;


  struct rgb *_raster;		/* RGB picture */
  float       _maxIntValue;	/* Max value for the integration */

  Matrix      _rotMatrix;       /* Rotation Matrix as Identity*/

  /* 2D array emulation using 1D memory chunk */
  inline unsigned offset(unsigned x, unsigned y)
    {
      assert(x<_xres && y<_yres);
      return x+_xres*y;
    }

public:

  ViewPlane(unsigned x=320, unsigned y=200) 
    {
      _delay = 0;
      _maxDelay = 20000000;
      _xres=x; 
      _yres=y;
      _xOrig = _yOrig = 0.0;
      _xScale = _yScale = _dx = _dy = 1.0;
      _coord_scale = 1.0;
      _maxIntValue = 4.0;

      _raster  = new rgb[_xres*_yres];
      if( _raster == NULL ) 
	throw( "Could not allocate memory for _raster." );
      _scrList = new ScreenList( _xres, _yres ); 
      if( _scrList == NULL )
	throw("Could not allocate memory for _scrList." );
    }
  
  ~ViewPlane() 
    {
      delete [] _raster;
      delete _scrList ;
    }

  struct rgb *returnImageArray( void ) { return _raster; }

  void getDim( unsigned& x, unsigned& y ) {
    x = _xres;
    y = _yres;
  }
  inline unsigned getDimX( void ) { return _xres; }
  inline unsigned getDimY( void ) { return _yres; }

  inline void Clear() { memset(_raster, 0, 3 * _xres * _yres); }
  inline void Clear(unsigned *range) 
    { 
      
      unsigned x,y;
      for( y = range[2] ; y <= range[3] ; y++ )
	{
	  rgb *dest = &Raster(range[0],y);
	  for( x = range[0] ; x <= range[1] ; x++ ) 
	    {
	      dest->r = dest->g = dest->b = 0;
	      dest++;
	    }
	  
	}
    }
  
  /* Access to individual pixels */
  inline rgb &Raster(unsigned x,unsigned y)    {    return _raster[offset(x,y)];    }

  void copyPixels( ViewPlane *pixptr,  unsigned *range ) 
    {

      //printf("range %d %d %d %d\n", 
      //     range[0] , range[1] , range[2] , range[3]);
      
      unsigned x,y;
      for( y = range[2] ; y <= range[3] ; y++ )
	{
	  rgb *dest = &Raster(range[0],y);
	  rgb *orig = &(pixptr->Raster(range[0],y));
	  for( x = range[0] ; x <= range[1] ; x++ ) 
	    {

	      dest->r = orig->r;
	      dest->g = orig->g;
	      dest->b = orig->b;
	      dest++; orig++;
	      //_raster[offset(x,y)] = pixptr->_raster[offset(x,y)];

	    }

	}

    }
  unsigned numPixels( void ) 
    {

      unsigned x,y,count=0;
      for( y = 0 ; y < _yres ; y++ )
	{
	  rgb *orig = &Raster(0,y);
	  for( x = 0 ; x < _xres ; x++ ) 
	    {

	      if( orig->r != 0 || orig->g != 0 || orig->b != 0 )
		count++;
	      orig++;

	    }

	}
      return count;
    }

  /* Update the rotation matrix */
  void UpdateViewRotMatrix( float phix, float phiy, float phiz)
    {

      _rotMatrix.UpdateRotationMatrix( phix, phiy, phiz);

    }
  /* Update the rotation matrix */
  void UpdateViewRotMatrix( float *matrix )
    {

      _rotMatrix.UpdateRotationMatrix( matrix );

    }

  // Make ViewPlane scaled to fit box <p1,p2> into ViewPlane. 
  inline void RotatePoint( Point& p )
  {
    float x,y,z; 
    
    x = p[0]*_rotMatrix._m[0][0]+p[1]*_rotMatrix._m[0][1]+p[2]*_rotMatrix._m[0][2];
    y = p[0]*_rotMatrix._m[1][0]+p[1]*_rotMatrix._m[1][1]+p[2]*_rotMatrix._m[1][2];
    z = p[0]*_rotMatrix._m[2][0]+p[1]*_rotMatrix._m[2][1]+p[2]*_rotMatrix._m[2][2];
    p[0] = x; p[1] = y; p[2] = z;
    
  }
  
  // Create a texture mapping from raster image 
  void textGen( void ) 
    {
      glEnable( GL_TEXTURE_2D );
      gluBuild2DMipmaps( GL_TEXTURE_2D, 3, _xres, _yres,
			 GL_RGB, GL_UNSIGNED_BYTE, _raster );
    }
  
  // Create a texture mapping from raster image 
  void textMapping( void ) 
    {
      // cts 03/10/99: is the DEPTH buffer really necessary ?
      //
      //glEnable( GL_DEPTH_TEST );
      glEnable( GL_TEXTURE_2D );
      glBegin( GL_POLYGON );
      glTexCoord2f( 0.0, 0.0 );                 glVertex2f( s2wX(     0), s2wY(     0));
      glTexCoord2f( _coord_scale, 0.0 );         glVertex2f( s2wX(_xres-1), s2wY(     0));
      glTexCoord2f( _coord_scale, _coord_scale ); glVertex2f( s2wX(_xres-1), s2wY(_yres-1));
      glTexCoord2f( 0.0, _coord_scale );         glVertex2f( s2wX(     0), s2wY(_yres-1));
      glEnd();
    }
  
  /* Apply a function to all pixels */
  void ForAll(void (*f)(ViewPlane*, unsigned, unsigned)) 
    {
      unsigned x,y;
      for( x=0 ; x<_xres ; x++ )
	for( y=0 ; y<_yres ; y++ )
	  f(this,x,y);
    }
  
  // Write raster as PPM file (ascii)
  void SavePPM(const char *fname) 
  {
    FILE *f=fopen(fname,"w");
    if( f==NULL ) throw ("ViewPlane::SavePPM: Can not open file");
    fprintf(f, "P6 %u %u %u\n",_xres,_yres,255);
    fwrite(_raster,3,_xres*_yres,f);
    fclose(f);
  }
  
  // Make ViewPlane scaled to fit box <p1,p2> into ViewPlane. 
  void ScaleTo( float size ) {
    Point pLow (-size,-size,-size);
    Point pHigh(size,  size, size);
    ScaleTo(pLow, pHigh);
  }
  void ScaleTo(Point &p1, Point &p2) 
    {
      _xScale=(float)(_xres-1)/(p2[0]-p1[0]);
      _yScale=(float)(_yres-1)/(p2[1]-p1[1]);
      /* Want uniform scaling => choose the smallest scale for both axis */
      if (_yScale > _xScale)
	_yScale = _xScale;
      else 
	_xScale = _yScale;
      _dx = 1.0/_xScale;
      _dy = 1.0/_yScale;

      _xOrig=(p1[0]+p2[0])/2.0 - _dx*(_xres-1)/2.0;
      _yOrig=(p1[1]+p2[1])/2.0 - _dy*(_yres-1)/2.0;
      
      //printf("ViewPlane::ScaleTo:\n   Pixel Size: <%f,%f>\n",_dx,_dy );
      //printf("   Origin    : <%f,%f>\n", _xOrig,  _yOrig );
      //printf("   Scale     : <%f,%f>\n", _xScale, _yScale);
    }
  
  // Convert World <-> Screen coordinates 
  inline unsigned w2sX(float x)    {      return (unsigned) ((x-_xOrig)*_xScale);    }
  inline unsigned w2sY(float y)    {      return (unsigned) ((y-_yOrig)*_yScale);    }
  inline float s2wX(unsigned x)    {      return ((float) x)/_xScale + _xOrig;    }
  inline float s2wY(unsigned y)    {      return ((float) y)/_yScale + _yOrig;    }
  
  // Timed rendering control functions
  inline void startTiming(void) 
    { 
      _delay = 0;
      //gettimeofday(&_tv1, NULL);
      _timingOn = REND_ON;
    }
  
  inline int  isRenderingGoingOn(void) { return (_timingOn == REND_ON); }
  
  void pauseTiming(void) 
    { 
      if( _timingOn != REND_ON )
	throw( "Trying to [Pause] a Stopped or Paused Rendering!" );
      stopTiming();
      _timingOn = REND_PAUSED;
    }
  
  void resumeTiming(void) 
    {
      switch( _timingOn ) {
      case REND_PAUSED:
	//gettimeofday(&_tv1, NULL);
	_timingOn = REND_ON;
	break;
      case REND_OFF :
	throw( "Trying to [Resume] a Stopped Rendering!" );
	//break;
      case REND_ON :
	throw( "Trying to [Resume] a Running Rendering!" );
      default:
	throw( "Trying to [Resume] an Undefined Status Rendering!!!" );
      }
    }

  void stopTiming(void) 
    {
      if( _timingOn != REND_ON )
	throw( "Trying to [Stop] a Stopped or Paused Rendering!" );
//      gettimeofday(&_tv2, NULL);
//      _delay += 1000000*(_tv2.tv_sec-_tv1.tv_sec) + _tv2.tv_usec-_tv1.tv_usec ;
      _timingOn = REND_OFF;
    }
  
  long checkTimeout(void) 
    {
      //unsigned localTiming = _timingOn;
      switch( _timingOn ) {
      case REND_OFF :
	throw("Trying to [checkTimeout] for a Stopped Rendering!");
      case REND_PAUSED :
	throw("Trying to [checkTimeout] for a Paused Rendering!");
      }
      pauseTiming();
      if( _delay>=_maxDelay ) return true;
      resumeTiming();
      return false;
    }
  
  // max is expected in nano-seconds.  1sec = 1,000,000(nano-sec)
  void setMaxDelay( long max ) { _maxDelay = max; }
  long getDelay(void)          { return _delay; }
  void displayTimeDelay( char *mess=NULL ) {
    if( mess )
      printf( "%s %ld ms\n", mess, getDelay() );
    else
      printf( "Time delay %ld ms\n", getDelay() );
  }

  // Functions to handle the threshold for the integration maximum value
  int  checkMaxIntegral( float intValue )  { return intValue >= _maxIntValue; }
  void setMaxIntegral( float intValue )    { _maxIntValue = intValue; }

};

#endif
