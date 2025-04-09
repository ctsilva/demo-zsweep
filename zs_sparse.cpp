/* -*-C++-*-
*******************************************************************************
*
* File:         zs_sparse.cpp
* Description:  
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Feb 4 2000
* Modified:     Fri Feb 6 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* (C) Copyright 2000
*
*******************************************************************************
*/
#include <string.h>
#include "zs_scene.hh"

//.......................................................................
void Scene::_saveSparseFile( char *filename ) {
/*
  unsigned numCell = 0;
  {
    for( unsigned i = 0 ; i<_nCells ; i++ )
      if( _cells_VEC[i].marked( MARK_NEEDED ) )
	numCell++;
  }

  
  char *outFileName = new char[200];
  if( outFileName == NULL )
    throw( "Could not allocate memory for outFileName." );
  strcpy( outFileName, "SPARE" );
  strcat( outFileName, filename );
  ofstream outFile( outFileName );
  outFile << _nPoints << "    " << numCell << "\n";
  cout << "Sparse File: points = " << _nPoints ;
  cout << " : tetra = " << numCell << "\n";

  unsigned i ;
  for( i = 0 ; i < _nPoints ; i++ ) {
    Point& p = _points_VEC[i];
    outFile << p.getX() << " ";
    outFile << p.getY() << " ";
    outFile << p.getZ() << " ";
    outFile << p.getVal() << endl;
    //outFile << setw(15) << setprecision(10) << p.getX() << " ";
    //outFile << setw(15) << setprecision(10) << p.getY() << " ";
    //outFile << setw(15) << setprecision(10) << p.getZ() << " ";
    //outFile << setw(4) << p.getVal() << endl;
  }

  for( i = 0 ; i < _nCells ; i++ ) {
    Cell& c = _cells_VEC[i];
    if( c.marked( MARK_NEEDED ) ) {
      for( int j = 0 ; j < 4 ; j++ )
	outFile << setw(6) << c.point(j) << " ";
      outFile << endl;
    }
  }

  outFile.close();
  delete outFileName;
*/
}

/* Marks all the vertices of an external face as external. */
void Scene::_sumCellsNeighbors( void ) {

  unsigned i;

  for( i = 0 ; i < _nCells ; i++ ) {
    
    Cell& currCell = _cells_VEC[i];
    switch( _neighbor[i].n ) {
    case 4:
      currCell.unmark( MARK_BOUNDARY );
      _numInterior++;
      break;
    case 3:
      currCell.mark( MARK_BOUNDARY );
      currCell.mark( MARK_NEEDED );
      _num1BFace++;
      break;
    case 2:
      currCell.mark( MARK_BOUNDARY );
      currCell.mark( MARK_NEEDED );
      _num2BFace++;
      break;
    case 1:
      currCell.mark( MARK_BOUNDARY );
      currCell.mark( MARK_NEEDED );
      _num3BFace++;
      break;
    case 0:
      currCell.mark( MARK_BOUNDARY );
      currCell.mark( MARK_NEEDED );
      currCell.mark( MARK_ISOLATED );
      _num4BFace++;
      break;
    }
    
  }
  assert( _num1BFace+_num2BFace+_num3BFace+_num4BFace+_numInterior == _nCells );
  //#ifdef OUTPUT
  cout << "  # of 4BFace Cells   : " << setw(6) << _num4BFace << endl;
  cout << "  # of 3BFace Cells   : " << setw(6) << _num3BFace << endl;
  cout << "  # of 2BFace Cells   : " << setw(6) << _num2BFace << endl;
  cout << "  # of 1BFace Cells   : " << setw(6) << _num1BFace << endl;
  cout << "  # of Interior Cells : " << setw(6) << _numInterior << endl;
  //#endif
  
}

//.......................................................................
int Scene::_canCellBeGhost( unsigned ti ) {

  if( _cells_VEC[ _neighbor[ti]._ti[ 0 ] ].marked( MARK_GHOST ) ||
      _cells_VEC[ _neighbor[ti]._ti[ 1 ] ].marked( MARK_GHOST ) ||
      _cells_VEC[ _neighbor[ti]._ti[ 2 ] ].marked( MARK_GHOST ) ||
      _cells_VEC[ _neighbor[ti]._ti[ 3 ] ].marked( MARK_GHOST )
      ) return false;
  return true;

}

