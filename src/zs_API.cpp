/* -*-C++-*-
*******************************************************************************
*
* File:         zs_API.cc
* Description:  API to ZSweep code
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Jul 22 2000
* Modified:     Mon Agu 06 2000
* Language:     C++
* Package:      ZSweep
* Status:       Version 1.0
*
* (C) Copyright 2000, Ricardo Farias
*
*******************************************************************************
*/
#include "zs_API.hh"

Scene         *sceneptr;
ViewPlane     *pixptr;
Lighting      *lightptr;
Scene         *scene[MAX_SCENES];
ViewPlane     *pix[MAX_SCENES];
Lighting      *light[MAX_SCENES];

/* Default options */
unsigned  int numScenes    = 0;
unsigned  int currScene    = 0;
unsigned  int mcl          = 0; // Max Cell List to allocat hash
char      visScene[MAX_SCENES];
char     *inputname    = NULL;
char     *basenameFile = "pix";
char     *tfname       = "lighting.tf";
unsigned  xres   = RESOLUTION, yres = RESOLUTION;
float     xang = 0.0, yang = 0.0, zang = 0.0;
float     px0 = 0.0, py0 = 0.0 , pz0 = 0.0;
float     px1 = 0.0, py1 = 0.0 , pz1 = 0.0;
float     brightness    = 1.0;

// Setup Functions ...............................................
void zSweep_setInputFile( char *name )    { inputname    = name; }
void zSweep_setTransferFile( char *name ) { tfname       = name; }
void zSweep_setBaseFile( char *name )     { basenameFile = name; }
void zSweep_setRotX( float angle ) { xang = angle; }
void zSweep_setRotY( float angle ) { yang = angle; }
void zSweep_setRotZ( float angle ) { zang = angle; }
void zSweep_setResX( unsigned int res ) { xres = res; }
void zSweep_setResY( unsigned int res ) { yres = res; }
void zSweep_setBrightness( float b ) { brightness = b; }

struct rgb *returnImageArray( void ) 
{
  return pixptr->returnImageArray();
}

// Auxiliary Funcitons ...........................................
int zSweep_checkInputFile( void )
{
  return (inputname==NULL)? false:true;
}
void zSweep_displaySetup( void )
{
  cout << endl;
  cout << "============================================================" << endl;
  cout << "Input File = " << inputname << "\n";
  cout << "Transfer   = " << tfname << "\n";
  cout << "Rotation   = (" << xang << "," << yang << "," << zang << ")\n";
  cout << "Resolution = (" << xres << "," << yres << ")\n";
  cout << "BaseName   = " << basenameFile << "\n";
}

// Processing Functions .........................................
void zSweep_begin( void ) 
{
  numScenes = currScene = 0;
  for( unsigned i = 0 ; i < MAX_SCENES ; i++ )
    visScene[MAX_SCENES] = NOT_VISIBLE;

}

void zSweep_initBlockFromFile( void ) 
{

  if( numScenes >= MAX_SCENES ) {
    cout << "Block exceed number of blocks allowed." << endl;
    return;
  }
  cout << endl;
  cout << "=======================================================" << endl;
  if( inputname != NULL ) 
    cout << "Input File = " << inputname << "\n";
  else
    cout << "Input File = NULL\n";
  cout << "Initializing Object # " << numScenes << endl;
  cout << "Transfer   = " << tfname << "\n";
  cout << "Rotation   = (" << xang << "," << yang << "," << zang << ")\n";
  cout << "Resolution = (" << xres << "," << yres << ")\n";
  cout << "BaseName   = " << basenameFile << "\n";

  currScene = numScenes;
  numScenes++;

  Scene *s = new Scene(inputname);
  if( s == 0 )
    throw( "Could not create Object." );
  sceneptr = scene[ currScene ] = s;
  visScene[ currScene ] = VISIBLE;

}

void zSweep_initBlockFromFile( char *name ) 
{
  inputname    = name; 
  zSweep_initBlockFromFile();
}

void zSweep_initBlock( void ) 
{

  if( numScenes >= MAX_SCENES ) {
    cout << "Block exceed number of blocks allowed." << endl;
    return;
  }
  cout << endl;
  cout << "=======================================================" << endl;
  cout << "Initializing Object # " << numScenes << endl;
  cout << "Rotation   = (" << xang << "," << yang << "," << zang << ")\n";
  cout << "Resolution = (" << xres << "," << yres << ")\n";
  cout << "BaseName   = " << basenameFile << "\n";

  currScene = numScenes;
  numScenes++;

  Scene *s = new Scene();
  if( s == 0 )
    throw( "Could not create Scene." );
  sceneptr = scene[ currScene ] = s;
  visScene[ currScene ] = VISIBLE;

}
void zSweep_end( void ) 
{
  float scenesize = sceneptr->getSceneSize();  
  /* Create viewplane */
  ViewPlane *pix = new ViewPlane(xres, yres);
  pixptr = pix;
  pix->ScaleTo( scenesize );
  /* Create lighting tables */
  Lighting *light = new Lighting( tfname, 1.0 );
  lightptr = light;
  light->PrintTables();
}

