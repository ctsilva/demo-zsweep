/* -*-C++-*-
*******************************************************************************
*
* File:         zs_mainManual.h
* Description:  main module to use the Z-Sweep code
* Author:       Ricardo Farias
* Created:      Fri 27 2000
* Modified:     Fri 27 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* rfarias@ams.sunysb.edu
*
*******************************************************************************
*/
#include "zs_API.hh"

/*----------------------------------------------------------------------------
 * Actually perform integration of scalar field along the ray.
 ----------------------------------------------------------------------------*/
main(int argc, char **argv) 
{

  float x[]      = { 0, 2, 2  , 0, 0, 1.5, 2  , 0   };
  float y[]      = { 0, 0, 1.5, 2, 0, 0  , 2  , 2   };
  float z[]      = { 0, 0, 0  , 0, 2, 2  , 1.8, 1.9 };
  float scalar[] = { 0.5, 0.3, 0.4, 0.1, 0.7, 0.4, 0.9, 0.6 };
  unsigned int topolog[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

  time_t Tstart,readtime;

  Tstart = time(NULL);
  //-----------------------------------------
  zSweep_begin();

  /* Read and scale scene */
  zSweep_initBlock();
  zSweep_setNodes( (unsigned)8, &x[0], &y[0], &z[0] );
  zSweep_setNodeScalars( 8, &scalar[0] );
  zSweep_setBlockConnectivity( (unsigned)1, HEXAHEDRON, &topolog[0] );

  zSweep_end();
  //-----------------------------------------
  readtime = time(NULL) - Tstart;
  printf("Scene (Reading+Preprocessing) time: %lu sec\n\n", readtime);


  zSweep_updateViewMatrix();
      
  double t_start = get_clock();
  zSweep_doWork();

  cout << "Time to generate frame :" << get_clock()-t_start << endl;

  zSweep_savePPMImage( 0 );

}
//----------------------------------------------------------------------------