//.................................................................
// Mark all ghost cells
void Scene::_clickInteriorCells( deque<unsigned> &tf_cells_DEQ  ) {

  assert( tf_cells_DEQ.size() > 0 );

  do {

    unsigned currCellIdx = tf_cells_DEQ[0];
    Cell&    currCell    = _cells_VEC[ currCellIdx ];
    assert( currCell.getCellType() == TETRAHEDRON );
    assert( _neighbor[currCellIdx].n == 4 );
    tf_cells_DEQ.pop_front();

    if( currCell.marked( MARK_GHOST ) ) {

      for( unsigned j = 0 ; j < (unsigned)_neighbor[currCellIdx].n ; j++ ) {
	
	unsigned neigCellIdx = _neighbor[currCellIdx]._ti[j];
	Cell&    neigCell    = _cells_VEC[ neigCellIdx ];

	if( neigCell.marked( MARK_GHOST ) ) {
	  cout << "Error: Two ghost neighbors!!!!!!!" << endl;
	  exit(1);
	}

	if( neigCell.marked( MARK_NEEDED ) ) continue;
	
	neigCell.mark( MARK_NEEDED );
	tf_cells_DEQ.push_back( neigCellIdx );
	
      }

    } else {
      if( currCell.marked( MARK_NEEDED ) ) {

	for( unsigned j = 0 ; j < (unsigned)_neighbor[currCellIdx].n ; j++ ) {
	  
	  unsigned neigCellIdx = _neighbor[currCellIdx]._ti[j];
	  Cell&    neigCell    = _cells_VEC[ neigCellIdx ];
	  if( neigCell.marked( MARK_GHOST ) || neigCell.marked( MARK_NEEDED ) )
	    continue;
	  
	  if( _canCellBeGhost( neigCellIdx ) )
	    neigCell.mark( MARK_GHOST );
	  else 
	    neigCell.mark( MARK_NEEDED );
	  tf_cells_DEQ.push_back( neigCellIdx );
	  
	}

      } else {
	cout << "Error in tetra flag." << endl;
	exit(1);
      }
    }

  } while( tf_cells_DEQ.begin() != tf_cells_DEQ.end() );

}

//.............................................................................
// Mark all ghost cells
void Scene::_clickAllCellsNumbered( unsigned qtt, deque<unsigned> &tf_cells_DEQ ) {

  for( unsigned i = 0 ; i < _nCells ; i++) {
    
    if( (unsigned)_neighbor[i].n == qtt ) {

      Cell& currCell = _cells_VEC[ i ];
      if( currCell.getCellType() != TETRAHEDRON || !currCell.marked( MARK_NEEDED ) )
	throw( "Error in function: Scene::_clickAllCellsNumbered ..." );

      for( unsigned j = 0 ; j < qtt ; j++ ) {

	unsigned neigCellIdx = _neighbor[i]._ti[j];
	Cell&    neigCell    = _cells_VEC[ neigCellIdx ];
	if( neigCell.marked( MARK_GHOST ) || neigCell.marked( MARK_NEEDED ) )
	  continue;

	if( _canCellBeGhost( neigCellIdx ) )
	  neigCell.mark( MARK_GHOST );
	else 
	  neigCell.mark( MARK_NEEDED );
	tf_cells_DEQ.push_back( neigCellIdx );

      }

    }
    
  }
  
}

//..............................................................
// Mark all ghost cells
void Scene::_graphClick( void ) {

  deque<unsigned>  _cells_DEQ;
  _sumCellsNeighbors();
  _clickAllCellsNumbered( 1, _cells_DEQ ); // Click all nodes with degree 1
  _clickAllCellsNumbered( 2, _cells_DEQ ); // Click all nodes with degree 2
  _clickAllCellsNumbered( 3, _cells_DEQ ); // Click all nodes with degree 3
  _clickInteriorCells( _cells_DEQ );

}

//-----------------------------------------------------------------------------
