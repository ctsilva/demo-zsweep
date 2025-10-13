/* -*-C++-*-
*******************************************************************************
*
* File:         zs_cell.cpp
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
* Structures of connectivity were written by Claudio Silva
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
#include <GL/gl.h>
#include <GL/glu.h>
#include <sys/time.h>
#include <unistd.h>
*/
#include "zs_cell.hh"

//--------------------------------------------------------------------------------
// Structure for Tetrahedra
//--------------------------------------------------------------------------------

// Tetrahedron ...............................


int TETRAedges[4][4] = {
  {3, 1, 2, 3},
  {3, 0, 2, 3},
  {3, 0, 1, 3},
  {3, 0, 1, 2} 
};

int TETRAfaces[4][4] = {
  {3, 0, 1, 3},
  {3, 0, 1, 2},
  {3, 0, 2, 3},
  {3, 1, 2, 3} 
};

// Given a vertex index (relative to the current cell)
// One can access the faces number that touches this vertex
int TETRAvertexfaces[4][4] = {
  {3, 0, 1, 2},
  {3, 0, 1, 3},
  {3, 1, 2, 3},
  {3, 0, 2, 3} 
};

//  Faces -- Share
//           Edge
//   0:1  --   0
//   0:2  --   1
//   0:3  --   2
//   1:2  --   3
//   1:3  --   4
//   2:3  --   5
// NOTE: There is no meaning in talk about the edge shared by faces 0 and 0
//       So these combinations have value -1 in the structure
// so if you need to access the faces shared by the edge between vertices 0-3
//   edgeNum = TETRAedgeN[0][3]     -> edgeNum == 2
//   face0 = TETRAfaceEdge[edge][0] -> face0   == 0
//   face1 = TETRAfaceEdge[edge][1] -> face1   == 2
int TETRAedgeN[4][4] = {
  {-1,  0,  1,  2},
  { 0, -1,  3,  4},
  { 1,  3, -1,  5},
  { 2,  4,  5, -1} 
};

// Edge -- Shares
//          Faces
//   0  --   0:1
//   1  --   0:2
//   2  --   0:3
//   3  --   1:2
//   4  --   1:3
//   5  --   2:3
int TETRAfaceEdge[6][2] = {
  {0, 1},    {1, 2},
  {0, 2},    {1, 3},
  {0, 3},    {2, 3}
};


// Hexahedron ...............................

int HEXAedges[8][4] = {
  {3, 1, 3, 4},
  {3, 0, 2, 5},
  {3, 1, 3, 6},
  {3, 0, 2, 7},
  {3, 0, 5, 7},
  {3, 1, 4, 6},
  {3, 2, 5, 7},
  {3, 3, 4, 6}
};

int HEXAfaces[6][5] = {
  {4, 0, 1, 2, 3},  /* 0 */
  {4, 4, 5, 6, 7},  /* 1 */
  {4, 0, 1, 5, 4},  /* 2 */
  {4, 3, 2, 6, 7},  /* 3 */
  {4, 0, 3, 7, 4},  /* 4 */
  {4, 1, 2, 6, 5}   /* 5 */
};

int HEXAvertexfaces[8][4] = {
  {3, 0, 2, 4},
  {3, 0, 2, 5},
  {3, 0, 3, 5},
  {3, 0, 3, 4},
  {3, 1, 2, 4},
  {3, 1, 2, 5},
  {3, 1, 3, 5},
  {3, 1, 3, 4} 
};

int HEXAedgeN[8][8] = {
  {-1,  0, -1, 10,  9, -1, -1, -1},
  { 0, -1,  1, -1, -1,  3, -1, -1},
  {-1,  1, -1, 11, -1, -1,  2, -1},
  {10, -1, 11, -1, -1, -1, -1,  8},
  { 9, -1, -1, -1, -1,  6, -1,  7},
  {-1,  3, -1, -1,  6, -1,  4, -1},
  {-1, -1,  2, -1, -1,  4, -1,  5},
  {-1, -1, -1,  8,  7, -1,  5, -1}
};

int HEXAfaceEdge[12][2] = {
  {0, 2},      {0, 5},
  {3, 5},      {2, 5},
  {1, 5},      {1, 3},
  {1, 2},      {1, 4},
  {3, 4},      {2, 4},
  {0, 4},      {0, 3}
};

