#ifndef _ZS_SCENE_H
#define _ZS_SCENE_H

/* -*-C++-*-
*******************************************************************************
*
* File:         zs_scene.hh
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
*******************************************************************************
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <deque>
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
#include "zs_cell.hh"
#include "zs_heap.hh"
#include "zs_viewpoint.hh"
#include "zs_hash.hh"
using namespace std;

// Auxiliary structure to find the click of the mesh
typedef struct _Neighbor {
  char n;          // Number of neighbors
  unsigned _ti[4]; // Indices for the neighbors
}Neighbor;

#define _GHOST  1
#define _NEEDED 2

#define _PERTURBATION 1e-7

class Scene
{
private:
  
  unsigned _nPoints;
  unsigned _nCells;
  unsigned _numInterior;
  unsigned _num1BFace, _num2BFace, _num3BFace, _num4BFace;
  unsigned _numBPoints; // Number of boundary points
  unsigned _numBTFaces; // Number of Tetra boundary faces
  unsigned _numBHFaces; // Number of Hexa  boundary faces

  float _xFactor, _xFactorI, _yC, _xC;
  float _xStep, _yStep;
  float _targetZ; // Controls the next Z for composite the screen lists

  Heap  _heap;
  Hash  *_hash;
  Neighbor *_neighbor;

  bool  _renderGoingOn, _renderPause, _imageUpdated;
  bool  _vertexDisp, _newfaceDisp, _oldfaceDisp, _edgeDisp, _swept, _nonswept;
  bool  _newedgeDisp, _oldedgeDisp, _centralize;
  float _currZ, _adjustX, _adjustY;
  unsigned _currId;
  int _edgeMode;

private:
  
  vector<Point>  _points_VEC;
  vector<Cell >  _cells_VEC;

  // Bounding box 
  Point _low;
  Point _high;
  float _sceneSize;

  void _getNextLine( char *buf, FILE *f );
  void _readToff( char *toffFile );
  
  // Internal Render Functions --------------------------------------------
  void _composePixelsUptoZ( float currZ, class ViewPlane *vp, class Lighting *light );
  void _composeAllPixels( class ViewPlane *vp, class Lighting *light );
  void _projectFace( Face& face, 
		     vector<Point> &tf_points_VEC, 
		     class ViewPlane *vp, 
		     class Lighting *light );
  void _projectFaces( vector<Point> &tf_points_VEC, 
		      vector<Cell> &tf_cells_VEC,
		      class ViewPlane *vp, 
		      class Lighting *light );
  void _checkFacesToProject( Point *p, 
			     unsigned cellIdx, 
			     Cell  *cell,
			     vector<Point> &tf_points_VEC, 
			     class ViewPlane *vp,
			     class Lighting *light );

#ifdef SMP_SGI
public:
  void _checkFacesToProject( Point *p, 
			     unsigned cellIdx, 
			     Cell  *cell,
			     vector<bool> &is_point_transformed_VEC, 
			     Hash& hash,
			     class ViewPlane *vp,
			     class Lighting *light ) ;

  void _composeAllPixels( class ViewPlane *vp, class Lighting *light,
			  unsigned *range )  ;

  void _composePixelsUptoZ( float currZ, class ViewPlane *vp, 
			    class Lighting *light, unsigned *range );

  void _projectFace( Face& face, 
		     vector<Point> &tf_points_VEC, 
		     class ViewPlane *vp, 
		     class Lighting *light,
		     unsigned *range) ;

  void _projectFaces( vector<Point> &tf_points_VEC, 
			   vector<Cell> &tf_cells_VEC,
			   class ViewPlane *vp, 
			   class Lighting *light,
			   Hash& hash,
			   unsigned *range ) ;
#endif

private:

  void _nextPoint( vector<Point> &tf_points_VEC, 
		   vector<Cell> &tf_cells_VEC,
		   class ViewPlane *vp, 
		   class Lighting *light );
  
  // Functions to find out external vertices ------------------------------
  void _compExt( void );
  void _buildVertexLists( void );
  void _compExtPoints( void );
  void _compExtFaces( void ); 
  int  _inPointCell( unsigned pId, Cell *c );
  unsigned _maxCellList( void );
  unsigned _sumCellList( void );

  void _saveSparseFile( char *filename );
  void _sumCellsNeighbors( void );
  int  _canCellBeGhost( unsigned ti );
  void _clickInteriorCells( deque<unsigned> &tf_cells_DEQ  );
  void _clickAllCellsNumbered( unsigned qtt, deque<unsigned> &tf_cells_DEQ );
  void _graphClick( void );

  // Debug functions ------------------------------------------------------
  void _checkSquareFace( int n, unsigned *vert );
  void _createCleanFile( void );
  void _listExternalPoints( void );
  void _projectCellListOfGivenPoint( void );


public:
  
  // Initialise scene from a toff file (wherever this format came from) 
  // Rotation angles can be specified.
  //
  Scene(void);
  Scene(char *toffFile);

  vector<Point>& getPoint_VEC() { return _points_VEC; }
  vector<Cell >& getCell_VEC() { return _cells_VEC; }
    
  Point *getPoint( int n ) {
    return &_points_VEC[n];
  }
  Cell *getCell( int n ) {
    return &_cells_VEC[n];
  }
  Cell& getCellRef( int n ) {
    return _cells_VEC[n];
  }

  inline void setNumPoints( unsigned int n ) { 
    _nPoints = n; 
    _points_VEC.reserve( _nPoints );
  }
  inline void pushPoint( float x, float y, float z, float scalar, unsigned id )
  {
    _points_VEC.push_back( Point( x, y, z, scalar, id ) );
  }

  inline void setNumCells( unsigned int n ) { 
    _nCells = n; 
    _cells_VEC.reserve( _nCells );
  }
  inline void pushCell( int type, unsigned int *x )
  {
    if( type == TETRAHEDRON ) {
      _cells_VEC.push_back( Cell( x[0],x[1],x[2],x[3] ) );
    } else {
      if( type == HEXAHEDRON ) {
	_cells_VEC.push_back( Cell( x[0],x[1],x[2],x[3],
				    x[4],x[5],x[6],x[7] ) );
      } else {
	throw ("ERROR reading cells in toff file");
      }
    }    
  }

  void compExt( void ) { _compExt(); }
  unsigned maxCellList( void ) { return _maxCellList(); }
  void allocHash( unsigned n ) {
  //  assert( mcl > 0 );
    _hash = new Hash( n*100 );
    assert( _hash != 0 );
  }

  //------------------------------------------------------------------------
  // Rendering Functions ---------------------------------------------------
  //------------------------------------------------------------------------
  void setupSweep( vector<Point> &tf_points_VEC ) {

    _heap.resetHeapVertex();

    /*
     * Insert the boundary points in the list 
     * first.
     */
    // ***debug
    unsigned ttotal = 0;
    // ***end
    _targetZ = tf_points_VEC[0].getZ();
    for( unsigned i = 0; i < tf_points_VEC.size(); i++ ) {
      
      tf_points_VEC[i].unmark( MARK_SWEPT );
      //if( tf_points_VEC[i].marked( MARK_BOUNDARY ) ) {
      if( tf_points_VEC[i].getCellVectorSize() > 0 ) {
	// ***debug
	ttotal++;
	// ***end
	Point *p = &tf_points_VEC[i];
	if( p->getCellVectorSize() > 0 ) {
	  _heap.insertVertex( p );
	  float z = p->getZ();
	  if( z < _targetZ ) _targetZ = z;
	}
      }
      //}
      
    }
    _targetZ = _targetZ - _PERTURBATION;
    // ***end
    // cout << "# points inserted into the heap = " << ttotal << endl;
    // ***end
    
  }

  void setupSweep( void ) { setupSweep( _points_VEC ); }
  void buildVertexLists( void ) { _buildVertexLists(); }

  //------------------------------------------------------------------------
  void initForRender( vector<Point> &tf_points_VEC, 
		      vector<Cell> &tf_cells_VEC, 
		      ViewPlane *vp ) {
    Rotate( tf_points_VEC, vp );

    MakeBBox( tf_points_VEC );
    _xFactor  = ABS( (float) vp->_xres / (_high[0]-_low[0]) );
    _xFactorI = ABS( ( (_high[0]-_low[0]) / ((float) vp->_xres) ) );
    if( (_high[0]-_low[0]) > (_high[1]-_low[1]) ) {
      _yStep = ABS(((_high[0]-_low[0])/(float) vp->_yres));
      _xStep = ABS(((_high[0]-_low[0])/(float) vp->_xres));
    }
    else {
      _yStep = ABS(((_high[1]-_low[1])/(float) vp->_yres));
      _xStep = ABS(((_high[1]-_low[1])/(float) vp->_xres));
    }
    _xC = _low[0];
    _yC = _low[1];
    setupSweep(); //This function sets the points as unmark(MARK_SWEPT)

    for( unsigned i = 0 ; i < tf_cells_VEC.size() ; i++ ) {

	tf_cells_VEC[i].unmark( MARK_SWEPT );
	tf_cells_VEC[i].resetTouch();

    }

  }

  inline void initForRender( ViewPlane *vp ) {
    initForRender( _points_VEC, _cells_VEC, vp );  
  }

  void render( vector<Point> &tf_points_VEC, vector<Cell> &tf_cells_VEC,
	       class ViewPlane *vp, class Lighting *light );


  void render( class ViewPlane *vp, class Lighting *light ) { 
    // time_t raytime, TStart = time(NULL);

    double t_start = get_clock();

    initForRender( vp );
    render(  _points_VEC, _cells_VEC, vp, light );
    // raytime = time(NULL) - TStart; 
    
    cout << "Raytracing + PreProcessing time: " << get_clock() - t_start << " sec\n";
  }


  //------------------------------------------------------------------------
  // Build a copy of the point array 
  //------------------------------------------------------------------------
  void replicatePoints(vector<Point> &tf_points_VEC)
    {
#ifdef OUTPUT
      cout << "Number of points in the Scene " << _points_VEC.size() << "\n";
#endif
      tf_points_VEC.clear();
      tf_points_VEC.reserve(_points_VEC.size());
      //printf("_points_VEC.size() = %d\n", _points_VEC.size());
      unsigned i;
      for(i = 0; i < _points_VEC.size(); i++)
	tf_points_VEC.push_back(_points_VEC[i]);
    }

  void replicateCells(vector<Cell> &tf_cells_VEC)
    {
#ifdef OUTPUT
      cout << "Number of Cells in the Scene " << _cells_VEC.size() << "\n";
#endif
      tf_cells_VEC.clear();
      tf_cells_VEC.reserve(_cells_VEC.size());
      unsigned i;
      for(i = 0; i < _cells_VEC.size(); i++)
	tf_cells_VEC.push_back(_cells_VEC[i]);
    }
  void replicateScene( vector<Point> &tf_points_VEC,
		       vector<Cell>  &tf_cells_VEC )
    {
      replicatePoints( tf_points_VEC );
      replicateCells( tf_cells_VEC );
    }

  /* Build a bounding box */
  inline void MakeBBox( void ) { MakeBBox( _points_VEC ); }
  void MakeBBox( vector<Point> &tf_points_VEC );
  inline Point &BBoxLow()  { return _low;  }
  inline Point &BBoxHigh() { return _high; }
  
  /* Rotate the scene around X,Y,Z by the specified amount */
  void Rotate( vector<Point> &tf_points_VEC, ViewPlane *vp );
  void Rotate( ViewPlane *vp ) { 
    Rotate( _points_VEC, vp );
  }
  
  /* Center the scene around (0,0,0) */
  void  Center();
  
  /* Scale values to [0.0..255.0] */
  void ShiftValue();
  
  float getSceneSize( void ) { return _sceneSize; }

  /* Returns the bounding box of the scene */
  void getSceneBB( GLfloat& x0, GLfloat& x1, GLfloat& y0, GLfloat& y1, 
                   GLfloat& z0, GLfloat& z1 );
  float getMinX( void ) { return _low[0]; }

  // OpenGL Functions
  void oglDisplayScene( unsigned flag );

  void  int_RenderInit( class ViewPlane *vp, 
			float xang=0, float yang=0, float zang=0 );
  bool  int_isRenderingGoingOn( void );
  void  int_setRenderPause( bool onoff );
  float int_getAdjustX( void );
  float int_getAdjustY( void );
  float int_getCurrentZ( void );
  int   int_RenderNextVertex( class ViewPlane *vp, class Lighting *light );
  void  int_switchVertexDisp(  void );
  void  int_switchEdgeDisp( int mode );
  void  int_switchNewfaceDisp( void );
  void  int_switchOldfaceDisp( void );
  bool  int_switchCentralize( void );
  void  int_switchOldedgeDisp( void );
  void  int_switchNewedgeDisp( void );
  void  int_switchSweptVertex( void );
  void  int_switchNonSweptVertex( void );
  void  int_getTargetZ( GLfloat& zt );
  bool  int_newImage( void );
  void  int_setOldIMage( void );

};
#endif
