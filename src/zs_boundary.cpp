/* -*-C++-*-
*******************************************************************************
*
* File:         zs_boundary.cpp
* Description:  Belong to class scene from zs_scene.hh
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Jan 30 2000
* Modified:     Fri Jan 30 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
* Comment:      Adapted from CSilva's boundary code that was base on 
*               Ashish Tiwari's code
*
* (C) Copyright 2000
*
*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "zs_scene.hh"

//------------------------------------------------------------------------
unsigned Scene::_maxCellList( void ) {
  
  unsigned mcl = 0; // Short for maxCellList
  for( unsigned i = 0 ; i < _nPoints ; i++ ) {
    unsigned len = _points_VEC[i].getCellVectorSize();
    if( len > mcl ) mcl = len;
  }
  return mcl;

}

//------------------------------------------------------------------------
unsigned Scene::_sumCellList( void ) {
  
  unsigned scl = 0; // Short for sumCellList
  for( unsigned i = 0 ; i < _nPoints ; i++ ) {
    scl += _points_VEC[i].getCellVectorSize();
  }
  return scl;

}

//------------------------------------------------------------------------
void Scene::_buildVertexLists( void ) {

  // Create for each point, a list with the indices for its cells
  unsigned i,j;
  // Count the number of tetrahedra that belong to each vertex.
  int *numCells = new int[ _nPoints ];
  if( numCells == NULL ) 
    throw( "Could not allocate memory for numCells.");
  // Zero Counters
  for( i = 0; i<_nPoints; i++ ) *(numCells+i) = 0;
  // Count the number of cells for each point
  for( i = 0 ; i < _nCells ; i++ ) {
    Cell& cell = _cells_VEC[i];
    for( j = 0 ; j < (unsigned)cell.getNumVertices() ; j++ ) {
      ( *( numCells + cell.point(j) ) )++;
    }
  }
  
#if 0
  for( i = 0 ; i < _nPoints ; i++ ) 
    cout << "Point " << i << " belongs to " << *(numCells+i) << "cells."<< endl;
#endif
  
  // Create space for the lists
  // ***debug
  unsigned ttotal = 0;
  // ***end
  for( i = 0 ; i < _nPoints ; i++ ) {
    // ***debug
    if( *(numCells+i) == 0 ) {
    //  cout << "Point # " << i << " Isolated" << endl;
      ttotal++;
    }
    // ***end
    _points_VEC[i].reserveCellVector( *(numCells+i) );
  }
  // ***end
  cout << "Total of isolated points " << ttotal << endl;
  // ***end

  // We don't need the counters anymore
  //delete numCells;
  
  // Push_back cell indices for all points vector
  for( i = 0 ; i < _nCells ; i++ ) {
    Cell& cell = _cells_VEC[i];
    for( j = 0 ; j < (unsigned)cell.getNumVertices() ; j++ ) {
      unsigned pt = cell.point(j);
      _points_VEC[pt].pushbackCell( i );
      assert( _points_VEC[pt].getCellVectorSize() <= *(numCells+pt) );
    }
  }

  // We don't need the counters anymore
  delete numCells;

#if 0
  // List the cell indices of all points' cell vectors
  for( i = 0 ; i < _nPoints ; i++ ) {
    unsigned vs = _points_VEC[i].getCellVectorSize();
    assert( vs > 0 );
    cout << "Point #" << i << " : ";
    for( j = 0 ; j < vs ; j++ ) {
      cout << _points_VEC[i].getCell( j ) << " ";
    }
    cout << "|" << endl;
  }
#endif
  
}

//------------------------------------------------------------------------
// Determines if a vertex belongs to a Cell
int Scene::_inPointCell( unsigned pId, Cell *c ) {

  int numcell = c->getNumVertices();
  for( int i = 0 ; i < numcell ; i++ )
    if( pId == c->point(i) )
      return true;
  return false;

}

//------------------------------------------------------------------------
// Marks in each Cell, which of its faces are external
void Scene::_compExtFaces( void ) {
  unsigned i,j;
  unsigned pId[32];

  for( i = 0 ; i < _nCells ; i++) {

    Cell *currCell = &_cells_VEC[i];
    unsigned numVertices = currCell->getNumVertices();
#ifdef _MAKESPARSE
    _neighbor[i].n = 0; // Zero counter of neighbor cells
#endif

    if( currCell->getCellType() == TETRAHEDRON ) {

      for( j = 0 ; j < numVertices ; j++ )
	pId[j] = currCell->point(j);
      
      // For every cell that p[0] belongs to...
      Point *p0 = &_points_VEC[ pId[0] ];
      for( j = 0 ; j < p0->getCellVectorSize() ; j++) {
	
	unsigned nextCellIdx = p0->getCell(j);
	Cell *nextCell = &_cells_VEC[ nextCellIdx ];
	
	if( nextCell == currCell ) continue;
	
	if( _inPointCell( pId[1], nextCell ) ) {
	  if( _inPointCell( pId[2], nextCell ) ) {
	    // face p0-p1-p2 belongs to both tetrahedra, so it is internal
	    currCell->mark( MARK_FACE0_INTERNAL );
#ifdef _MAKESPARSE
	    _neighbor[i]._ti[ _neighbor[i].n++ ] = nextCellIdx;
#endif
	  } else {
	    if( _inPointCell( pId[3], nextCell ) ) {
	      // face p0-p1-p3 belongs to both tetrahedra, so it is internal    
	      currCell->mark( MARK_FACE1_INTERNAL );
#ifdef _MAKESPARSE
	      _neighbor[i]._ti[ _neighbor[i].n++ ] = nextCellIdx;
#endif
	    }
	  }
	} else {
	  if( _inPointCell( pId[2], nextCell ) && _inPointCell( pId[3], nextCell ) ) {
	    // face v0-v2-v3 belongs to both tetrahedra, so it is internal   
	    currCell->mark( MARK_FACE2_INTERNAL );
#ifdef _MAKESPARSE
	    _neighbor[i]._ti[ _neighbor[i].n++ ] = nextCellIdx;
#endif
	  }
	}
      }

      // For every tetrahedron that p[1] belong to...
      Point *p1 = &_points_VEC[ pId[1] ];
      for( j = 0 ; j < p1->getCellVectorSize() ; j++ ) {
	
	unsigned nextCellIdx = p1->getCell(j);
	Cell *nextCell = &_cells_VEC[ nextCellIdx ];
	
	if( nextCell == currCell ) continue;
	
	if( _inPointCell( pId[2], nextCell ) && _inPointCell( pId[3], nextCell ) ) {
	  // face v1-v2-v3 belongs to both tetrahedra, so it is internal
	  currCell->mark( MARK_FACE3_INTERNAL );
#ifdef _MAKESPARSE
	  _neighbor[i]._ti[ _neighbor[i].n++ ] = nextCellIdx;
#endif
	}
      }
      
#ifdef _MAKESPARSE
      if( _neighbor[i].n > 4 ) {
	cout << "Error: There exist isolated cells in the mesh" << endl;
	cout << "Hint : You must adapt the struct Neighbor in zs_scene.hh" << endl;
	exit(1);
      }
#endif

    } else { // HEXAHEDRON

#ifdef _MAKESPARSE
      throw( "Cannot create Sparse file for hexahedral dataset." );
#endif

      /*-----------------------------------------------------------

         Vertex 0 is pivot for faces 0, 2 and 4
                              7
                              o                    Face  Vertices
                             /|                     0     0:1:2:3
                            / |                     2     0:1:5:4
                           /  |                     4     0:3:7:4
                          /   |
                        3o--------------o2
                         |    |         |
                         |    |4        |
                         |    o---------|----o 5
                         |   /          |   /
                         |  /           |  /
                         | /            | /
                         |/             |/
                         o--------------o
                        0                1


         Vertex 6 is pivot for faces 1, 3 and 5

                              7               6
                              o--------------o     Face  Vertices
                             /|             /|      1     4:5:6:7
                            / |            / |      3     3:2:6:7
                           /  |           /  |      5     1:2:6:5
                          /   |          /   |
                        3o--------------o2   |
                              |         |    |
                              |4        |    |
                              o---------|----o 5
                                        |   /
                                        |  /
                                        | /
                                        |/
                                        o
                                         1
      */

      //###############################################################
      for( j = 0 ; j < numVertices ; j++ )
	pId[j] = currCell->point(j);
      
      // For every cell that p[0] belongs to...
      Point *p0 = &_points_VEC[ pId[0] ];
      for( j = 0 ; j < p0->getCellVectorSize() ; j++) {
	
	unsigned nextCellIdx = p0->getCell(j);
	Cell *nextCell = &_cells_VEC[ nextCellIdx ];
	
	if( nextCell == currCell ) continue;
	
	if( _inPointCell( pId[1], nextCell ) ) {
	  if( _inPointCell( pId[2], nextCell ) ) {
	    if( _inPointCell( pId[3], nextCell ) ) {
	      // FACE 0 : belongs to both hexa. It is internal
	      currCell->mark( MARK_FACE0_INTERNAL );
	    }
	  } else {
	    if( _inPointCell( pId[5], nextCell ) ) {
	      if( _inPointCell( pId[4], nextCell ) ) {
		// FACE 2 : belongs to both hexa. It is internal    
		currCell->mark( MARK_FACE2_INTERNAL );
	      }
	    }
	  }
	} else {
	  if( _inPointCell( pId[3], nextCell ) && 
	      _inPointCell( pId[4], nextCell ) &&
	      _inPointCell( pId[7], nextCell ) ) {
	    // FACE 4 belongs to both tetra. It is internal   
	    currCell->mark( MARK_FACE4_INTERNAL );
	  }
	}

      }

      // For every cell that p[6] belongs to...
      Point *p6 = &_points_VEC[ pId[6] ];
      for( j = 0 ; j < p6->getCellVectorSize() ; j++) {
	
	unsigned nextCellIdx = p6->getCell(j);
	Cell *nextCell = &_cells_VEC[ nextCellIdx ];
	
	if( nextCell == currCell ) continue;
	
	if( _inPointCell( pId[2], nextCell ) ) {
	  if( _inPointCell( pId[1], nextCell ) ) {
	    if( _inPointCell( pId[5], nextCell ) ) {
	      // FACE 5 : belongs to both hexa. It is internal
	      currCell->mark( MARK_FACE5_INTERNAL );
	    }
	  } else {
	    if( _inPointCell( pId[3], nextCell ) ) {
	      if( _inPointCell( pId[7], nextCell ) ) {
		// FACE 3 : belongs to both hexa. It is internal    
		currCell->mark( MARK_FACE3_INTERNAL );
	      }
	    }
	  }
	} else {
	  if( _inPointCell( pId[4], nextCell ) && 
	      _inPointCell( pId[5], nextCell ) &&
	      _inPointCell( pId[7], nextCell ) ) {
	    // FACE 1 belongs to both tetra. It is internal   
	    currCell->mark( MARK_FACE1_INTERNAL );
	  }
	}

      }


      //#######################################################################

    }

  }

}