//--------------------------------------------------------------------------------
// Cell class functions
//--------------------------------------------------------------------------------
void Cell::neighborFaces( unsigned  vertexGlobalIdx, unsigned *neigFaces ) {

  unsigned vertexCellIdx = 0;
  unsigned i, nvert = getNumVertices();
  switch( _type ) {

  case TETRAHEDRON:
    
    // Let's find the the position of vertex currVertexGlobalIdx in curr cell
    for( i = 0 ; i < nvert ; i ++ ) {
      if( vertexGlobalIdx == point( i ) ) {
	vertexCellIdx = i;
	break;
      }
    }
    neigFaces[0] = TETRAvertexfaces[ vertexCellIdx ][0];
    for( i = 1 ; i < neigFaces[0]+1 ; i++ ) {
      neigFaces[i] = TETRAvertexfaces[ vertexCellIdx ][i];
    }
    break;
    
  case HEXAHEDRON:
    
    // Let's find the the position of vertex currVertexGlobalIdx in curr cell
    for( i = 0 ; i < nvert ; i ++ ) {
      if( vertexGlobalIdx == point( i ) ) {
	vertexCellIdx = i;
	break;
      }
    }
    neigFaces[0] = HEXAvertexfaces[ vertexCellIdx ][0];
    for( i = 1 ; i < neigFaces[0]+1 ; i++ ) {
      neigFaces[i] = HEXAvertexfaces[ vertexCellIdx ][i];
    }
    break;
  default:
    throw( "Could not determine the type of the cell. Aborting..." );
  }

}

//--------------------------------------------------------------------------------
// Return the Cell Index of the vertex, or -1 if fail
void Cell::getFaceGlobalVerticesIndices( unsigned  faceCellIdx,
					 unsigned *vertices
					 ) {
  
  unsigned i;

  switch( _type ) {
    
  case TETRAHEDRON :
    
    vertices[0] = TETRAfaces[ faceCellIdx ][ 0 ];
    for( i = 1 ; i < vertices[0]+1 ; i++ ) {
      vertices[i] = point( TETRAfaces[ faceCellIdx ][ i ] );
    }
    break;
    
  case HEXAHEDRON :
    
    vertices[0] = HEXAfaces[ faceCellIdx ][ 0 ];
    for( i = 1 ; i < vertices[0]+1 ; i++ ) {
      vertices[i] = point( HEXAfaces[ faceCellIdx ][ i ] );
    }
    break;
    
  default:
    
    throw( "Could not determine the type of the cell. Aborting..." );
    
  }

}


#ifdef SMP_SGI

//--------------------------------------------------------------------------------
int Cell::onlyIWasSwept( unsigned vertexGlobalIdx,
			 unsigned faceCellIdx,
			 vector<bool> &is_point_transformed_VEC)
{
  
  int i, numVertices;
  switch( _type ) {
    
  case TETRAHEDRON :
    
    numVertices = TETRAfaces[ faceCellIdx ][ 0 ];
    for( i = 0 ; i < numVertices ; i++ ) {
      unsigned otherGlobalIdx = point( TETRAfaces[ faceCellIdx ][ i+1 ] );
      if( otherGlobalIdx != vertexGlobalIdx ) {
	if( is_point_transformed_VEC[ otherGlobalIdx ] )
	  return false;
      }
    }
    break;
    
  case HEXAHEDRON :
    
    numVertices = HEXAfaces[ faceCellIdx ][ 0 ];
    for( i = 0 ; i < numVertices ; i++ ) {
      unsigned otherGlobalIdx = point( HEXAfaces[ faceCellIdx ][ i+1 ] );
      if( otherGlobalIdx != vertexGlobalIdx ) {
	if( is_point_transformed_VEC[ otherGlobalIdx ] )
	  return false;
      }
    }
    break;
    
  default:
    
    throw( "Could not determine the type of the cell. Aborting..." );
    
  } 

  return true;

}

#endif




