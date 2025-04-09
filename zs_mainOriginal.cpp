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
#include "zs_debug.hh"
#include "zs_basic.hh"
#include "zs_scene.hh"
#include "zs_light.hh"
#include "zs_viewpoint.hh"

#ifdef SMP_SGI
#ifdef FAKE_SMP_SGI
#include "zs_fakeSMP.hh"
#else
#include <ulocks.h>
#include <task.h>
#endif
#endif

extern Scene         *sceneptr;
extern ViewPlane     *pixptr;
extern Lighting      *lightptr;

// Parallel Version only
// When defined it makes the code insert into the heap any point
// that belongs to any cell that uses at least one point that lays
// inside the bucket region.
// Comment this constant and the code will only include into the heap
// (for each bucket) only the points laying inside the bucket region
// Beware that the test we conducted showed the higher the data set
// resolution, less error will be noticed if you comment this constant.
#define USE_NEIGHBOR_VERTICES

#define RESOLUTION        64

#ifdef SMP_SGI

// ** IMPORTANTE
// The number of screen chunks and processors must still be set manually
#define CHUNK                        8
#define NUMBER_OF_CHUNKS  (CHUNK*CHUNK)
#define NUM_PROCESSORS               8

typedef vector<unsigned> ListPoints;
typedef ListPoints *ListPointsP;

ListPoints listOfPoints[NUMBER_OF_CHUNKS*NUM_PROCESSORS];

float    chunkXResolution = (float)RESOLUTION/(float)CHUNK;
float    chunkYResolution = (float)RESOLUTION/(float)CHUNK;
float invChunkXResolution = 1/chunkXResolution;
float invChunkYResolution = 1/chunkYResolution;

void setChunkVariables( float xres, float yres ) 
{
  chunkXResolution = (float)xres/(float)CHUNK;
  chunkYResolution = (float)yres/(float)CHUNK;
  invChunkXResolution = 1/chunkXResolution;
  invChunkYResolution = 1/chunkYResolution;
}

void divide_work( Scene *sceneptr, ViewPlane *pixptr, Lighting *lightptr )
{
  //  sceneptr->render( pixptr, lightptr );

  int my_id            = m_get_myid();
  int n_procs          = m_get_numprocs();
  int starting_chunk   = NUMBER_OF_CHUNKS*my_id;

#if 0
  fprintf(stderr,"%d: n_procs = %d starting_chunk = %d\n",
	  my_id, n_procs, starting_chunk );
#endif

  // clear my chunks
  {
    int j;
    for(j = starting_chunk; j < (starting_chunk + NUMBER_OF_CHUNKS); j++)
      listOfPoints[j].clear();
  }

  vector<Point> &points_VEC = sceneptr->getPoint_VEC();

  int number_of_points = points_VEC.size()/n_procs;

  int start_point      = number_of_points * my_id;
  int end_point        = start_point + number_of_points;
  
  vector<Point>::iterator start_point_it = points_VEC.begin() + start_point;
  vector<Point>::iterator end_point_it = points_VEC.begin() + end_point;

  printf("%d: start_point_it = %d end_point_it = %d\n",
	 my_id, start_point, end_point-1 );

  if(my_id == n_procs-1)
    end_point_it = points_VEC.end();

  vector<Point>::iterator i;
  for(i = start_point_it; i != end_point_it; i++)
    {
      Point& p = *i;
      pixptr->RotatePoint( p );

      float x = p[0];
      float y = p[1];

      unsigned X = pixptr->w2sX( x );
      unsigned Y = pixptr->w2sY( y );

      int cx = (int)(X * invChunkXResolution);
      int cy = (int)(Y * invChunkYResolution);

      unsigned p_index = i - points_VEC.begin();
      //      printf("%d: p_index = %d\n",	     my_id, p_index);
      //      unsigned chunck_num = starting_chunk + cx + cy * CHUNK;
      listOfPoints[starting_chunk + cx + cy * CHUNK ].push_back( p_index );

#if 0
      printf("p %g %g %d %d bucket = %d\n", 
	     x, y,
	     X, Y,
	     cx + cy * CHUNK
	     );
#endif

    }


#if 0
  { // Debug purpose only
    int i;
    for( i = starting_chunk ; i < starting_chunk+NUMBER_OF_CHUNKS ; i++ ) 
      {
	
	if( listOfPoints[i].size() == 0 ) 
	  {
	    printf( "listOfPoints[%d] is empty\n", i );
	    continue;
	  }
	unsigned j;
	printf( "listOfPoints[%d] : ", i );
	for( j = 0 ; j<listOfPoints[i].size() ; j++ )
	  {
	    
	    printf( "%d , ", listOfPoints[i][j] );
	    
	    
	  }
	printf( "\n" );
      }
  }
#endif

  m_sync();

}


