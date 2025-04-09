/* -*-C++-*-
*******************************************************************************
*
* File:         zs_render.h
* Description:  Functions in this module belong to class Scene
* 
* Author:       Claudio Silva / Ricardo Farias
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
#ifdef SMP_SGI

#include "zs_debug.hh" 
#include "zs_basic.hh"
#include "zs_scene.hh"
#include "zs_light.hh"
#include "zs_viewpoint.hh"

//----------------------------------------------------------------------------------
void Scene::_composeAllPixels( class ViewPlane *vp, class Lighting *light,
			       unsigned *range )  
{
  // Now lets sum up the pixel's color
  unsigned xDim, yDim;
  vp->getDim( xDim, yDim );
  unsigned x,y;
  bool  currIsBoundary, nextIsBoundary;
  float zCurr, zNext, vCurr, vNext;
  unsigned currCellNum, nextCellNum;

  for( x = range[0]; x <= range[1]; x++ ) {
    for( y = range[2] ; y <= range[3]; y++ ) {
      
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
}

/* Previous function
void Scene::_composePixelsUptoZ( float zMax, class ViewPlane *vp, 
				 class Lighting *light, unsigned *range ) 
{
  // Now lets sum up the pixel's color
  unsigned int lim[4];
  // vp->_scrList->getRange( lim );

  vp->_scrList->setRange( range );

  lim[0] = range[0];
  lim[1] = range[1];
  lim[2] = range[2];
  lim[3] = range[3];

  unsigned int y;
  unsigned int *p_currScrPtr;
  unsigned int *startScrTable = vp->_scrList->getScrTable();
  ScrUnitP  scrStatic     = vp->_scrList->getUnitTable();
  unsigned dimX = vp->_scrList->getDimX();

  for(y = lim[2] ; y <= lim[3] ; y++ ) 
    {
      p_currScrPtr = startScrTable + dimX*y + lim[0] ;

      unsigned int i, nextId;
      ScrUnitP int_currScrPtr;
      for(i = 0; i < lim[1]-lim[0]+1; i++)
	{
  
	  if( !(*p_currScrPtr) ) goto NothingToDo;

	  int_currScrPtr = &scrStatic[ *p_currScrPtr ];
	  
	  if( !( int_currScrPtr->next ) ) goto NothingToDo;
	  
	  bool  currIsBoundary, nextIsBoundary;
	  float zCurr, zNext, vCurr, vNext;
	  unsigned currCellNum, nextCellNum;

	  // vp->_scrList->getUnit( zCurr, vCurr, currCellNum, currIsBoundary );
	  {
	    zCurr              = int_currScrPtr->z;
	    vCurr              = int_currScrPtr->val;
	    currCellNum        = int_currScrPtr->cellIdx;
	    currIsBoundary     = int_currScrPtr->isBoundary;
	  }

	  light->SetCurrPixel( &(vp->Raster(i+lim[0], y)));
	  
	  nextId = int_currScrPtr->next;
	  while( !nextId )
	    {
	      int_currScrPtr     = &scrStatic[ nextId ];
	      // vp->_scrList->getUnit( zNext, vNext, nextCellNum, nextIsBoundary ) 
	      {
		zNext          = int_currScrPtr->z;
		vNext          = int_currScrPtr->val;
		nextCellNum    = int_currScrPtr->cellIdx;
		nextIsBoundary = int_currScrPtr->isBoundary;
		nextId         = int_currScrPtr->next;
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
	  p_currScrPtr++;

	}
    }

  vp->_scrList->cleanRangeUptoZ( zMax );

}*/

void Scene::_composePixelsUptoZ( float zMax, class ViewPlane *vp, 
				 class Lighting *light, unsigned *range  ) 
{

  // Now lets sum up the pixel's color
  unsigned int lim[4];
  vp->_scrList->setRange( range );
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

}