void zSweep_updateViewMatrix( void ) 
{
  pixptr->UpdateViewRotMatrix( xang, yang, zang );
}
void zSweep_updateViewMatrix( float *matrix ) 
{
  pixptr->UpdateViewRotMatrix( matrix );
}
void zSweep_savePPMImage( char *name ) 
{
  cout << "Rendering : " << name << endl;
  pixptr->SavePPM( name );
}
void zSweep_savePPMImage( int n ) 
{
  char outFileName[50];
  sprintf( outFileName, "%s.%03d.ppm", 
	   (inputname!=NULL)? inputname:"OUTFILE", (int)n );
  zSweep_savePPMImage( outFileName );
}
void zSweep_setCurrentBlock( unsigned int n ) 
{

  if( n >= numScenes )
    throw( "Trying to activate inexistent object." );

  currScene = n;
  sceneptr = scene[ currScene ];
  visScene[ currScene ] = VISIBLE;

}

  // VISIBLE / NOT_VISIBLE
void zSweep_setBlockViewable( int n, int status = VISIBLE ) 
{

  visScene[ n ] = status;

}

void zSweep_setNodes( unsigned int num_nodes,
		      float *x_coords, 
		      float *y_coords, 
		      float *z_coords )
{

  sceneptr->setNumPoints( num_nodes );
  for( unsigned int i = 0 ; i < num_nodes ; i++ )  {
    sceneptr->pushPoint( x_coords[i],y_coords[i],z_coords[i], 0, i );
  }
  sceneptr->Center();

}

void zSweep_setNodeScalars( unsigned int num_nodes, float *scalars )
{

  vector<Point> &points_VEC = sceneptr->getPoint_VEC();
  for( unsigned int i = 0 ; i < num_nodes ; i++ ) {
    Point& cp = points_VEC[ i ] ;
    cp.setVal( scalars[i] );
  }
  sceneptr->ShiftValue();

}

void zSweep_setBlockConnectivity( unsigned int num_cells,
				  int cell_type, 
				  unsigned int *x )
{

  sceneptr->setNumCells( num_cells );
  for( unsigned int i = 0 ; i < num_cells ; i++ )  {

    sceneptr->pushCell( cell_type, x );
    switch( cell_type )
      {
      case TETRAHEDRON : 
	x += 4;
	break;
      case HEXAHEDRON :
	x += 8 ;
	break;
      default : throw ("ERROR reading cells in toff file");
      }
  }

  sceneptr->buildVertexLists();
  // Sum the number of Unit in the hash table.
  if( mcl == 0 ) {
    mcl = sceneptr->maxCellList();
    if( mcl > 0 ) sceneptr->allocHash( mcl );
  }
  sceneptr->compExt();

}

void zSweep_setDefaultTransferFunction( void )
{
  lightptr->readExpanded();
}
void zSweep_setTransferFunction( float r[256], 
                                 float g[256], 
                                 float b[256]
                               )
{
  for( unsigned int i = 0 ; i < 256 ; i++ )
    {
      lightptr->setColor( i, r[i], g[i], b[i] );
    }
  lightptr->zeroOpacity();
}
void zSweep_setTransferFunction( float r[256], 
                                 float g[256], 
                                 float b[256], 
                                 float alpha[256]
                               )
{
  for( unsigned int i = 0 ; i < 256 ; i++ )
    {
      lightptr->setColor( i, r[i], g[i], b[i], alpha[i] );
    }
}

void zSweep_doWork( void )
{
  for( unsigned int i = 0 ; i < numScenes ; i++ ) {
    if( visScene[i] == VISIBLE )
      scene[i]->render( pixptr, lightptr );
  }
}
void zSweep_displayPixelsRendered( void )
{
  cout << "Rendered Pixels = " << pixptr->numPixels() << endl;
}

// Not used yet ......................................
void zSweep_setP0s( float f0, float f1, float f2 ) 
{
  px0 = f0; py0 = f1; pz0 = f2;
}
void zSweep_setP1s( float f0, float f1, float f2 ) 
{
  px1 = f0; py1 = f1; pz1 = f2;
}
//--------------------------------------------------------------