//--------------------------------------------------------------------------------
int Cell::onlyIWasSwept( unsigned vertexGlobalIdx,
			   unsigned faceCellIdx,
			   vector<Point> &tf_points_VEC ) {
  
  int i, numVertices;
  switch( _type ) {
    
  case TETRAHEDRON :
    
    numVertices = TETRAfaces[ faceCellIdx ][ 0 ];
    for( i = 0 ; i < numVertices ; i++ ) {
      unsigned otherGlobalIdx = point( TETRAfaces[ faceCellIdx ][ i+1 ] );
      if( otherGlobalIdx != vertexGlobalIdx ) {
	if( tf_points_VEC[ otherGlobalIdx ].marked( MARK_SWEPT ) )
	  return false;
      }
    }
    break;
    
  case HEXAHEDRON :
    
    numVertices = HEXAfaces[ faceCellIdx ][ 0 ];
    for( i = 0 ; i < numVertices ; i++ ) {
      unsigned otherGlobalIdx = point( HEXAfaces[ faceCellIdx ][ i+1 ] );
      if( otherGlobalIdx != vertexGlobalIdx ) {
	if( tf_points_VEC[ otherGlobalIdx ].marked( MARK_SWEPT ) )
	  return false;
      }
    }
    break;
    
  default:
    
    throw( "Could not determine the type of the cell. Aborting..." );
    
  } 

  return true;

}
//--------------------------------------------------------------------------------
// Return the Cell Index of the vertex, or -1 if fail
float Cell::getMaxZ( vector<Point> &tf_points_VEC) {

  if( _type != TETRAHEDRON && _type != HEXAHEDRON )
    throw( "Could not determine the type of the cell. Aborting..." );
  int i, nvert = getNumVertices();
  float maxZ = tf_points_VEC[ point(0) ].getZ();
  for( i = 1 ; i < nvert ; i++ ) {
    float currZ = tf_points_VEC[ point(i) ].getZ();
    if( currZ > maxZ ) maxZ = currZ;
  }
  return maxZ;

}

//--------------------------------------------------------------------------------
// Return the Cell Index of the vertex, or -1 if fail
int Cell::Belong( unsigned  currVertexGlobalIdx ) {

  if( _type != TETRAHEDRON && _type != HEXAHEDRON )
    throw( "Could not determine the type of the cell. Aborting..." );

  int i, nvert = getNumVertices();

  for( i = 0 ; i < nvert ; i++ ) {
    if( currVertexGlobalIdx == point( i ) ) 
      return i;
  }
  return -1;
}

/*
void  GetAdjacentFaces(int v0Index, int v1Index, struct CellInfo cell, 
		       Vertex *faceArray0[], int *nFaceArray0,
		       Vertex *faceArray1[], int *nFaceArray1)
{
  int i;
  int edgeNumber;
  int faces[2];

  switch(cell.cellTypeIndex) {

  case TETRAHEDRON:
    
    edgeNumber = TETRAedgeN[v0Index][v1Index];
    faces[0] = TETRAfaceEdge[edgeNumber][0];
    faces[1] = TETRAfaceEdge[edgeNumber][1];

    *nFaceArray0 = TETRAface[faces[0]][0];
    for(i = 1; i < (1+(*nFaceArray0)); i++) {
      faceArray0[(i-1)] = 
	tetraList[cell.cellIndex].v[TETRAface[faces[0]][i]];
    }

    *nFaceArray1 = TETRAface[faces[1]][0];
    for(i = 1; i < (1+(*nFaceArray1)); i++) {
      faceArray1[(i-1)] = 
	tetraList[cell.cellIndex].v[TETRAface[faces[1]][i]];
    }
    
    break;
    
  case HEXAHEDRON:

    edgeNumber = HEXAedgeN[v0Index][v1Index];
    faces[0] = HEXAfaceEdge[edgeNumber][0];
    faces[1] = HEXAfaceEdge[edgeNumber][1];

    *nFaceArray0 = HEXAface[faces[0]][0];
    for(i = 1; i < (1+(*nFaceArray0)); i++) {
      faceArray0[(i-1)] = 
	hexaList[cell.cellIndex].v[HEXAface[faces[0]][i]];
    }

    *nFaceArray1 = HEXAface[faces[1]][0];
    for(i = 1; i < (1+(*nFaceArray1)); i++) {
      faceArray1[(i-1)] = 
	hexaList[cell.cellIndex].v[HEXAface[faces[1]][i]];
    }

    break;
    
  default:
    fprintf(stderr,"Could not determine the type of the cell. Aborting...\n");
    exit(-1);
  }
  
}
*/
//------------------------------------------------------------------------------