void bucket_render(int bucket, Scene *sceneptr, ViewPlane *pixptr,  Lighting *light )
{

  //int my_id            = m_get_myid();
  int n_procs          = m_get_numprocs();

  int X = bucket % CHUNK;
  int Y = bucket / CHUNK;

  vector<bool> point_marks_VEC (sceneptr->getPoint_VEC().size());

  unsigned range[4];

  // Range of the screen that will be rendered
  range[0] = (unsigned)( X    * chunkXResolution);
  range[1] = (unsigned)((X+1) * chunkXResolution - 1);
  range[2] = (unsigned)( Y    * chunkYResolution);
  range[3] = (unsigned)((Y+1) * chunkYResolution - 1);

  //printf("bucket[%d] %d %d\n", bucket, X, Y);
  //printf("  range X0=%d, X1=%d   Y0=%d, Y1=%d\n", 
  // range[0] , range[1] , range[2] , range[3]);

  // Counting the number of points to be sent to the heap
  int num_points = 0;
  int i;
  for(i = 0; i < n_procs; i++)
    {
      vector<unsigned>::iterator ui;
      
      unsigned  __chunk = i * NUMBER_OF_CHUNKS + bucket;
      for(ui  = listOfPoints[ __chunk ].begin();
	  ui != listOfPoints[ __chunk ].end();
	  ui++)
	{

	  Point *p = sceneptr->getPoint(*ui);

#ifdef USE_NEIGHBOR_VERTICES

// 	  printf("p %g %g %d %d\n", 
// 		 (*p)[0], (*p)[1],
// 		 pix->w2sX( (*p)[0] ),
// 		 pix->w2sY( (*p)[1] ));

	  unsigned numCell = p->getCellVectorSize();
	  unsigned j;
	  for( j = 0 ; j < numCell ; j++ ) {
	    unsigned cellIdx = p->getCell( j );
	    Cell& cell = sceneptr->getCellRef(cellIdx);
	    int k;
	    for(k = 0; k < cell.getNumVertices(); k++)
	      {
		int v = cell[k];
		if(point_marks_VEC[v] != true)
		  {
		    point_marks_VEC[v] = true;
		    num_points++;
		  }
	      }
	  }
#else

	  if( p->getCellVectorSize() > 0 ) 
	    {
	      int v = *ui;
	      if(point_marks_VEC[v] != true)
		{
		  point_marks_VEC[v] = true;
		  num_points++;
		}
	    }
#endif

	}
    }

  if( num_points == 0 ) return;

  vector<Point>&  tf_points_VEC = sceneptr->getPoint_VEC();
  vector<Cell >&  tf_cells_VEC  = sceneptr->getCell_VEC();

  //yfprintf(stderr,"%d: bucket = %d num_points = %d\n",  my_id, bucket, num_points);

  // Sending all marked points to the heap and setting the targetZ
  Heap local_heap(num_points);
  local_heap.resetHeapVertex();
  float targetZ = FLT_MAX;
  unsigned i1;
  for( i1 = 0 ; i1 < sceneptr->getPoint_VEC().size() ; i1++ )
    {
      if(point_marks_VEC[i1])
	{
	  Point* p = sceneptr->getPoint(i1);
	  local_heap.insertVertex( p );
	  float z = p->getZ();
	  if( z < targetZ ) 
	    targetZ = z;
	}
      point_marks_VEC[i1] = false;
    }
  // fprintf(stderr,"%d: bucket = %d num_points = %d\n",  my_id, bucket, num_points);

  // ----------------------------------------------
  // Starting the render itself
  // ----------------------------------------------
  unsigned xres, yres;
  pixptr->getDim( xres, yres );
  ViewPlane *pix = new ViewPlane(xres, yres);
  pix->ScaleTo( sceneptr->getSceneSize() );
  pix->Clear(range);
  pix->_scrList->resetImageRange( range );

  Hash local_hash(1000);
  {
    unsigned k=0;
    while( local_heap.vertexHeapNotEmpty() ) 
      {
	if( k%500 == 0 ) {
	  //fprintf(stderr,"%d: point = %d\n",  my_id, k);
	}
	k++;

	// _nextPoint( tf_points_VEC, tf_cells_VEC, vp, light );
	{
	  // Get next point from heap
	  Point *p = local_heap.getMinVertex();
	  point_marks_VEC[ p->getId() ] = true;
 	  unsigned numCell = p->getCellVectorSize();
// 	  if( numCell == 0 ) 
// 	    continue;

	  assert(p->getCellVectorSize() != 0);

// 	  fprintf(stderr,"%d: targetZ = %g p->getZ() = %g\n",  
// 		  my_id, targetZ, p->getZ());

	  if( p->getZ() < targetZ ) 
	    {
	      // Scan all Cell that touches the current point ...
	      // And send to the hash table all faces to which it belongs to.
	      for ( unsigned i = 0 ; i < numCell ; i++ ) 
		{ 
		  unsigned cellIdx = p->getCell( i );
		  Cell *cell = sceneptr->getCell(cellIdx);
		  
		  // Note that when the second point of a cell is touched, all 4 faces
		  // of a tetrahedron are projected. So a tetrahedral cell is considered
		  // swept if two or more of its vertices have been swept.
		  // A hexahedral cell needs that least 5 of its vertices have been
		  // swept to asure that all faces were projected
		  
		  sceneptr->_checkFacesToProject( p, cellIdx, cell, 
						  point_marks_VEC,
						  local_hash,
						  pix, light );
		}
	    }
	  else 
	    {
	      sceneptr->_composePixelsUptoZ( p->getZ(), pix, light, range );
	      
	      for( unsigned i = 0 ; i < numCell ; i++ ) 
		{
		  unsigned cellIdx = p->getCell( i );
		  Cell *cell = sceneptr->getCell(cellIdx);
		  
		  // Note that when the second point of a cell is touched, all 4 faces
		  // of a tetrahedron are projected. So a tetrahedral cell is considered
		  // swept if two or more of its vertices have been swept.

		  sceneptr->_checkFacesToProject( p, cellIdx, cell, 
						  point_marks_VEC,
						  local_hash,
						  pix, light );

		  // Get the vertex with maximum Z to update targetZ
		  float maxZ = cell->getMaxZ( tf_points_VEC );
		  if( maxZ >= targetZ ) 
		    targetZ = maxZ + _PERTURBATION;
		}
	    }
	  
	  sceneptr->_projectFaces( tf_points_VEC, tf_cells_VEC, pix, light, local_hash, range );
	}
      }
	
    sceneptr->_composeAllPixels( pix, light, range );
  }


//   printf("range %d %d %d %d\n", 
// 	 range[0] , range[1] , range[2] , range[3]);

  // XXX vai ter que copiar somewhere
  pixptr->copyPixels( pix, range );
  
  delete pix;

  //  printf("done with copying\n");
  

}

