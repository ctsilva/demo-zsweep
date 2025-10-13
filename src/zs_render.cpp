/* -*-C++-*-
*******************************************************************************
*
* File:         zs_render.h
* Description:  Functions in this module belong to class Scene
* Author:       Ricardo Farias
* Created:      Fri 28 2000
* Modified:     Fri 28 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* rfarias@ams.sunysb.edu
*
*******************************************************************************
*/
#include "zs_debug.hh" 
#include "zs_basic.hh"
#include "zs_scene.hh"
#include "zs_light.hh"
#include "zs_viewpoint.hh"


//static unsigned __max_        = 0;
static unsigned __totalFaces_ = 0;
//static unsigned __maxHash_    = 0;

//----------------------------------------------------------------------------------
void Scene::_composeAllPixels( class ViewPlane *vp, class Lighting *light ) {
  // Now lets sum up the pixel's color


  // ***************************************************
  //unsigned max, numListNoNull;
  //vp->_scrList->getScrListInfo( max, numListNoNull );
  //if( max>__max_ ) __max_ = max;
  // ***************************************************


  unsigned xDim, yDim;
  vp->getDim( xDim, yDim );
  unsigned x,y;
  bool  currIsBoundary, nextIsBoundary;
  float zCurr, zNext, vCurr, vNext;
  unsigned currCellNum, nextCellNum;
  for( x = 0 ; x < xDim ; x++ ) {
    for( y = 0 ; y < yDim ; y++ ) {

      if( ! ( vp->_scrList->prepareForScan( x, y ) ) ) continue;
      // Get first intersection
      vp->_scrList->getUnit( zCurr, vCurr, currCellNum, currIsBoundary );
      light->SetCurrPixel( &(vp->Raster(x,y)) );
      unsigned count = 0;
      
      while( vp->_scrList->getUnit( zNext, vNext, nextCellNum, nextIsBoundary ) ) {
	if( !( currIsBoundary && nextIsBoundary && (currCellNum != nextCellNum) ) ) {
	  if( ! light->Update( vCurr, vNext, zNext-zCurr ) )
	    break; // Pixel opacity reached max value 1.0
	}
  	count++;
  	zCurr = zNext;
  	vCurr = vNext;
	currCellNum    = nextCellNum;
	currIsBoundary = nextIsBoundary;
      }
    }
  }
  vp->_scrList->resetImageArray();
  _imageUpdated = true;
}


void Scene::_composePixelsUptoZ( float zMax, class ViewPlane *vp, class Lighting *light ) 
{

  // Now lets sum up the pixel's color
  unsigned int lim[4];
  vp->_scrList->getRange( lim );
  unsigned int y;

  ScrUnitP Pool = vp->_scrList->getUnitTable();
  unsigned int *imageArray = vp->_scrList->getScrTable();
  unsigned int  idxPixel, currPixel;
  unsigned dimX = vp->_scrList->getDimX();

  for(y = lim[2] ; y <= lim[3] ; y++ ) 
    {
      currPixel = dimX*y + lim[0];
      idxPixel = dimX*y + lim[0] ;

      unsigned int i;
      ScrUnitP h;
      for(i = 0; i < lim[1]-lim[0]+1; i++)
	{

	  idxPixel = imageArray[ currPixel ] ;

	  if( !idxPixel ) goto NothingToDo;

	  h = &Pool[ idxPixel ];

	  if( !(h->next) ) goto NothingToDo;
	  
	  bool  currIsBoundary, nextIsBoundary;
	  float zCurr, zNext, vCurr, vNext;
	  unsigned currCellNum, nextCellNum;

	  // vp->_scrList->getUnit( zCurr, vCurr, currCellNum, currIsBoundary );
	  {
	    zCurr          = h->z;
	    vCurr          = h->val;
	    currCellNum    = h->cellIdx;
	    currIsBoundary = h->isBoundary;
	  }

	  light->SetCurrPixel( &(vp->Raster(i+lim[0], y)));
	  
	  idxPixel = h->next;

 	  while( idxPixel )
	    {

	      h = &Pool[ idxPixel ];
	      // vp->_scrList->getUnit( zNext, vNext, nextCellNum, nextIsBoundary ) 
	      {
		zNext          = h->z;
		vNext          = h->val;
		nextCellNum    = h->cellIdx;
		nextIsBoundary = h->isBoundary;
		idxPixel       = h->next;
	      }

	      if( zNext > zMax ) 
		break;
	      
	      if(!(currIsBoundary && nextIsBoundary && (currCellNum != nextCellNum))) 
		{
		  if(!light->Update(vCurr, vNext, zNext-zCurr))
		    break; // Pixel opacity reached max value 1.0
		}
	      
	      zCurr = zNext;
	      vCurr = vNext;
	      currCellNum    = nextCellNum;
	      currIsBoundary = nextIsBoundary;
	      
	    }
	NothingToDo:
	  currPixel++;

	}
    }

  vp->_scrList->cleanRangeUptoZ( zMax );
  _imageUpdated = true;

}