//------------------------------------------------------------------------
// Marks all the vertices of an external face as external. 
void Scene::_compExtPoints( void ) {

  unsigned i, j;
  Point *p[32];
  _numBTFaces = _numBHFaces = 0;

  for( i = 0 ; i < _nCells ; i++ ) {
    
    Cell& currCell = _cells_VEC[i];
    unsigned numVertices = currCell.getNumVertices();
    for( j = 0 ; j < numVertices ; j++ )
      p[j] = &_points_VEC[ currCell.point(j) ] ;
    
    if( currCell.getCellType() == TETRAHEDRON ) {
      
      unsigned bFaces = 4;
      if( !currCell.marked(MARK_FACE0_INTERNAL) ) {
	p[0]->mark( MARK_BOUNDARY );
	p[1]->mark( MARK_BOUNDARY );
	p[2]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      if( !currCell.marked(MARK_FACE1_INTERNAL) ) {
	p[0]->mark( MARK_BOUNDARY );
	p[1]->mark( MARK_BOUNDARY );
	p[3]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      if( !currCell.marked(MARK_FACE2_INTERNAL) ) {
	p[0]->mark( MARK_BOUNDARY );
	p[2]->mark( MARK_BOUNDARY );
	p[3]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      if( !currCell.marked(MARK_FACE3_INTERNAL) ) {
	p[1]->mark( MARK_BOUNDARY );
	p[2]->mark( MARK_BOUNDARY );
	p[3]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      _numBTFaces += bFaces;
      
    } else {

      unsigned bFaces = 6;
      if( !currCell.marked(MARK_FACE0_INTERNAL) ) {
	p[0]->mark( MARK_BOUNDARY );
	p[1]->mark( MARK_BOUNDARY );
	p[2]->mark( MARK_BOUNDARY );
	p[3]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      if( !currCell.marked(MARK_FACE1_INTERNAL) ) {
	p[4]->mark( MARK_BOUNDARY );
	p[5]->mark( MARK_BOUNDARY );
	p[6]->mark( MARK_BOUNDARY );
	p[7]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      if( !currCell.marked(MARK_FACE2_INTERNAL) ) {
	p[0]->mark( MARK_BOUNDARY );
	p[1]->mark( MARK_BOUNDARY );
	p[4]->mark( MARK_BOUNDARY );
	p[5]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      if( !currCell.marked(MARK_FACE3_INTERNAL) ) {
	p[2]->mark( MARK_BOUNDARY );
	p[3]->mark( MARK_BOUNDARY );
	p[6]->mark( MARK_BOUNDARY );
	p[7]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      if( !currCell.marked(MARK_FACE4_INTERNAL) ) {
	p[0]->mark( MARK_BOUNDARY );
	p[3]->mark( MARK_BOUNDARY );
	p[4]->mark( MARK_BOUNDARY );
	p[7]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      if( !currCell.marked(MARK_FACE5_INTERNAL) ) {
	p[1]->mark( MARK_BOUNDARY );
	p[2]->mark( MARK_BOUNDARY );
	p[5]->mark( MARK_BOUNDARY );
	p[6]->mark( MARK_BOUNDARY );
	bFaces--;
      }
      _numBHFaces += bFaces;

    }
    
  }
  _numBPoints = 0;
  for( i = 0 ; i < _nPoints ; i++ ) {
    Point& cp = _points_VEC[ i ] ;
    if(  cp.marked( MARK_BOUNDARY ) ) _numBPoints++;
  }

}

/* Main function of the file. It computes the external faces
 * as a means of computing the external vertices.
 */
void Scene::_compExt( void ) {

  _compExtFaces();
  _compExtPoints();

}

// End of Module zs_boundary.cpp ------------------------------------------
