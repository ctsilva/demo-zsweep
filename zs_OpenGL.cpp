/* -*-C++-*-
*******************************************************************************
*
* File:         zs_OpenGL.cc
* Description:  
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Jan 28 2000
* Modified:     Thu Sep 06 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* (C) Copyright 2000
*
*******************************************************************************
*/
#include "zs_scene.hh"

void Scene::oglDisplayScene( unsigned flag = MARK_ANY ) 
{

//#ifdef OPENGL

  Point *p[32];
  unsigned i, j;
  switch( flag ) {

  case MARK_BOUNDARY :
    //case MARK_NOTVISIBLE_BOUNDARY :

    glColor4f(1.0, 1.0, 0.0, 1.0);

    for( i = 0 ; i < _nCells ; i++ ) {
      Cell& currCell = _cells_VEC[ i ];
      unsigned numVertices = currCell.getNumVertices();
      for( j = 0 ; j < numVertices ; j++ )
	p[j] = &_points_VEC[ currCell.point(j) ] ;
      
      if( currCell.getCellType() == TETRAHEDRON ) {
	
	if( !currCell.marked(MARK_FACE0_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glEnd();
  
	}
	if( !currCell.marked(MARK_FACE1_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
 
	}
	if( !currCell.marked(MARK_FACE2_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
  
	}
	if( !currCell.marked(MARK_FACE3_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	
      } else {
	
	if( !currCell.marked(MARK_FACE0_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE1_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE2_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE3_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE4_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE5_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glEnd();
	  
	}
	
      }
      
    }
    
    break;
    
  default:
    
    // Display the interios faces
    glColor4f(0.0, 0.6, 0.8,1.0);
    for( i = 0 ; i < _nCells ; i++ ) {
      Cell& currCell = _cells_VEC[ i ];
      unsigned numVertices = currCell.getNumVertices();
      for( j = 0 ; j < numVertices ; j++ )
	p[j] = &_points_VEC[ currCell.point(j) ] ;
      
      if( currCell.getCellType() == TETRAHEDRON ) {
	
	if( currCell.marked(MARK_FACE0_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glEnd();
	  
	}
	if( currCell.marked(MARK_FACE1_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	if( currCell.marked(MARK_FACE2_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	if( currCell.marked(MARK_FACE3_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	
      } else {
	
	if( !currCell.marked(MARK_FACE0_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE1_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE2_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE3_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE4_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE5_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glEnd();
	  
	}

      }
      
    }

    // Display the surface
    glColor4f(1.0, 1.0, 0.0,1.0);
    for( i = 0 ; i < _nCells ; i++ ) {
      Cell& currCell = _cells_VEC[ i ];
      unsigned numVertices = currCell.getNumVertices();
      for( j = 0 ; j < numVertices ; j++ )
	p[j] = &_points_VEC[ currCell.point(j) ] ;
      
      if( currCell.getCellType() == TETRAHEDRON ) {
	
	if( !currCell.marked(MARK_FACE0_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE1_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE2_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE3_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	
      } else {
	
	if( !currCell.marked(MARK_FACE0_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE1_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE2_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE3_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE4_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[0]->getX(), p[0]->getY(), p[0]->getZ() );
	  glVertex3f( p[3]->getX(), p[3]->getY(), p[3]->getZ() );
	  glVertex3f( p[7]->getX(), p[7]->getY(), p[7]->getZ() );
	  glVertex3f( p[4]->getX(), p[4]->getY(), p[4]->getZ() );
	  glEnd();
	  
	}
	if( !currCell.marked(MARK_FACE5_INTERNAL) ) {
	  
	  glBegin( GL_LINE_LOOP );
	  glVertex3f( p[1]->getX(), p[1]->getY(), p[1]->getZ() );
	  glVertex3f( p[2]->getX(), p[2]->getY(), p[2]->getZ() );
	  glVertex3f( p[6]->getX(), p[6]->getY(), p[6]->getZ() );
	  glVertex3f( p[5]->getX(), p[5]->getY(), p[5]->getZ() );
	  glEnd();
  
	}
	
      }
      
    }

    break;
  }

//#endif
}
/*
void Scene::displayVisEdges( vector<Point> &tf_points_VEC,
			     vector<Triangle> &tf_triangles_VEC ) 
{

//  #ifdef NO_WIRE_FRAME
//    return;
//  #endif

#ifdef OPENGL
#define tf_points_VEC _points_VEC
#define   vector<Triangle> &tf_triangles_VEC
    glColor3f(1.0, 1.0, 1.0);
  glBegin( GL_POLYGON );

    for ( unsigned t=0; t < _boundaryTriangles_VEC.size(); t++)
      {
	Triangle *tp = & tf_triangles_VEC[_boundaryTriangles_VEC[t]];
	if( tp->marked(MARK_BOUNDARY) ) {
	  Point& a = (Point &) tf_points_VEC[ tp->_pi[0] ];
	  Point& b = (Point &) tf_points_VEC[ tp->_pi[1] ];
	  Point& c = (Point &) tf_points_VEC[ tp->_pi[2] ];
	  glVertex3f( a[0], a[1], a[2] );
	  glVertex3f( b[0], b[1], b[2] );
	  glVertex3f( b[0], b[1], b[2] );
	  glVertex3f( c[0], c[1], c[2] );
	  glVertex3f( c[0], c[1], c[2] );
	  glVertex3f( a[0], a[1], a[2] );
	}
      }
    
  glEnd();
#endif
}
*/
//-----------------------------------------------------------------------------
