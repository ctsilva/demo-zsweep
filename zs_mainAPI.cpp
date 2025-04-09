/* -*-C++-*-
*******************************************************************************
*
* File:         zs_mainAPI.h
* Description:  main module to use the Z-Sweep API
* Author:       Ricardo Farias
* Created:      Thu 24 2000
* Modified:     Thu 24 2000
* Language:     C++
* Package:      ZSweep Volume Render
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
  
  int opt=1;
  while (opt) {
    switch( getopt(argc, argv, "b:t:X:Y:Z:x:y:h:B:") )
      {
      case 'b':
	zSweep_setBaseFile( optarg );
	break;
      case 't':
	zSweep_setTransferFile( optarg );
	break;
      case 'X':
	float xang;
	if (sscanf(optarg,"%f", &xang)!=1)
	  goto usage;
	zSweep_setRotX( xang );
	break;
      case 'Y':
	float yang;
	if (sscanf(optarg,"%f", &yang)!=1)
	  goto usage;
	zSweep_setRotY( yang );
	break;
      case 'Z':
	float zang;
	if (sscanf(optarg,"%f", &zang)!=1)
	  goto usage;
	zSweep_setRotZ( zang );
	break;
      case 'x':
	unsigned int xres;
	if (sscanf(optarg,"%u", &xres)!=1)
	  goto usage;
	zSweep_setResX( xres );
	break;
      case 'y':
	unsigned int yres;
	if (sscanf(optarg,"%u", &yres)!=1)
	  goto usage;
	zSweep_setResY( yres );
	break;
      case 'B':
	float brightness;
	if (sscanf(optarg,"%f", &brightness)!=1)
	  goto usage;
	zSweep_setBrightness( brightness );
	break;
      case EOF:
	zSweep_setInputFile( argv[optind] );
	opt=0;
	break;
      case 'h':
      case '?':
      usage:
      fprintf(stderr,"Usage: %s [options] <file.off>\n", argv[0]);
      fprintf(stderr,"  Where [options] can be:\n" );
      fprintf(stderr,"     -t <transfer-file.tf>\n" );
      fprintf(stderr,"     -x xres -y yres\n" );
      fprintf(stderr,"     -X angle -Y angle -Z angle\n" );
      fprintf(stderr,"     -B bightness\n" );
      fprintf(stderr,"     -b <basename to save the images>\n" );
      exit(1);
      }
  }

  if( !zSweep_checkInputFile() ) 
    throw ("Please supply an input filename");
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
  zSweep_setRotY( 1 ); // Set increment for the rotation == 1, around axis Y

  // Rotates the object, around Y axis, in 1 degree step.
  for( int count = 0 ; count <= 360 ; count++ ) 
    {
      
      double t_start = get_clock();
      
      zSweep_doWork();
      
      cout << "Time to generate frame " << count << ":" << get_clock()-t_start << endl;

      zSweep_savePPMImage( (int)count );

      zSweep_updateViewMatrix();
      
    }


}
//----------------------------------------------------------------------------