//----------------------------------------------------------------------------------
// Project Triangular faces
//----------------------------------------------------------------------------------
void Scene::_projectFace( Face& face, 
			  vector<Point> &tf_points_VEC, 
			  class ViewPlane *vp, 
			  class Lighting *light ) {
  
  // The bounding box is :
  //  bb[0] == min X  :  bb[2] == min Y
  //  bb[1] == max X  :  bb[3] == max Y
  float bb[4];

  if( _renderPause ) return;

  __totalFaces_++;

  if( face.coplanar( tf_points_VEC ) ) return;
  if( !face.updateCoeffs( tf_points_VEC ) ) return;
  face.getBB( tf_points_VEC, bb  );
  unsigned 
    xMin = vp->w2sX(bb[0]), 
    xMax = vp->w2sX(bb[1]), 
    yMin = vp->w2sY(bb[2]),
    yMax = vp->w2sY(bb[3]), 
    x, y;
  
  for( x = xMin ; x <= xMax ; x++ ) {
    
    float X = vp->s2wX(x);

    for( y = yMin ; y <= yMax ; y++ ) {
      
      float Y = vp->s2wY(y);
      
      if( face.IsWithin( X , Y ) ) {

	vp->_scrList->insertUnit( x, y, 
				  face.GetZ(X,Y), 
				  face.GetVal(X,Y),
				  face.getCellIdx(), 
				  face.getIsBoundary() );
      }
    }
  }

  //__totalFaces_++;

}

//----------------------------------------------------------------------------------
void Scene::_projectFaces( vector<Point> &tf_points_VEC, 
			   vector<Cell> &tf_cells_VEC,
			   class ViewPlane *vp, 
			   class Lighting *light ) 
{
  
  _hash->prepareForScan();
  
  //if( _hash->getNumOfUnits() > __maxHash_ ) __maxHash_ = _hash->getNumOfUnits();
  //__totalFaces_ += _hash->getNumOfUnits();
  

  for( unsigned hashSize = _hash->getNumOfUnits() ; hashSize > 0 ; hashSize-- ) 
    {

      unsigned n0, n1, n2, cellIdx, isBoundary;;
      isBoundary = _hash->getUnit( n0, n1, n2, cellIdx );
      Face face( n0, n1, n2, cellIdx, isBoundary );
      _projectFace( face, tf_points_VEC, vp, light );
      
    }

  _hash->resetHashTable();
  /*{
    unsigned int max=0,noNull=0;
    vp->_scrList->getScrListInfo(max, noNull);
    cout << "Maximum screen list = " << max << " NoNull = " << noNull 
    << "    Total units = " << max*noNull << endl;
    }*/

}

