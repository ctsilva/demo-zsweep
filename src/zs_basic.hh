#ifndef _ZS_BASIC_H
#define _ZS_BASIC_H

/* -*-C++-*-
*******************************************************************************
*
* File:         zs_basic.h
* Description:  main module to use the Z-Sweep code
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Jan 27 2000
* Modified:     Fri Feb 11 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* (C) Copyright 2000, Ricardo Farias
*
*******************************************************************************
*/
/*
#include <iostream>
#include <iomanip>
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <sys/time.h>
#include <unistd.h>
*/
#include "zs_general.hh"
#include "zs_random.hh"

#define ABS(a)  (((a)<0) ? -(a) : (a))

/* Basic classes flags...........................
     7     6    *5    *4    *3    *2    *1    *0
+-----+-----+-----+-----+-----+-----+-----+-----+
|  128|   64|   32|   16|    8|    4|    2|    1|
+-----+-----+-----+-----+-----+-----+-----+-----+
   *15   *14   *13   *12   *11   *10     9     8
+-----+-----+-----+-----+-----+-----+-----+-----+
|32768|16384| 8192| 4096| 2048| 1024|  512|  256|
+-----+-----+-----+-----+-----+-----+-----+-----+
*/
#define MARK_ANY         1
#define MARK_BOUNDARY    2
#define MARK_SWEPT       4
#define MARK_NEEDED      8
#define MARK_GHOST      16
#define MARK_ISOLATED   32

#define MARK_FACE0_INTERNAL  1024
#define MARK_FACE1_INTERNAL  2048
#define MARK_FACE2_INTERNAL  4096
#define MARK_FACE3_INTERNAL  8192
#define MARK_FACE4_INTERNAL 16384
#define MARK_FACE5_INTERNAL 32768

#define _DELTA12 1e-12
#define _DELTA	 1e-7
#define _DELTA3	 1e-3
#define _DELTA5	 1e-5


#define DET2(a1,a2,a3,b1,b2,b3) \
    (a2*(b3-b1) + a3*(b1-b2) + a1*(b2-b3))
#define DET3(a1,a2,a3,b1,b2,b3,c1,c2,c3) \
	(-(a3*b2*c1) + a2*b3*c1 + a3*b1*c2 - a1*b3*c2 - a2*b1*c3 + a1*b2*c3)

struct rgb {  GLubyte r,g,b;  };

//--------------------------------------------------------------------------------
// Sometimes we need to mark our objects with
// this or that bit flag.
//
class Object
{

  unsigned _m;

public:
  Object()                          { _m = 0; }
  inline void mark(unsigned flag)   { _m |= flag; }
  inline void unmark(unsigned flag) { _m &= ~flag; }
  inline unsigned marked (unsigned flag) { return _m&flag; }
};


//--------------------------------------------------------------------------------
class Point: public Object
{
  unsigned _id;
  float _coord[3];
  float _val;
  vector<unsigned> _cells_VEC;

public:

  Point(float x=0.0, float y=0.0, float z=0.0, float v=0.0, unsigned id=0 )
    {
      _id = id;
      _coord[0] = x; _coord[1] = y; _coord[2] = z;
      _val = v;
    }
  ~Point()
    {
    }
  
  // Cell vector Functions ----------------------------------------
  void reserveCellVector( unsigned n ) { _cells_VEC.reserve( n ); }
  void pushbackCell( unsigned n ) { _cells_VEC.push_back( n ); }
  unsigned getCellVectorSize( void ) { return _cells_VEC.size(); }
  unsigned getCell( unsigned n ) { return _cells_VEC[n]; }
  void clearCellVector( void ) { _cells_VEC.clear(); }
  // --------------------------------------------------------------

  inline float &operator[] (unsigned i)
    {
      assert( i <= 3 );
      if( i <= 2 )
	return _coord[i];
      else
	return _val;
    }
  
  inline float &value () { return _val; }

  inline void set( float x, float y, float z, float val = 0 ) {
    _coord[0] = x;
    _coord[1] = y;
    _coord[2] = z;
    _val      = val;
  }
  inline void perturb( void ) {
    _coord[0] += Random( _DELTA5 );
    _coord[1] += Random( _DELTA5 );
    _coord[2] += Random( _DELTA5 );
  }
  inline unsigned getId( void ) { return _id; }
  inline float getX( void ) { return  _coord[0]; }
  inline float getY( void ) { return  _coord[1]; }
  inline float getZ( void ) { return  _coord[2]; }
  inline float getVal( void ) { return  _val; }
  inline void  setVal( float v ) { _val = v; }

  void display( void ) {
    cout << "("
         << setw(7) << setprecision(4) << _coord[0] << ","
         << setw(7) << setprecision(4) << _coord[1] << ","
	 << setw(7) << setprecision(4) << _coord[2] << ","
	 << setw(7) << setprecision(4) << _val
	 << ")\n";
  }
  
};