int available_bucket = 0;

void main_render( Scene *sceneptr, ViewPlane *pixptr,  Lighting *lightptr )
{
#if 0 
   if(m_get_myid() != 0)
     {
       m_sync();
       return;
     }
#endif

  while(1)
    {
      int my_next_bucket;
      m_lock();
      {
	my_next_bucket = available_bucket++;
      }
      m_unlock();
      
      //if( my_next_bucket == NUMBER_OF_CHUNKS-1 )
      //break;

      if( my_next_bucket >= NUMBER_OF_CHUNKS )
	break;

      fprintf(stderr,"%d: I will render my_next_bucket = %d\n",m_get_myid(),my_next_bucket);

      bucket_render(my_next_bucket, sceneptr, pixptr, lightptr );

    } 
  
  m_sync();

}

#endif



/*----------------------------------------------------------------------------
 * Actually perform integration of scalar field along the ray.
 ----------------------------------------------------------------------------*/
void zsmain(int argc, char **argv) 
{
  //Scene         *sceneptr;
  //ViewPlane     *pixptr;
  //Lighting      *lightptr;

  /* Default options */
  char     *inputname    = "/Users/csilva/src/demo-zsweep/input.off";
  // char     *inputname    = "demodata01.off";
  char     *basenameFile = "pix";
  char     *tfname       = "/Users/csilva/src/demo-zsweep/spx.tf";
  unsigned  xres   = RESOLUTION, yres = RESOLUTION;
  float     xang = 0.0, yang = 0.0, zang = 0.0;
  //  float     px0 = 0.0, py0 = 0.0 , pz0 = 0.0;
  //  float     px1 = 0.0, py1 = 0.0 , pz1 = 0.0;
  //  unsigned  nf=1;
  //int       needps        = 0;
  //char     *psfile        = "grid.ps";
  //int       needtiming    = 0;
  //char     *timingfile    = "timing.pgm";
  float     brightness    = 1.0;
  time_t Tstart,readtime;
/*
 int opt=1;
  while (opt) {
//	switch (getopt(argc, argv, "b:s:f:n:t:X:Y:Z:x:y:h:T:P:B:"))
	switch ( 4 )
	{
	case 'b':
	    basenameFile = optarg;
	    break;
	case 's':
	    if (sscanf(optarg,"%f:%f:%f", &px0, &py0, &pz0)!=3)
		goto usage;
	    break;
	case 'f':
	    if (sscanf(optarg,"%f:%f:%f", &px1, &py1, &pz1)!=3)
		goto usage;
	    break;
	case 'n':
	    if (sscanf(optarg,"%u", &nf)!=1)
		goto usage;
	    break;
	case 't': 
	    tfname = optarg;
	    break;
	case 'X':
	    if (sscanf(optarg,"%f", &xang)!=1)
		goto usage;
	    break;
	case 'Y':
	    if (sscanf(optarg,"%f", &yang)!=1)
		goto usage;
	    break;
	case 'Z':
	    if (sscanf(optarg,"%f", &zang)!=1)
		goto usage;
	    break;
	case 'x':
	    if (sscanf(optarg,"%u", &xres)!=1)
		goto usage;
	    break;
	case 'y':
	    if (sscanf(optarg,"%u", &yres)!=1)
		goto usage;
	    break;
	case 'T':
	  //needtiming = 1;
	  //if (optarg) timingfile=optarg;
	    break;
	case 'P':
 	  //needps = 1;
	  //if (optarg) psfile=optarg;
	    break;
	case 'B':
	    if (sscanf(optarg,"%f", &brightness)!=1)
		goto usage;
	    break;
	case EOF:
	    inputname = argv[optind];
	    opt=0;
	    break;
	case 'h':
	case '?':
	usage:
	fprintf(stderr,"Usage: %s [options] <file.off>\n", argv[0]);
	fprintf(stderr,"  Where [options] can be:\n" );
	fprintf(stderr,"     -t <transfer-file.tf>\n" );
	fprintf(stderr,"     -b <basename to save the images>\n" );
	fprintf(stderr,"     -x xres -y yres\n" );
	fprintf(stderr,"     -X angle -Y angle -Z angle\n" );
	fprintf(stderr,"     -s px0:py0:pz0\n" );
	fprintf(stderr,"     -f px1:py1:pz1\n" );
	fprintf(stderr,"     -n nframes\n" );
	fprintf(stderr,"     -T timing.pgm -P grid.ps file.scalar\n" );
	fprintf(stderr,"     -P grid.ps\n" );
	exit(1);
	}
  }
*/
  if (inputname==NULL) 
    throw ("Please supply an input filename");

  cout << endl;
  cout << "==================================================================" << endl;
  cout << "Input File = " << inputname << "\n";
  cout << "Transfer   = " << tfname << "\n";
  cout << "Rotation   = (" << xang << "," << yang << "," << zang << ")\n";
  cout << "Resolution = (" << xres << "," << yres << ")\n";
  cout << "BaseName   = " << basenameFile << "\n";

  Tstart = time(NULL);
  
  /* Read and scale scene */
  Scene *s = new Scene(inputname);
  sceneptr = s;
  float scenesize = s->getSceneSize();
  
  /* Create viewplane */
  ViewPlane *pix = new ViewPlane(xres, yres);
  pixptr = pix;
  pix->ScaleTo( scenesize );

  /* Create lighting tables */
  Lighting *light = new Lighting(tfname, brightness);
  lightptr = light;
  light->PrintTables();
  
  readtime = time(NULL) - Tstart;
  printf("Scene (Reading+Preprocessing) time: %lu sec\n\n", readtime);

  //--------------------------------------------------------------------------
  // Rendering First Image ---------------------------------------------------
  //--------------------------------------------------------------------------

  /*
  char outFileName[50];
  int count = 0;
  pixptr->UpdateViewRotMatrix( xang, yang, zang );
  yang = 1;
  sceneptr->render( pixptr, lightptr );
  sprintf( outFileName, "%s.%03d.ppm", inputname, (int)yang*count );
  cout << "Rendering : " << outFileName << endl;
  pixptr->SavePPM( outFileName );
  pixptr->UpdateViewRotMatrix( xang, yang, zang );
  */  

}
//----------------------------------------------------------------------------