//----------------------------------------------------------------------------------
void Scene::_checkFacesToProject( Point *p, 
				  unsigned cellIdx, 
				  Cell  *cell,
				  vector<Point> &tf_points_VEC, 
				  class ViewPlane *vp,
				  class Lighting *light ) {

  // Get the faces who touch me ------------------------------------
  unsigned vert[32];
  unsigned neigFaces[32];
  unsigned myIdx = p->getId();
  cell->neighborFaces( myIdx, neigFaces );

  GLfloat cne[4] = { 1.0f, 0.6f, 0.6f, 0.5f };
  GLfloat coe[4] = { 0.4f, 0.4f, 1.0f, 0.3f };
  GLfloat cnf[4] = { 1.0f, 1.0f, 0.0f, 0.5f };
  GLfloat cof[4] = { 0.0f, 1.0f, 1.0f, 0.3f };

  // Projecting faces already project
  if( _oldedgeDisp ) {

    glColor4f( coe[0], coe[1], coe[2], coe[3] );
    for( unsigned  i = 1 ; i < neigFaces[0]+1 ; i++ ) {
      
      if( !cell->onlyIWasSwept( myIdx, neigFaces[i], tf_points_VEC ) ) {

	cell->getFaceGlobalVerticesIndices( neigFaces[i], vert );

	if( cell->getCellType() == TETRAHEDRON ) {
	  
	  assert( vert[0] == 3 );
	  if( _hash->insertUnit( vert[1], vert[2], vert[3], cellIdx ) ) {

	    Point& p0 = tf_points_VEC[ vert[1] ];
	    Point& p1 = tf_points_VEC[ vert[2] ];
	    Point& p2 = tf_points_VEC[ vert[3] ];
	    float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	    float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	    float x3 = p2[0], y3 = p2[1], z3 = p2[2];

	    glBegin( GL_LINES );
	    glVertex3f( x1, y1, z1 );
	    glVertex3f( x2, y2, z2 );
	    glVertex3f( x2, y2, z2 );
	    glVertex3f( x3, y3, z3 );
	    glVertex3f( x3, y3, z3 );
	    glVertex3f( x1, y1, z1 );
	    glEnd();

	  }

	} else { // Hexahedron
	
	  // To ensure that all triangle will be uniquely represented
	  // we have to find the position of the current vertex inside
	  // the current face and then create the triangles beginning
	  // from this relative position
	  assert( vert[0] == 4 );
	  int k = 0, n = 0;
	  for( k = 0 ; k < 4 ; k++ ) {
	    vert[k] = vert[k+1];
	    if( myIdx == vert[k] ) n = k;
	  }
	  
	  // Debug purpose only
	  //_checkSquareFace( n, vert );
	  
	  if( _hash->insertUnit( vert[n], vert[(n+1)%4], vert[(n+2)%4], cellIdx ) &&
	      _hash->insertUnit( vert[n], vert[(n+2)%4], vert[(n+3)%4], cellIdx )
	      ) {
	    
	    {
	      Point& p0 = tf_points_VEC[ vert[ n     ] ];
	      Point& p1 = tf_points_VEC[ vert[(n+1)%4] ];
	      Point& p2 = tf_points_VEC[ vert[(n+2)%4] ];
	      float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	      float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	      float x3 = p2[0], y3 = p2[1], z3 = p2[2];
	      glBegin( GL_LINES );
	        glVertex3f( x1, y1, z1 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x3, y3, z3 );
	        glVertex3f( x3, y3, z3 );
	        glVertex3f( x1, y1, z1 );
	      glEnd();
	    }
	    {
	      Point& p0 = tf_points_VEC[ vert[ n     ] ];
	      Point& p1 = tf_points_VEC[ vert[(n+2)%4] ];
	      Point& p2 = tf_points_VEC[ vert[(n+3)%4] ];
	      float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	      float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	      float x3 = p2[0], y3 = p2[1], z3 = p2[2];
	      glBegin( GL_LINES );
	        glVertex3f( x1, y1, z1 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x3, y3, z3 );
	        glVertex3f( x3, y3, z3 );
	        glVertex3f( x1, y1, z1 );
	      glEnd();
	    }

	  }
	  
	}
	
      }
      
    }

  }

  // Projecting faces already project
  if( _oldfaceDisp ) {

    glColor4f( cof[0], cof[1], cof[2], cof[3] );
    for( unsigned  i = 1 ; i < neigFaces[0]+1 ; i++ ) {
      
      if( !cell->onlyIWasSwept( myIdx, neigFaces[i], tf_points_VEC ) ) {

	cell->getFaceGlobalVerticesIndices( neigFaces[i], vert );

	if( cell->getCellType() == TETRAHEDRON ) {
	  
	  assert( vert[0] == 3 );
	  if( _hash->insertUnit( vert[1], vert[2], vert[3], cellIdx ) ) {

	    Point& p0 = tf_points_VEC[ vert[1] ];
	    Point& p1 = tf_points_VEC[ vert[2] ];
	    Point& p2 = tf_points_VEC[ vert[3] ];
	    float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	    float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	    float x3 = p2[0], y3 = p2[1], z3 = p2[2];

	    glBegin( GL_TRIANGLES );
	    glVertex3f( x1, y1, z1 );
	    glVertex3f( x2, y2, z2 );
	    glVertex3f( x3, y3, z3 );
	    glEnd();

	  }

	} else { // Hexahedron
	
	  // To ensure that all triangle will be uniquely represented
	  // we have to find the position of the current vertex inside
	  // the current face and then create the triangles beginning
	  // from this relative position
	  assert( vert[0] == 4 );
	  int k = 0, n = 0;
	  for( k = 0 ; k < 4 ; k++ ) {
	    vert[k] = vert[k+1];
	    if( myIdx == vert[k] ) n = k;
	  }
	  
	  // Debug purpose only
	  //_checkSquareFace( n, vert );
	  
	  if( _hash->insertUnit( vert[n], vert[(n+1)%4], vert[(n+2)%4], cellIdx ) &&
	      _hash->insertUnit( vert[n], vert[(n+2)%4], vert[(n+3)%4], cellIdx )
	      ) {
	    
	    {
	      Point& p0 = tf_points_VEC[ vert[ n     ] ];
	      Point& p1 = tf_points_VEC[ vert[(n+1)%4] ];
	      Point& p2 = tf_points_VEC[ vert[(n+2)%4] ];
	      float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	      float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	      float x3 = p2[0], y3 = p2[1], z3 = p2[2];
	      glBegin(GL_TRIANGLES);
	        glVertex3f( x1, y1, z1 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x3, y3, z3 );
	      glEnd();
	    }
	    {
	      Point& p0 = tf_points_VEC[ vert[ n     ] ];
	      Point& p1 = tf_points_VEC[ vert[(n+2)%4] ];
	      Point& p2 = tf_points_VEC[ vert[(n+3)%4] ];
	      float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	      float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	      float x3 = p2[0], y3 = p2[1], z3 = p2[2];
	      glBegin(GL_TRIANGLES);
	        glVertex3f( x1, y1, z1 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x3, y3, z3 );
	      glEnd();
	    }

	  }
	  
	}
	
      }
      
    }

  }

  // Projecting edges from the faces not yet projected
  if( _newedgeDisp ) {

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor4f( cne[0], cne[1], cne[2], cne[3] );
    for( unsigned  i = 1 ; i < neigFaces[0]+1 ; i++ ) {
      
      if( cell->onlyIWasSwept( myIdx, neigFaces[i], tf_points_VEC ) ) {

	cell->getFaceGlobalVerticesIndices( neigFaces[i], vert );

	if( cell->getCellType() == TETRAHEDRON ) {
	  
	  assert( vert[0] == 3 );
	  if( _hash->insertUnit( vert[1], vert[2], vert[3], cellIdx ) ) {

	    Point& p0 = tf_points_VEC[ vert[1] ];
	    Point& p1 = tf_points_VEC[ vert[2] ];
	    Point& p2 = tf_points_VEC[ vert[3] ];
	    float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	    float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	    float x3 = p2[0], y3 = p2[1], z3 = p2[2];

	    glBegin( GL_TRIANGLES );
	    glVertex3f( x1, y1, z1 );
	    glVertex3f( x2, y2, z2 );
	    glVertex3f( x3, y3, z3 );
	    glEnd();

	  }

	} else { // Hexahedron
	
	  // To ensure that all triangle will be uniquely represented
	  // we have to find the position of the current vertex inside
	  // the current face and then create the triangles beginning
	  // from this relative position
	  assert( vert[0] == 4 );
	  int k = 0, n = 0;
	  for( k = 0 ; k < 4 ; k++ ) {
	    vert[k] = vert[k+1];
	    if( myIdx == vert[k] ) n = k;
	  }
	  
	  // Debug purpose only
	  //_checkSquareFace( n, vert );
	  
	  if( _hash->insertUnit( vert[n], vert[(n+1)%4], vert[(n+2)%4], cellIdx ) &&
	      _hash->insertUnit( vert[n], vert[(n+2)%4], vert[(n+3)%4], cellIdx )
	      ) {
	    
	    {
	      Point& p0 = tf_points_VEC[ vert[ n     ] ];
	      Point& p1 = tf_points_VEC[ vert[(n+1)%4] ];
	      Point& p2 = tf_points_VEC[ vert[(n+2)%4] ];
	      float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	      float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	      float x3 = p2[0], y3 = p2[1], z3 = p2[2];
	      glBegin(GL_TRIANGLES);
	        glVertex3f( x1, y1, z1 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x3, y3, z3 );
	      glEnd();
	    }
	    {
	      Point& p0 = tf_points_VEC[ vert[ n     ] ];
	      Point& p1 = tf_points_VEC[ vert[(n+2)%4] ];
	      Point& p2 = tf_points_VEC[ vert[(n+3)%4] ];
	      float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	      float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	      float x3 = p2[0], y3 = p2[1], z3 = p2[2];
	      glBegin(GL_TRIANGLES);
	        glVertex3f( x1, y1, z1 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x3, y3, z3 );
	      glEnd();
	    }

	  }
	  
	}
	
      }
      
    }

  }

  // Projecting faces not yet projected
  if( _newfaceDisp ) {

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f( cnf[0], cnf[1], cnf[2], cnf[3] );
    for( unsigned  i = 1 ; i < neigFaces[0]+1 ; i++ ) {
      
      if( cell->onlyIWasSwept( myIdx, neigFaces[i], tf_points_VEC ) ) {

	cell->getFaceGlobalVerticesIndices( neigFaces[i], vert );

	if( cell->getCellType() == TETRAHEDRON ) {
	  
	  assert( vert[0] == 3 );
	  if( _hash->insertUnit( vert[1], vert[2], vert[3], cellIdx ) ) {

	    Point& p0 = tf_points_VEC[ vert[1] ];
	    Point& p1 = tf_points_VEC[ vert[2] ];
	    Point& p2 = tf_points_VEC[ vert[3] ];
	    float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	    float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	    float x3 = p2[0], y3 = p2[1], z3 = p2[2];

	    glBegin( GL_TRIANGLES );
	    glVertex3f( x1, y1, z1 );
	    glVertex3f( x2, y2, z2 );
	    glVertex3f( x3, y3, z3 );
	    glEnd();

	  }

	} else { // Hexahedron
	
	  // To ensure that all triangle will be uniquely represented
	  // we have to find the position of the current vertex inside
	  // the current face and then create the triangles beginning
	  // from this relative position
	  assert( vert[0] == 4 );
	  int k = 0, n = 0;
	  for( k = 0 ; k < 4 ; k++ ) {
	    vert[k] = vert[k+1];
	    if( myIdx == vert[k] ) n = k;
	  }
	  
	  // Debug purpose only
	  //_checkSquareFace( n, vert );
	  
	  if( _hash->insertUnit( vert[n], vert[(n+1)%4], vert[(n+2)%4], cellIdx ) &&
	      _hash->insertUnit( vert[n], vert[(n+2)%4], vert[(n+3)%4], cellIdx )
	      ) {
	    
	    {
	      Point& p0 = tf_points_VEC[ vert[ n     ] ];
	      Point& p1 = tf_points_VEC[ vert[(n+1)%4] ];
	      Point& p2 = tf_points_VEC[ vert[(n+2)%4] ];
	      float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	      float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	      float x3 = p2[0], y3 = p2[1], z3 = p2[2];
	      glBegin(GL_TRIANGLES);
	        glVertex3f( x1, y1, z1 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x3, y3, z3 );
	      glEnd();
	    }
	    {
	      Point& p0 = tf_points_VEC[ vert[ n     ] ];
	      Point& p1 = tf_points_VEC[ vert[(n+2)%4] ];
	      Point& p2 = tf_points_VEC[ vert[(n+3)%4] ];
	      float x1 = p0[0], y1 = p0[1], z1 = p0[2];
	      float x2 = p1[0], y2 = p1[1], z2 = p1[2];
	      float x3 = p2[0], y3 = p2[1], z3 = p2[2];
	      glBegin(GL_TRIANGLES);
	        glVertex3f( x1, y1, z1 );
	        glVertex3f( x2, y2, z2 );
	        glVertex3f( x3, y3, z3 );
	      glEnd();
	    }

	  }
	  
	}
	
      }
      
    }

  }
  
}

