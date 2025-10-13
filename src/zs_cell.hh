#ifndef _ZS_CELL_H
#define _ZS_CELL_H
/* -*-C++-*-
*******************************************************************************
*
* File:         zs_cell.h
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
#include <sys/time.h>
#include <unistd.h>
#ifdef OPENGL
#include <GL/gl.h>
#include <GL/glu.h>
#endif
*/
#include "zs_general.hh"
#include "zs_basic.hh"

// I'll consider only
enum POLY_TYPE { TETRAHEDRON=0, HEXAHEDRON };

//--------------------------------------------------------------------------------
class Cell: public Object
{
private:

  POLY_TYPE _type;
  char      _touched;
  unsigned *_pIdx;

public:

  Cell( unsigned p0, unsigned p1, unsigned p2, unsigned p3 )
    {
      _type    = TETRAHEDRON;
      _touched = 0;
      _pIdx    = new unsigned[4];
      if( _pIdx == NULL ) 
	throw( "Could not allocate memory for _pIdx:tetra.");
      _pIdx[0] = p0;
      _pIdx[1] = p1; 
      _pIdx[2] = p2; 
      _pIdx[3] = p3;       
    }
  Cell( unsigned p0, unsigned p1, unsigned p2, unsigned p3, 
	unsigned p4, unsigned p5, unsigned p6, unsigned p7)
    {
      _type    = HEXAHEDRON;
      _touched = 0;
      _pIdx    = new unsigned[8];
      if( _pIdx == NULL ) 
	throw( "Could not allocate memory for _pIdx:hexa.");
      _pIdx[0] = p0;
      _pIdx[1] = p1;
      _pIdx[2] = p2;
      _pIdx[3] = p3;
      _pIdx[4] = p4;
      _pIdx[5] = p5;
      _pIdx[6] = p6;
      _pIdx[7] = p7;
    }

//--------------------------------------------------------------------------------
  int getCellType( void ) { return _type; }

  int getNumVertices( void ) {
    if( _type == TETRAHEDRON )
      return 4;
    else
      return 8;
  }

  int numFacesTouchingVertex( void ) {
    if( _type == TETRAHEDRON )
      return 3;
    else
      return 3;
  }
  
  inline unsigned point( unsigned i ) { 
    if( _type == TETRAHEDRON )
      assert(i < 4 );
    else
      assert(i < 8 );
    return _pIdx[i];
  }
  
  inline unsigned &operator[] (unsigned i)
  {
    if( _type == TETRAHEDRON )
      assert(i < 4 );
    else
      assert(i < 8 );
    return _pIdx[i];
  }
  
  void resetTouch( void ) { _touched = 0; }
  void incTouch( void ) { 
    _touched++; 
    if( _type == TETRAHEDRON )
      assert( _touched < 4 );
    else
      assert( _touched < 8 );
  }
  int  getTouch( void ) { return _touched; }

  int Swept( void ) { 
    if( _type == TETRAHEDRON )
      return ( _touched < 2 )? false:true; 
    else
      return ( _touched < 5 )? false:true; 
  }

  //--------------------------------------------------------------------------
  void neighborFaces( unsigned   vertexGlobalIdx, unsigned  *neigFaces );
  void getFaceGlobalVerticesIndices( unsigned  faceCellIdx,
				     unsigned *vertices );
  int onlyIWasSwept( unsigned vertexGlobalIdx,
		     unsigned faceCellIdx,
		     vector<Point> &tf_points_VEC );
  float getMaxZ( vector<Point> &tf_points_VEC);
  int   Belong( unsigned  currVertexGlobalIdx );

#ifdef SMP_SGI

  int onlyIWasSwept( unsigned vertexGlobalIdx,
		     unsigned faceCellIdx,
		     vector<bool> &is_point_transformed_VEC);
  
#endif

};

#endif
//------------------------------------------------------------------------------
