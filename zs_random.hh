/*---------------------------------------------------------------------------
// Program  : Random(N)
// Goal     : Random number generator between 0 and N-1
// Date     : 01/20/97
// R.Farias : rfarias@ams.sunysb.edu
//-------------------------------------------------------------------------*/
/* Usage example:

   #include <stdio.h>
   #include "random.h"
   void main( void ) {

     randomize();  // Inicialize the random seed
     for( int i ; i < 100 ; i++ 0 {

       int k = Random( 100 );
       printf( "k = %5d\n", k );

     }

   }

 */
#include <time.h>
#include <stdlib.h>
#include <math.h>

#ifndef RAND_MAX
#define RAND_MAX (pow(2,31)-1)
#endif

#define Random(num)     (int)((num)*((float)rand()/((float)RAND_MAX+1.0)))
#define randomize()     srand((unsigned)time(NULL))

//-------------------------------------------------------------------------*/