/*----------------------------------------------------------------------------
 * Event Handler. Keeps the necessary data structures during the sweep.
 ----------------------------------------------------------------------------*/
void Scene::_nextPoint( vector<Point> &tf_points_VEC, 
			vector<Cell> &tf_cells_VEC,
			class ViewPlane *vp, 
			class Lighting *light ) {

  Point *p;

  if( !_renderPause ) {
    // Get next point from heap
    p = _heap.getMinVertex();
    p->mark( MARK_SWEPT );
  } else {
    // This command returns the vertex pointer without poping it out of the heap
    p = _heap.vertexHeapNotEmpty();
  }

  {

    if( _vertexDisp ) {
      
      vector<Point>::iterator start_point_it = _points_VEC.begin();
      vector<Point>::iterator   end_point_it = _points_VEC.end();

      glBegin(GL_POINTS);

      vector<Point>::iterator i;
      for(i = start_point_it; i != end_point_it; i++)
	{
	  Point& pp = *i;
	  float x = pp[0];
	  float y = pp[1];
	  float z = pp[2];
	  if( pp.marked( MARK_SWEPT ) ) {
	    if( _swept ) {
	      glPointSize( 4 );
	      glColor4f(1.0,  1.0, 1.0, 1.0);
	      glVertex3f( x, y,  z);
	    }
	  } else {
	    if( _nonswept ) {
	      glPointSize( 4 );
	      glColor4f(1.0, 1.0, 0.0, 1.0);
	      glVertex3f( x, y,  z);
	    }
	  }
	}
      //glPointSize( 4 );
      //glColor3f(1.0, 1.0, 1.0);
      //glVertex3f( p->getX(), p->getY(), p->getZ() );

      glEnd();

    }
  
  }

  glBegin(GL_POINTS);
  glPointSize( 6 );
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f( p->getX(), p->getY(), p->getZ() );
  glEnd();

  unsigned numCell = p->getCellVectorSize();
  if( numCell == 0 ) return;

  float currZ = p->getZ();

  _currZ = currZ;

  if( currZ >= _targetZ /*|| vp->_scrList->maxScrListLengthAchieved()*/ )
    {

      if( currZ >= _targetZ )
	_composePixelsUptoZ( _targetZ, vp, light );
      else
 	_composePixelsUptoZ( currZ, vp, light );
     
      for( unsigned i = 0 ; i < numCell ; i++ ) 
	{
	  unsigned cellIdx = p->getCell( i );
	  Cell *cell = &tf_cells_VEC[cellIdx];
	  // Note that when the second point of a cell is touched, all 4 faces
	  // of a tetrahedron are projected. So a tetrahedral cell is considered
	  // swept if two or more of its vertices have been swept.
	  //if( !cell->Swept() ) 
	    {
	      //cell->incTouch();
	      _checkFacesToProject( p, cellIdx, cell, tf_points_VEC, vp, light );
	      // Get the vertex with maximum Z to update targetZ
	      if( currZ >= _targetZ ) 
		{
		  float maxZ = cell->getMaxZ( tf_points_VEC );
		  if( maxZ > _targetZ ) _targetZ = maxZ + _PERTURBATION;
		}
	    }
	}
      
    }
  else
    {

      // Scan all Cell that touches the current point ...
      // And send to the hash table all faces to which it belongs to.
      for( unsigned i = 0 ; i < numCell ; i++ ) {
	
	unsigned cellIdx = p->getCell( i );
	Cell *cell = &tf_cells_VEC[cellIdx];
	// Note that when the second point of a cell is touched, all 4 faces
	// of a tetrahedron are projected. So a tetrahedral cell is considered
	// swept if two or more of its vertices have been swept.
	// A hexahedral cell needs that least 5 of its vertices have been
	// swept to asure that all faces were projected
	//if( !cell->Swept() )
	{
	  //cell->incTouch();
	  _checkFacesToProject( p, cellIdx, cell, tf_points_VEC, vp, light );
	}
	
      }
      
    }
  
  _projectFaces( tf_points_VEC, tf_cells_VEC, vp, light );

}