//----------------------------------------------------------------------------------
// Project Triangular faces
//----------------------------------------------------------------------------------
void Scene::_projectFace( Face& face, 
			  vector<Point> &tf_points_VEC, 
			  class ViewPlane *vp, 
			  class Lighting *light,
			  unsigned *range) 
{
//   printf("_projectFace: face %d %d %d\n", 
// 	 face.getPointIdx(0),face.getPointIdx(1),face.getPointIdx(2));

//   for(int xx = 0; xx < 3; xx++)
//     printf("%g %g %g\n", 
// 	   tf_points_VEC[face.getPointIdx(xx)][0], 
// 	   tf_points_VEC[face.getPointIdx(xx)][1],
// 	   tf_points_VEC[face.getPointIdx(xx)][2]);


//   for(xx = 0; xx < 3; xx++)
//     printf("%d %d\n", 
// 	   vp->w2sX(tf_points_VEC[face.getPointIdx(xx)][0]), 
// 	   vp->w2sY(tf_points_VEC[face.getPointIdx(xx)][1]));
  

  // The bounding box is :
  //  bb[0] == min X  :  bb[2] == min Y
  //  bb[1] == max X  :  bb[3] == max Y
  float bb[4];

  if( face.coplanar( tf_points_VEC ) ) return;
  if ( !face.updateCoeffs( tf_points_VEC ) ) return;
  face.getBB( tf_points_VEC, bb  );
  unsigned 
    xMin = vp->w2sX(bb[0]), 
    xMax = vp->w2sX(bb[1]), 
    yMin = vp->w2sY(bb[2]),
    yMax = vp->w2sY(bb[3]), 
    x, y;

  //   printf("projecting face %d %d %d %d\n", xMin, xMax, yMin, yMax);

  // clip to range
  if(xMin < range[0])
    xMin = range[0];
  if(xMax > range[1])
    xMax = range[1];
  if(yMin < range[2])
    yMin = range[2];
  if(yMax > range[3])
    yMax = range[3];

  if(xMin > xMax)
    return;

  if(yMin > yMax)
    return;

  //   printf("clipping to %d %d %d %d\n", xMin, xMax, yMin, yMax);
  
  for( x = xMin ; x <= xMax ; x++ ) 
    {
      float X = vp->s2wX(x);
      for( y = yMin ; y <= yMax ; y++ ) 
	{
	  float Y = vp->s2wY(y);
	  if( face.IsWithin( X , Y ) ) 
	    {
	      vp->_scrList->insertUnit( x, y, 
					face.GetZ(X,Y),     face.GetVal(X,Y),
					face.getCellIdx(),   face.getIsBoundary() );
	    }
	}
    }
}


//----------------------------------------------------------------------------------
void Scene::_projectFaces( vector<Point> &tf_points_VEC, 
			   vector<Cell> &tf_cells_VEC,
			   class ViewPlane *vp, 
			   class Lighting *light,
			   Hash& hash,
			   unsigned *range ) 
{
  hash.prepareForScan();

  for( unsigned hashSize = hash.getNumOfUnits() ; hashSize > 0 ; hashSize-- ) 
    {
      unsigned n0, n1, n2, cellIdx, isBoundary;;
      isBoundary = hash.getUnit( n0, n1, n2, cellIdx );
      Face face( n0, n1, n2, cellIdx, isBoundary );

      //       printf("_projectFaces: face %d %d %d\n", n0, n1, n2);
      
      _projectFace( face, tf_points_VEC, vp, light, range );
    }

  hash.resetHashTable();
}

//----------------------------------------------------------------------------------
void Scene::_checkFacesToProject( Point *p, 
				  unsigned cellIdx, 
				  Cell  *cell,
				  vector<bool> &is_point_transformed_VEC, 
				  Hash& hash,
				  class ViewPlane *vp,
				  class Lighting *light ) 
{
  // Get the faces who touch me ------------------------------------
  unsigned vert[32];
  unsigned neigFaces[32];
  unsigned myIdx = p->getId();
  cell->neighborFaces( myIdx, neigFaces );

  // Now we project all faces that has no other vertex swept before
  for( unsigned  i = 1 ; i < neigFaces[0]+1 ; i++ ) 
    {
      if( cell->onlyIWasSwept( myIdx, neigFaces[i], is_point_transformed_VEC) ) 
	{
	  cell->getFaceGlobalVerticesIndices( neigFaces[i], vert );
	  if( cell->getCellType() == TETRAHEDRON ) {
	    
	    assert( vert[0] == 3 );
	    
// 	    printf("inserting %d (%d) %d (%d) %d (%d) \n", 
// 		   vert[1],  is_point_transformed_VEC[ vert[1] ],
// 		   vert[2],  is_point_transformed_VEC[ vert[2] ],
// 		   vert[3],  is_point_transformed_VEC[ vert[3] ]);
	    
	    hash.insertUnit( vert[1], vert[2], vert[3], cellIdx );
	    
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
	    
	    hash.insertUnit( vert[n], vert[(n+1)%4], vert[(n+2)%4], cellIdx );
	    hash.insertUnit( vert[n], vert[(n+2)%4], vert[(n+3)%4], cellIdx );
	  }
	}
    }
  
}

#endif

// End of Module -------------------------------------------------------------------