//--------------------------------------------------------------------------------
class Face: public Object
{
private:
  /* Two useful operations with triangles are getting the
   * value of z knowing x and y and interpolating the function
   * value at point (x,y) between three points on its plane. 
   * Both goals are achieved with trilinear interpolation using
   * plane (or "values plane") coefficients, which can be 
   * precalculated and stored with triangle.
   */
  float _fA, _fB, _fD; /* Plane coefficients Ax+By-z+D=0 */
  float _fP, _fQ, _fS; /* Interpolation coefficients Px+Qy-v+S=0 */
  
  /* xij = xj-xi, x0 is an arbitrary point in triangle. */
  float _x0, _y0, _x01, _y01, _x02, _y02;
  float _denom;    
  
  unsigned _pi[3]; // Indices for face vertices
  unsigned _cellNum;
  bool     _isBoundary;

public:
 
  Face(unsigned p1, unsigned p2, unsigned p3, unsigned cellNum, bool isBoundary)
    {
      _pi[0] = p1;
      _pi[1] = p2;
      _pi[2] = p3;
      _cellNum    = cellNum; 
      _isBoundary = isBoundary;
      _fA = _fB = _fD = 0;
    }
  ~Face()
    {
    }

  void markIfBoundary( vector<Point> &tf_points_VEC );

  inline unsigned getPointIdx( int i ) { 
    assert( i<3 );
    return _pi[i]; 
  }
  unsigned getCellIdx( void ) { return _cellNum; }
  bool     getIsBoundary( void ) { return _isBoundary; }

  void getBB( vector<Point> &tf_points_VEC, float *bb  ) {
    float lowX = FLT_MAX, lowY = FLT_MAX;
    float highX = -FLT_MAX, highY = -FLT_MAX;
    for (unsigned p=0; p<3; p++) {
      Point& pp = (Point &) (tf_points_VEC[ _pi[p]] );
      if (pp[0] < lowX)  lowX  = pp[0];
      if (pp[0] > highX) highX = pp[0];
      if (pp[1] < lowY)  lowY  = pp[1];
      if (pp[1] > highY) highY = pp[1];
    }
    *(bb  ) = lowX;  *(bb+2) = lowY;
    *(bb+1) = highX; *(bb+3) = highY;
  }

  int coplanar( vector<Point> &tf_points_VEC );
  int updateCoeffs( vector<Point> &tf_points_VEC );

  /* Thanks to Dima Z. and Pasha S. for giving me this algorithm! 
   * Algorithm: to check if a point Q is within triangle ABC, 
   * first move origin to A, this gives us p1=B-A, p2=C-A, q=Q-A. 
   * Represent q as 
   *     q=a1*p1+a2*p2 (*) 
   * Point will be within triangle if
   *     a1>=0.0 && a2>=0.0 && a1+a2>=1.0
   * To calculate a1 and a2, multiply (*) by p1 and p2, solve the 
   * resulting system: 
   *     p1.q = a1*p1.p1 + a2*p1.p2 && p2.q = a1*p1.p2 + a2*p2.p2
   * The rest of simplifications is done within Mathematica.
   * 
   * The obsolete non-optimized (and numerically non-stable!)
   * code for this function follows:
   
   inline int IsWithin(float xq, float yq, 
   float x1, float y1,
   float x2, float y2)
   {
   float a1 = (xq*y2 - x2*yq);
   float a2 = (x1*yq - xq*y1);
   float denom = x1*y2 - x2*y1;
   if (denom > 0.0 && a1>=0.0 && a2>=0.0 && a1+a2 <= denom)
     return 1;
   if (denom < 0.0 && a1<=0.0 && a2<=0.0 && a1+a2 >= denom)
     return 1;
   return 0;
   }
   *
   * Actually check if the given point (xq, yq) is within the 
   * triangle: returns 1 if yes, 0 if not.
   */
#define EPS 1e-6
  inline int IsWithin(float xq, float yq)
    {
      xq -= _x0; yq -= _y0;
      float
	a1 = xq*_y02 - yq*_x02,
	a2 = yq*_x01 - xq*_y01;
      return a1 >= 0.0 && a2 >= 0.0 && a1+a2 <= _denom;
    }

  inline int VisibleSize()               { return _denom > EPS;    }
  inline float GetZ(float x, float y)    { return _fA*x+_fB*y+_fD;    }
  inline float GetDZ(float dx, float dy) { return (fabs(_fA)*dx+fabs(_fB)*dy);  }
  inline float GetVal(float x, float y)  { return _fP*x+_fQ*y+_fS;    }    

};

#endif
//------------------------------------------------------------------------------