/*----------------------------------------------------------------------------
 * Event Handler. Keeps the necessary data structures during the sweep.
 ----------------------------------------------------------------------------*/
void Scene::render( vector<Point> &tf_points_VEC, 
		    vector<Cell> &tf_cells_VEC,
		    class ViewPlane *vp, 
		    class Lighting *light ) {

  vp->Clear(); // Clear image buffer
  vp->_scrList->resetImageArray();

  time_t raytime, TStart = time(NULL);

  unsigned k=0;
  while( _heap.vertexHeapNotEmpty() ) {
    if( k%10000 == 0 )
      cout << "# Point :" << k << endl;
    k++;
    _nextPoint( tf_points_VEC, tf_cells_VEC, vp, light );
  }
  _composeAllPixels( vp, light );

  raytime = time(NULL) - TStart; 
  cout << "Raytracing time: " << raytime << " sec\n";

  //cout << "Longest Screen List            : " << __max_     << endl;
  //cout << "Maximum Num of Faces p/vertex  : " << __maxHash_ << endl;
  cout << "Total Number of Faces          : " << __totalFaces_ << endl;
  //cout << "Average Num of Faces p/vertex  : " << 
  //  (float)__totalFaces_/(float)_nPoints << endl;

}

//---------------------------------------------------
// Set of Functions to allow INTeractive rendering
//---------------------------------------------------
void Scene::int_RenderInit( class ViewPlane *vp, 
			    float xang, float yang, float zang )
{
  _renderGoingOn = true;
  vp->UpdateViewRotMatrix( xang, yang, zang );
  initForRender( vp );
  vp->Clear(); // Clear image buffer
  vp->_scrList->resetImageArray();
}
bool Scene::int_isRenderingGoingOn( void )
{
  return _renderGoingOn;
}
void Scene::int_setRenderPause( bool onoff )
{
  _renderPause = onoff;
}
void Scene::int_switchVertexDisp( void )
{
  _vertexDisp = !_vertexDisp;
}
void Scene::int_switchEdgeDisp( int mode )
{
    if( mode == _edgeMode ) {
    	_edgeDisp = !_edgeDisp;
	} else {
      _edgeMode = mode;
      if( !_edgeDisp ) _edgeDisp = true;
	}
}
void Scene::int_switchNewfaceDisp( void )
{
  _newfaceDisp = !_newfaceDisp;
}
void Scene::int_switchNewedgeDisp( void )
{
  _newedgeDisp = !_newedgeDisp;
}
void Scene::int_switchOldfaceDisp( void )
{
  _oldfaceDisp = !_oldfaceDisp;
}
void Scene::int_switchOldedgeDisp( void )
{
  _oldedgeDisp = !_oldedgeDisp;
}
bool Scene::int_switchCentralize( void )
{
  _centralize = !_centralize;
  return _centralize;
}
void Scene::int_switchSweptVertex( void )
{
  _swept = !_swept;
}
void Scene::int_switchNonSweptVertex( void )
{
  _nonswept = !_nonswept;
}
float Scene::int_getAdjustX( void )
{
  return _adjustX;
}
float Scene::int_getAdjustY( void )
{
  return _adjustY;
}
float Scene::int_getCurrentZ( void )
{
  return _currZ;
}
void Scene::int_getTargetZ( GLfloat& zt ) { zt = _targetZ; }

bool Scene::int_newImage( void ) { return _imageUpdated; }
void Scene::int_setOldIMage( void ) { _imageUpdated = false; }

int Scene::int_RenderNextVertex( class ViewPlane *vp, class Lighting *light )
{
  if( _heap.vertexHeapNotEmpty() ) {
    if( _centralize ) {
      Point *p = _heap.vertexHeapNotEmpty();
      if( _currId != p->getId() ) { // Avoid re-centralization
	float cx = p->getX();
	float cy = p->getY();
	float cz = p->getZ();

	vector<Point>::iterator start_point_it = _points_VEC.begin();
	vector<Point>::iterator   end_point_it = _points_VEC.end();
	vector<Point>::iterator i;
	for(i = start_point_it; i != end_point_it; i++)
	  {
	    Point& pp = *i;
	    pp.set( pp[0]-cx, pp[1]-cy, pp[2]-cz, pp[3] );
	  }
	_currId = p->getId();
      }
    }
    if( _edgeDisp )
      oglDisplayScene( _edgeMode );
    _nextPoint( _points_VEC, _cells_VEC, vp, light );
    return true;
  }
  _renderGoingOn = false;
  _composeAllPixels( vp, light );
  return false;
}

// End of Module -------------------------------------------------------------------
