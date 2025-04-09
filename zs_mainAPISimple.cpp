/* -*-C++-*-
*******************************************************************************
*
* File:         zs_main.h
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

  time_t Tstart,readtime;

  zSweep_setTransferFile( "complex5.off.tf" );
  zSweep_setInputFile( "complex5.off" );

  zSweep_displaySetup();

  Tstart = time(NULL);
  //-----------------------------------------
  zSweep_begin();

  /* Read and scale scene */
  zSweep_initBlockFromFile();

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
