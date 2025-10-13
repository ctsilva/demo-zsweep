/* -*-C++-*-
*******************************************************************************
*
* File:         zs_basic.cpp
* Description:  main module to use the Z-Sweep code
* Author:       Ricardo Farias
* Created:      Fri Jan 27 2000
* Modified:     Fri Feb 11 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* rfarias@ams.sunysb.edu
*
*******************************************************************************
*/
#include "zs_basic.hh"
//using namespace std;

//------------------------------------------------------------------------------
#define EPS02 1e-5 
int Face::coplanar( vector<Point> &tf_points_VEC ) {
  Point& p0 = tf_points_VEC[ _pi[0] ];
  Point& p1 = tf_points_VEC[ _pi[1] ];
  Point& p2 = tf_points_VEC[ _pi[2] ];
  return ( ( fabs(p0.getX()-p1.getX()) < EPS02 && fabs(p0.getX()-p2.getX()) < EPS02 ) ||
	   ( fabs(p0.getY()-p1.getY()) < EPS02 && fabs(p0.getY()-p2.getY()) < EPS02 )
	   )? true : false;
}

//------------------------------------------------------------------------------
void Face::markIfBoundary( vector<Point> &tf_points_VEC ) {
  Point& p0 = tf_points_VEC[ _pi[0] ];
  Point& p1 = tf_points_VEC[ _pi[1] ];
  Point& p2 = tf_points_VEC[ _pi[2] ];
  if( p0.marked( MARK_BOUNDARY ) && 
      p1.marked( MARK_BOUNDARY ) && 
      p2.marked( MARK_BOUNDARY ) )
    mark( MARK_BOUNDARY );
}

//------------------------------------------------------------------------------
int Face::updateCoeffs( vector<Point> &tf_points_VEC ) {
  Point& p0 = tf_points_VEC[ _pi[0] ];
  Point& p1 = tf_points_VEC[ _pi[1] ];
  Point& p2 = tf_points_VEC[ _pi[2] ];
  
  // Calculate A,B,C,D 
  double x1 = p0[0], y1 = p0[1], z1 = p0[2];
  double x2 = p1[0], y2 = p1[1], z2 = p1[2];
  double x3 = p2[0], y3 = p2[1], z3 = p2[2];
  double C = - DET2(x1,x2,x3,y1,y2,y3);

  if( fabs(C) < _DELTA12 ) 
    return false;

  // these get to be re-used later
  _fA = + DET2(y1,y2,y3,z1,z2,z3)/C;
  _fB = - DET2(x1,x2,x3,z1,z2,z3)/C;
  _fD = - DET3(x1,x2,x3,y1,y2,y3,z1,z2,z3)/C;
  
  // Calculate R,Q,R,S 
  z1 = p0.value(); z2 = p1.value(); z3 = p2.value();
  _fP = + DET2(y1,y2,y3,z1,z2,z3)/C;
  _fQ = - DET2(x1,x2,x3,z1,z2,z3)/C;
  _fS = - DET3(x1,x2,x3,y1,y2,y3,z1,z2,z3)/C;
  
  // Calculate PointInTri parameters
  _x0=x1;     _y0=y1;
  _x01=x2-_x0; _y01=y2-_y0;
  _x02=x3-_x0; _y02=y3-_y0;
  _denom = _x01*_y02 - _x02*_y01; // Actually denom=1/CR!
  if (_denom < 0.0) // Swap 1<->2 
    {
      float tmp;
      tmp=_x01; _x01=_x02; _x02=tmp;
      tmp=_y01; _y01=_y02; _y02=tmp;
      _denom = -_denom;
    }
  return true;

}

//------------------------------------------------------------------------------
