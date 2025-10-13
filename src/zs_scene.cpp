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

Scene::Scene(void) 
    : _low(0,0,0), _high(0,0,0)
{

  // These variables will be summed at function _compExtPoints in zs_boundary.cpp
  _numInterior = 0;
  _num1BFace = _num2BFace = _num3BFace = _num4BFace = 0;

}

Scene::Scene(char *toffFile) 
    : _low(0,0,0), _high(0,0,0)
{
  unsigned totalMem  = 0;
  unsigned vertexMem = 0;
  unsigned cellMem   = 0;

  _edgeDisp   = MARK_BOUNDARY;
  _vertexDisp = _swept = _nonswept = true;
  _centralize = false;
  _newfaceDisp = _oldfaceDisp = true;
  _edgeDisp = _renderPause = _imageUpdated = false;
  _newedgeDisp = _oldedgeDisp = false;
  _currId = 0;
  _adjustX = _adjustY = 0;

  // These variables will be summed at function _compExtPoints in zs_boundary.cpp
  _numInterior = 0;
  _num1BFace = _num2BFace = _num3BFace = _num4BFace = 0;

#ifdef OUTPUT
  printf( "\n-----------------------------------------------------------\n" );
  printf( "Creating Scene ============================================\n" );
  printf( "-----------------------------------------------------------\n" );
#endif

  _readToff( toffFile );
  vertexMem = _nPoints * sizeof(Point);
  totalMem  = vertexMem;
  cellMem   = _nCells * sizeof(Cell);
  totalMem += cellMem;

  // Create for each point, a list with the indices of its cells
  _buildVertexLists();
  {
    // Only now allocate the number of Unit in the hash table.
    // This value is also used to allocate more memory if necessary. ;-)
    unsigned mcl = _maxCellList();
#ifdef OUTPUT
    unsigned avg = _sumCellList()/_nPoints;
    cout << "Longest Vertex CellList = " << mcl << endl;
    cout << "Average Vertex CellList = " << avg << endl;
#endif
    if( mcl > 0 ) allocHash( mcl );// Considering the # of faces
  }
  
  // Find out the external vertices
#ifdef _MAKESPARSE
  _neighbor = new Neighbor[_nCells];
#endif
  _compExt();
#ifdef _MAKESPARSE
  _graphClick();
  delete _neighbor;
#endif

#ifdef _MAKESPARSE
  {
    
    _saveSparseFile( toffFile );
    cout << "Just for debug purpose" << endl;
    exit(0);
  }
#endif

  // Debug Functions ''''''''''''''''''
  //_createCleanFile();
  //_listExternalPoints();

  //#ifdef OUTPUT
  {
    unsigned numCell = 0;
    unsigned max = 0;
    for( unsigned i = 0 ; i < _points_VEC.size() ; i++ ) {
      numCell += _points_VEC[i].getCellVectorSize();
      max = ( max < _points_VEC[i].getCellVectorSize() )?
	_points_VEC[i].getCellVectorSize():max;
    }
#ifdef OUTPUT
    cout << "Maximum Cells list (in bytes) = " << setw(6) << max << "\n";
#endif
    totalMem += numCell * sizeof(int);
  }
  //#endif

  cout << "Scene::Scene(toffFile):\n";
  cout << "   #Points = " << setw(7) << _points_VEC.size() << " : MemSize = ";
  cout << setw(7) << _points_VEC.size()*sizeof(Point) << "\n";
  cout << "   #Cells  = " << setw(7) << _cells_VEC.size()  << " : MemSize = ";
  cout << setw(7) << _cells_VEC.size()*sizeof(Cell) << "\n";

  //#ifdef OUTPUT
  cout << "   Total Memory used by scene structure : " << totalMem << "\n";
  //#endif

  Center();
  ShiftValue();

  // Debug Functions ''''''''''''''''''
  //_projectCellListOfGivenPoint();
  
}

void Scene::MakeBBox( vector<Point> &tf_points_VEC )
{
  unsigned i,t;
  /* Start by the first point (anyone) */
  {
    Point& p = tf_points_VEC[ 0 ];
    for( i=0; i < 3 ; i++ )
      {
	_low[i] = _high[i] = p[i];
      }
  }
  /* Expand it around others triangle of the boundary */
  for( t=0; t < tf_points_VEC.size() ; t++ ) {
    for( int np=0 ; np < 3 ; np++ ) {
      Point& p = tf_points_VEC[t];
      for( i = 0 ; i < 3 ; i++ ) {
	if ( _low[i]  > p[i] ) _low[i]  = p[i];
	if ( _high[i] < p[i] ) _high[i] = p[i];
      }
    }
  }
  
}

void Scene::Rotate(vector<Point> &tf_points_VEC, ViewPlane *vp ) 
{
  for( unsigned t=0 ; t < tf_points_VEC.size() ; t++ ) 
    {
      vp->RotatePoint( tf_points_VEC[t] );
    }
}

void Scene::Center()
{
  MakeBBox( _points_VEC );
  float x0 = (_low[0]+_high[0])/2.0;
  float y0 = (_low[1]+_high[1])/2.0;
  float z0 = (_low[2]+_high[2])/2.0;
  /* Subtract (x0,y0,z0) from all points and scale so deltaZ=1.0 */
  for (unsigned i=0; i<_nPoints; i++)
    {
      Point *p = & _points_VEC[i];
      (*p)[0] -= x0;
      (*p)[1] -= y0;
      (*p)[2] -= z0;
      //float scale = 1;
      float scale = (_high[2]-_low[2]);
      (*p)[0] /= scale;
      (*p)[1] /= scale;
      (*p)[2] /= scale;
    }    

  MakeBBox( _points_VEC );
  _sceneSize =  sqrt(_high[0]*_high[0]+_high[1]*_high[1]+_high[2]*_high[2]);
#ifdef OUTPUT
  printf("Scene::Center:\n   Moved to (%7.4f,%7.4f,%7.4f)\n", x0,y0,z0 );
  printf("   BoundingBox (%7.4f,%7.4f,%7.4f) -> (%7.4f,%7.4f,%7.4f)\n", 
	 _low[0],_low[1],_low[2],_high[0],_high[1],_high[2]);
#endif
  //return scenesize;
}

void Scene::ShiftValue()
{
  float minValue = FLT_MAX;
  float maxValue = -FLT_MAX;
  unsigned i;
  for (i=0; i<_nPoints; i++)
    {
      Point *p = & _points_VEC[i];
      if (p->value() && p->value() < minValue)
	minValue = p->value();
      if (p->value() > maxValue)
	maxValue = p->value();
    }
#ifdef OUTPUT
  printf("Scene::ShiftValue:\n   Values found: [%f..%f], scaled to [0..255]\n", 
	 minValue, maxValue);
#endif
  maxValue -= minValue;
  if( maxValue == 0 ) maxValue = _DELTA;
  for (i=0; i<_nPoints; i++)
    {
      Point *p = & _points_VEC[i];
      p->value() -= minValue;
      p->value() *= 255.0/maxValue;
    }    
}

void Scene::getSceneBB(GLfloat& x0, GLfloat& x1, 
		       GLfloat& y0, GLfloat& y1, 
		       GLfloat& z0, GLfloat& z1 ) 
{
  x0 = _low[0];   x1 = _high[0];
  y0 = _low[1];   y1 = _high[1];
  z0 = _low[2];   z1 = _high[2];
}

//-----------------------------------------------------------------------------
