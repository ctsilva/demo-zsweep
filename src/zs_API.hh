#ifndef _ZS_API_HH
#define _ZS_API_HH

/* -*-C++-*-
*******************************************************************************
*
* File:         zs_API.hh
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
#include "zs_debug.hh"
#include "zs_basic.hh"
#include "zs_scene.hh"
#include "zs_light.hh"
#include "zs_viewpoint.hh"

#define MAX_SCENES    100
#define RESOLUTION    512
#define VISIBLE       1
#define NOT_VISIBLE   0

// Functions
void zSweep_setInputFile( char *name );
void zSweep_setTransferFile( char *name );
void zSweep_setBaseFile( char *name );
void zSweep_setRotX( float angle );
void zSweep_setRotY( float angle );
void zSweep_setRotZ( float angle );
void zSweep_setResX( unsigned int res );
void zSweep_setResY( unsigned int res );
void zSweep_setBrightness( float b );

struct rgb *returnImageArray( void );

// Auxiliary Functions ...........................................
int zSweep_checkInputFile( void );
void zSweep_displaySetup( void );
// Processing Functions .........................................
void zSweep_begin( void );
void zSweep_initBlockFromFile( void );
void zSweep_initBlockFromFile( char *name );
void zSweep_initBlock( void );
void zSweep_end( void );
void zSweep_updateViewMatrix( void );
void zSweep_updateViewMatrix( float *matrix );
void zSweep_savePPMImage( char *name );
void zSweep_savePPMImage( int n );
void zSweep_setCurrentBlock( int n );
void zSweep_setBlockViewable( int n, int status );
void zSweep_setNodes( unsigned int num_nodes, 
		      float *x_coords, 
		      float *y_coords, 
		      float *z_coords );
void zSweep_setNodeScalars( unsigned int num_nodes, float *scalars );
void zSweep_setDefaultTransferFunction( void );
void zSweep_setTransferFunction( float r[256], 
                                 float g[256], 
                                 float b[256]
				 );
void zSweep_setTransferFunction( float r[256], 
                                 float g[256], 
                                 float b[256], 
                                 float alpha[256]
				 );
void zSweep_setBlockConnectivity( unsigned int num_cells,
				  int cell_type, 
				  unsigned int *x );
void zSweep_doWork(void);
void zSweep_displayPixelsRendered( void );

// Not used yet ......................................
void zSweep_setP0s( float f0, float f1, float f2 );
void zSweep_setP1s( float f0, float f1, float f2 );

#endif
