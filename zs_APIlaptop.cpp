#ifndef _ZS_API_HH
#define _ZS_API_HH

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
#include zs_API.hh

Scene         *sceneptr, *s[MAX_SCENES];
ViewPlane     *pixptr;
Lighting      *lightptr;

/* Default options */
unsigned  numScenes    = 0;
unsigned  currScene    = 0;
char     *inputname    = NULL;
char     *basenameFile = "pix";
char     *tfname       = "lighting.tf";
unsigned  xres   = RESOLUTION, yres = RESOLUTION;
float     xang = 0.0, yang = 0.0, zang = 0.0;
float     px0 = 0.0, py0 = 0.0 , pz0 = 0.0;
float     px1 = 0.0, py1 = 0.0 , pz1 = 0.0;
unsigned  nf=1;
float     brightness    = 1.0;

void zSweep_SetInputFile( char *name )    { inputname    = name; }
void zSweep_SetTransferFile( char *name ) { tfname       = name; }
void zSweep_SetBaseFile( char *name )     { basenameFile = name; }
void zSweep_SetNumFrames( unsigned n ) { nf = n; }
void zSweep_SetXand( float angle ) { xang = angle; }
void zSweep_SetYand( float angle ) { yang = angle; }
void zSweep_SetZand( float angle ) { zang = angle; }
void zSweep_SetXRes( float res ) { xres = res; }
void zSweep_SetYRes( float res ) { yres = res; }
void zSweep_SetBrightness( float b ) { brightness = b; }
void zSweep_SetP0s( float f0, float f1, float f2 ) 
{
  px0 = f0; py0 = f1; pz0 = f2;
}
void zSweep_SetP1s( float f0, float f1, float f2 ) 
{
  px1 = f0; py1 = f1; pz1 = f2;
}

void zSweep_Init( void ) 
{

  if( inputname == NULL ) 
    throw ("Please supply an input filename");

  cout << endl;
  cout << "==================================================================" << endl;
  if( inputname == NULL ) 
    cout << "Input File = " << inputname << "\n";
  else
    cout << "Input File = NULL\n";
  cout << "Transfer   = " << tfname << "\n";
  cout << "Rotation   = (" << xang << "," << yang << "," << zang << ")\n";
  cout << "Resolution = (" << xres << "," << yres << ")\n";
  cout << "BaseName   = " << basenameFile << "\n";

  Scene *s = new Scene();
  sceneptr = s;
 
}

void zSweep_SetNodes( int num_nodes, 
		      float *x_coords, 
		      float *y_coords, 
		      float *z_coords )
{

  _nPoints = num_nodes;
  _points_VEC.reserve( _nPoints );
  {
    unsigned i;
    for( i = 0 ; i < _nPoints ; i++ ) 
      {
	
	_points_VEC.push_back( Point( x_coords[i], y_coords[i], z_coords[i], 0, i ) );
	
      }
    MakeBBox( _points_VEC );
  }

}

void zSweep_SetNodeScalars( unsigned num_nodes, float *scalars )
{

  assert( _nPoints != num_nodes );
  {
    unsigned i;
    for( i = 0 ; i < _nPoints ; i++ ) 
      {
	Point& cp = _points_VEC[ i ] ;
	cp.setVal( scalars[i] );
      }
  }

}

void zSweep_SetTransferFunction( float r[256], 
                                 float g[256], 
                                 float b[256], 
                                 float alpha[256]
                               )
{
  unsigned int i;
  for (i=0; i<=256; i++)
    {
      _red[i]     = r[i];
      _green[i]   = g[i];
      _blue[i]    = b[i];
      _opacity[i] = alpha[i];
    }
}

#endif
