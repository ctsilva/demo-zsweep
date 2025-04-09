/* -*-C++-*-
*******************************************************************************
*
* File:         zs_readToff.cpp
* Description:  
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Jan 28 2000
* Modified:     Fri Feb 11 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* (C) Copyright 2000
*
*******************************************************************************
*/
#include "zs_scene.hh"

#define _BUF_SIZE  500

void Scene::_getNextLine( char *buf, FILE *f ) {

  do {
    fgets( buf, _BUF_SIZE-1 , f );
  } while( *buf == '#' || *buf == '\n' );
  int i = 0;
  while( *(buf+i) != '\n' ) i++;
  assert( *(buf+i) == '\n' );
  *(buf+i) = 0;

}

//-----------------------------------------------------------------------------
void Scene::_readToff( char *toffFile ) {

  char     buf[_BUF_SIZE];
  unsigned i, x[10];
  unsigned nT = 0, nH = 0;
  float    coord[4];

  FILE *f = fopen(toffFile,"r");
  if (f==NULL) {
    cout << "InputFile: " << toffFile << " : ";
    throw ("Can not open TOFF file");
  }

  _getNextLine( buf, f );
  if( sscanf( buf, "%u %u", &_nPoints, &_nCells) == 1 ) {
    _getNextLine( buf, f );
    if( sscanf( buf, "%u", &_nCells ) != 1 ) {
      throw( "Error: In the header of TOFF file");
    }
  }

  // Reading Points and Tetrahedra ............................................
  _points_VEC.reserve( _nPoints );
  {
    for( i = 0 ; i < _nPoints ; i++ ) {

      _getNextLine( buf, f );
      if( sscanf( buf, "%f %f %f %f", &coord[0], &coord[1], &coord[2], &coord[3]) != 4)
	throw ("Can not read TOFF point");
      _points_VEC.push_back( Point( coord[0], coord[1], coord[2], coord[3], i ) );
      
      // Build also the bounding box
      if( i == 0 ) {
	_low[0] = _high[0] = x[0];
	_low[1] = _high[1] = x[1];
	_low[2] = _high[2] = x[2];
      } else {
	if( x[0] <  _low[0] )  _low[0] = x[0];
	if( x[0] > _high[0] ) _high[0] = x[0];
	if( x[1] <  _low[1] )  _low[1] = x[1];
	if( x[1] > _high[1] ) _high[1] = x[1];
	if( x[2] <  _low[2] )  _low[2] = x[2];
	if( x[2] > _high[2] ) _high[2] = x[2];
      }
#ifdef OUTPUT
      if( i>0 && i%10000 == 0 )
	cout << "Points read : " << setw(6) << i << "\n";
#endif
      
    }
    cout << "Points read : " << setw(6) << i << "\n";
  }

  // read all tetrahedra
  _cells_VEC.reserve( _nCells );
  {
    unsigned i;
    for ( i = 0; i<_nCells; i++) {
      
#ifdef OUTPUT
      if( i%5000 == 0 )
	cout << "Tetra read  : " << setw(6) << i << "\n";
#endif

      _getNextLine( buf, f );
      int type = sscanf( buf, "%u %u %u %u %u %u %u %u %u %u", 
			 &x[0], &x[1], &x[2], &x[3],
			 &x[4], &x[5], &x[6], &x[7],
			 &x[8], &x[9] );
      
      if( type == 4 ) {
	nT++;
	_cells_VEC.push_back( Cell( x[0],x[1],x[2],x[3] ) );
      } else {
	if( type == 8 ) {
	  nH++;
	  _cells_VEC.push_back( Cell( x[0],x[1],x[2],x[3],
				      x[4],x[5],x[6],x[7] ) );
	} else {
	  throw ("ERROR reading cells in toff file");
	}
      }    
    }
    cout << "# of Cells read : " << setw(6) << i << "\n";

  }
  cout << "# of Tetra = " << nT << endl;
  cout << "# of Hexa  = " << nH << endl;
  fclose(f); 

}
//-----------------------------------------------------------------------------
