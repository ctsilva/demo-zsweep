/* -*-C++-*-
*******************************************************************************
*
* File:         zs_scene.cpp
* Description:  
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Jan 28 2000
* Modified:     Fri Jan 28 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* (C) Copyright 2000
*
*******************************************************************************
*/
#include "zs_scene.hh"
#include <iostream>
#include <fstream>
using namespace std;

//-----------------------------------------------------------------------------
//#define _CHECKLIST__
void Scene::_checkSquareFace( int n, unsigned *vert ) {
#ifdef _CHECKLIST__
  static unsigned lastCheckList = 0;
  static unsigned checkList[100000][10];
  unsigned k=0, _v[4];
  
  for( k = 0 ; k < 4 ; k++ ) _v[k] = vert[k];

  char flag=true;
  while( flag ) {
    flag = false;
    for( k = 0 ; k < 3 ; k++ ) {
      if( _v[k] > vert[k+1] ) {
	unsigned aux = _v[k];
	_v[k]   = _v[k+1];
	_v[k+1] = aux;
	flag = true;
	break;
      }
    }
  }

  k = 0;
  while( k < lastCheckList ) {

    if( _v[0] == checkList[k][0] && _v[1] == checkList[k][1] && 
	_v[2] == checkList[k][2] && _v[3] == checkList[k][3] 
	) {

      if( 
	 checkList[k][4] != vert[n]       ||
	 checkList[k][5] != vert[(n+1)%4] ||
	 checkList[k][6] != vert[(n+2)%4] ||
	 checkList[k][7] != vert[n]       ||
	 checkList[k][8] != vert[(n+2)%4] ||
	 checkList[k][9] != vert[(n+3)%4]
	 ) {

	cout << "Error found in creating the faces from square faces\n";
	exit(1);

      }
      return;
    }
    k++;
  }

  assert( k == lastCheckList );
  checkList[k][0] = _v[0];
  checkList[k][1] = _v[1];
  checkList[k][2] = _v[2];
  checkList[k][3] = _v[3];
  checkList[k][4] = vert[n];
  checkList[k][5] = vert[(n+1)%4];
  checkList[k][6] = vert[(n+2)%4];
  checkList[k][7] = vert[n];
  checkList[k][8] = vert[(n+2)%4];
  checkList[k][9] = vert[(n+3)%4];
  
  lastCheckList++;

#endif
}

//-----------------------------------------------------------------------------
void Scene::_createCleanFile( void ) {
/*
  // Create new file without the isolated points
  cout << endl << endl << "Creation of Clean SPX required, wait..." << endl;
  ofstream outFile( "cleanSPX.off" );
  unsigned *newIdx = new unsigned[_nPoints];
  unsigned i, newNumPoint = 0;
  for( i = 0 ; i < _nPoints ; i++ ) {
    if( _points_VEC[i].getCellVectorSize() > 0 ) {
      newNumPoint++;
    }
  }
  outFile << newNumPoint << endl << _nCells << endl;
  unsigned k = 0;
  for( i = 0 ; i < _nPoints ; i++ ) {
    if( _points_VEC[i].getCellVectorSize() > 0 ) {
      Point& p = _points_VEC[i];
      outFile << p.getX() << " ";
      outFile << p.getY() << " ";
      outFile << p.getZ() << " ";
      outFile << p.getVal() << endl;
      newIdx[i] = k++; // Update conversion table
    }
  }
  // Now lets save cells
  for( i = 0 ; i < _nCells ; i++ ) {
    Cell& curr = _cells_VEC[i];
    unsigned a = curr.point( 0 );
    unsigned b = curr.point( 1 );
    unsigned c = curr.point( 2 );
    unsigned d = curr.point( 3 );
    outFile << newIdx[a] << " ";
    outFile << newIdx[b] << " ";
    outFile << newIdx[c] << " ";
    outFile << newIdx[d] << endl;
  }
  outFile.close();
  delete newIdx;
  cout << "Clean SPX.off craeted successfully" << endl;
  exit(0);
*/
}

//-----------------------------------------------------------------------------
void Scene::_listExternalPoints( void ) {

  // For Debug Only: list all external points
  cout << "List off Boudnary Points" << endl;
  for( unsigned i = 0 ; i < _nPoints ; i++ ) {
    if( _points_VEC[i].marked( MARK_BOUNDARY ) )
      cout << i << " : ";
  }
  cout << endl;

}

//-----------------------------------------------------------------------------
void Scene::_projectCellListOfGivenPoint( void ) {
#if 0

  unsigned PTEST = 15082;

  Point *p = &_points_VEC[ PTEST ];
  unsigned numCell = p->getCellVectorSize();
  for( unsigned i = 0 ; i < numCell ; i++ ) {
    
    unsigned cellIdx = p->getCell( i );
    Cell *cell = &_cells_VEC[cellIdx];
    unsigned currIdx = p->getId();
    // First lets find the other 3 vertices --------------------------
    unsigned otherPointsIdx[3];
    {
      int k = 0;
      for( int i = 0 ; i < 4 ; i++ ) {
	if( currIdx != cell->point(i) ) {
	  otherPointsIdx[k++] = cell->point(i);
	  assert( k<=3 );
	}
	assert( k==3 );
      }
    }
    // Now we project all faces that has no other vertex swept before
    for( int i = 0 ; i < 2 ; i++ ) {
      Point& pi = _points_VEC[ otherPointsIdx[i] ];
      for( int j = i+1 ; j < 3 ; j++ ) {
	Point& pj = _points_VEC[ otherPointsIdx[j] ];
	if( !pi.marked(MARK_SWEPT) && !pj.marked(MARK_SWEPT) ) {
	  _hash->insertUnit( currIdx, otherPointsIdx[i], otherPointsIdx[j] );
	}
      }
    }
  }
#endif
}
//-----------------------------------------------------------------------------
