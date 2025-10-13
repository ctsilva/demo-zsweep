// zs_screenLists.cpp --------------------------------------------------------
// By Ricardo Farias : rfarias@ams.sunysb.edu
// Date : Jan 30 2000
// 
// Create an array of pointers for a specified structure
// ---------------------------------------------------------------------------
#include "zs_screenLists.hh"
#include "zs_general.hh"

//#define RICBUG
//A-----------------------------------------------------------------------
ScreenList::ScreenList( unsigned x, unsigned y ) {
  
  unsigned i;
  _imageArray = new unsigned int [x*y];
  if( _imageArray == NULL )
    throw( "Could not allocate memory for _imageArray." );
  _imageLast = new unsigned int [x*y];
  if( _imageLast == NULL ) 
    throw( "Could not allocate memory for _imageLast." );
  for( i = 0 ; i < x*y ; i++ ) {
    _imageArray[i] = _imageLast[i] = 0;
  }

  _Pool     = NULL;
  _chunk    = 0;
  _idxPool  = _idxPixel = 0;
  _allocPool();
#ifdef OUTPUT
  cout << "Chunk size = " << _chunk << "  Size = " 
       << _chunk*sizeof( ScrUnit ) << endl;
#endif

  dimX = x; dimY = y;
  rangeX[0] = dimX+1;
  rangeY[0] = dimY+1;
  rangeX[1] = rangeY[1] = 0;
  
}

//A-----------------------------------------------------------------------
void ScreenList::_allocPool( void ) {

  unsigned i, new_chunk;

  if( _chunk == 0 ) new_chunk = 50000;
  else new_chunk = 2*_chunk;

  //cout << ">> Chunk = " << new_chunk << "  Size = " 
  //     << new_chunk*sizeof( ScrUnit ) << endl;

  _Pool = (ScrUnitP)realloc( _Pool, new_chunk*sizeof(ScrUnit) );
  if( _Pool == NULL ) 
    {
      cout << "Error allocating memory for _Pool of scrUnits." << endl;
      exit(1);
    }
  
  for( i = _chunk ; i < new_chunk ; i++ ) 
    {
      _Pool[i].id   = i;
      _Pool[i].next = i+1;
    }
  _Pool[new_chunk-1].next = 0;

  // Reminder: Element zero is not used, since _idxPool == 0 means
  // pool empty
  _idxPool = ( _chunk == 0 )? 1 : _chunk ;
  _chunk   = new_chunk;
  
}

//A-----------------------------------------------------------------------
inline void ScreenList::_freeUnit( ScrUnit *h ) {
  if( _idxPool )
    h->next = _Pool[_idxPool].id;
  else {
    h->next = h->prev = 0;
  }
  _idxPool = h->id;
}

//A-----------------------------------------------------------------------
inline void ScreenList::_getUnit( void ) 
{
  if( _idxPool  == 0 ) _allocPool();
  _currUnit = &_Pool[ _idxPool ];
  assert(_currUnit != NULL);
  _idxPool = _currUnit->next;
}

//A-----------------------------------------------------------------------
void ScreenList::resetPixelList( unsigned k ) {
  if( !_imageArray[ k ] ) return;
  ScrUnit *h = &_Pool[ _imageArray[ k ] ];
  while( true ) {
    unsigned int next = h->next;
    _freeUnit( h );
    if( !next ) break;
    h = &_Pool[ next ];
  }
  _imageLast[ k ] = _imageArray[ k ] = 0;
}

//A-----------------------------------------------------------------------
void ScreenList::resetImageArray( void ) 
{
  unsigned linear;
  for( linear = 0 ; linear < dimX*dimY ; linear++ )
    resetPixelList( linear );
  rangeX[0] = dimX+1;
  rangeY[0] = dimY+1;
  rangeX[1] = rangeY[1] = 0;
}

//A-----------------------------------------------------------------------
void ScreenList::resetImageRange( unsigned int *r ) {
  unsigned int x, y,  _rangeX[2], _rangeY[2];
  _rangeX[0] = *(r);   _rangeY[0] = *(r+2);
  _rangeX[1] = *(r+1); _rangeY[1] = *(r+3);
  assert( _rangeX[0] <= _rangeX[1] );
  unsigned idxG = _rangeY[0]*dimX; // GlobalIndex
  for( y = _rangeY[0] ; y <= _rangeY[1] ; y++ ) {
    unsigned idx = idxG + _rangeX[0];
    for( x = _rangeX[0] ; x <= _rangeX[1] ; x++ ) {
      resetPixelList( idx );
      idx++;
    }
    idxG += dimX;
  }
}

//A-----------------------------------------------------------------------
inline void ScreenList::freePixelFirstUnit( unsigned X, unsigned Y )
{
  unsigned k = _elem(X,Y);
  if( !_imageArray[ k ] ) return;
  ScrUnit *h = &_Pool[ _imageArray[ k ] ];
  unsigned int next = h->next;
  _freeUnit( h );
  if( !next ) {
    _imageLast[ k ] = _imageArray[ k ] = 0;
  } else _imageLast[ k ] = next;

}

//A-----------------------------------------------------------------------
void ScreenList::cleanPixelUptoZ( float zMax, unsigned k ) {
  if( !_imageArray[ k ] ) return;
  ScrUnit *h = &_Pool[ _imageArray[ k ] ];
  while( h->next && _Pool[h->next].z <= zMax ) {
    unsigned int next = h->next;
    _freeUnit( h );
    if( !next ) break;
    h = &_Pool[ next ];
  }
  assert( h != NULL );
  h->prev = 0;
  _imageArray[ k ] = h->id;
}

//A-----------------------------------------------------------------------
void ScreenList::cleanPixelUptoZ( float zMax, unsigned X, unsigned Y ) {
  cleanPixelUptoZ( zMax, _elem(X,Y) );
}

//A-----------------------------------------------------------------------
void ScreenList::cleanRangeUptoZ( float zMax ) {
  unsigned int x,y;
  if( rangeX[0] == dimX+1 ) return;
  unsigned idxG = rangeY[0]*dimX; // GlobalIndex
  for( y = rangeY[0] ; y <= rangeY[1] ; y++ ) {
    unsigned idx = idxG + rangeX[0];
    for( x = rangeX[0] ; x <= rangeX[1] ; x++ ) {
      cleanPixelUptoZ( zMax, idx );
      idx++;
    }
    idxG += dimX;
  }
  rangeX[0] = dimX+1; 
  rangeY[0] = dimY+1;
  rangeX[1] = rangeY[1] = 0;
}

//A-----------------------------------------------------------------------
void ScreenList::setRange( unsigned int *r ) 
{
  rangeX[0] = *(r);   rangeY[0] = *(r+2);
  rangeX[1] = *(r+1); rangeY[1] = *(r+3);
}
void ScreenList::getRange( unsigned int *r ) 
{
  *(r)   = rangeX[0]; *(r+2) = rangeY[0];
  *(r+1) = rangeX[1]; *(r+3) = rangeY[1];
}
bool ScreenList::noRange( void ) { return (rangeX[0]==dimX+1)? true:false; }


//A========================================================================
void ScreenList::insertUnit( unsigned X, unsigned Y,
			     float z, float val, 
			     unsigned cellIdx, bool isBoundary ) 
{
  _currElem = _elem(X,Y);

  if( X < rangeX[0] ) rangeX[0] = X;
  if( X > rangeX[1] ) rangeX[1] = X;
  if( Y < rangeY[0] ) rangeY[0] = Y;
  if( Y > rangeY[1] ) rangeY[1] = Y;

  // Get next Unit
  _getUnit();
  _currUnit->z    = z;
  _currUnit->val  = val;
  _currUnit->cellIdx    = cellIdx;
  _currUnit->isBoundary = isBoundary;

  // Current Pixel List Empty
  if( _imageArray[ _currElem ] == 0 ) {
    assert( _imageLast[_currElem] == 0 );
    _currUnit->prev = _currUnit->next = 0;
    _imageLast[ _currElem ] = _imageArray[ _currElem ] = _currUnit->id;
    return;
  }

  // Is unit after the last unit in list?
  ScrUnit *lastScrPtr = &_Pool[ _imageLast[_currElem] ];
  if( z >= lastScrPtr->z ) {
    _currUnit->next       = 0;
    lastScrPtr->next      = _currUnit->id;
    _currUnit->prev       = _imageLast[_currElem];
    _imageLast[_currElem] = _currUnit->id;
    return;
  }

  // Is Unit before the first unit in list?
  ScrUnit *firstScrPtr = &_Pool[ _imageArray[ _currElem ] ];
  if( z <= firstScrPtr->z ) {
    _currUnit->next   = firstScrPtr->id;
    _currUnit->prev   = 0;
    firstScrPtr->prev = _currUnit->id;
    _imageArray[ _currElem ] = _currUnit->id;
    return;
  }

  // Lets look for its correct place
  ScrUnit *currScrPtr = lastScrPtr;
  while( currScrPtr->prev ) 
    {
      if( z >= _Pool[ currScrPtr->prev ].z ) 
	{
	  ScrUnit *prevScrPtr = &_Pool[ currScrPtr->prev ];
	  _currUnit->prev = prevScrPtr->id;
	  _currUnit->next = currScrPtr->id;
	  prevScrPtr->next = currScrPtr->prev = _currUnit->id;
	  return;
	}
      currScrPtr = &_Pool[ currScrPtr->prev ];
    }

  /*
  //  _currUnit->next = NULL;
  //  currScrPtr->next = _currUnit;
  cout << "The program should not get here!!!!!!!!!!!!" << endl;
  cout << "---------------------------------------------------------------\n";
  cout << "(" << X << "," << Y << ")   k = " << k << endl;
  cout << "z = " << z << "    val = " << val << endl;
  cout << "scrTable[ k ] = " << scrTable[ k ] << "    last[k] = " << last[k] << endl;
  {
    currScrPtr = &scrStatic[ last[k] ];

    while( currScrPtr->prev != -1 ) {
      
      cout << "   currScrPtr->id = " << currScrPtr->id << endl;
      float prevZ = scrStatic[ currScrPtr->prev ].z;
      
      cout << "    " << "prev = " << currScrPtr->prev;
      cout <<         "     next = " << currScrPtr->next << endl;
      cout << "    " << "prevZ = " << prevZ << "   prevVal = " << currScrPtr->val << endl;

      currScrPtr = &scrStatic[ currScrPtr->prev ];

    }
    cout << "   currScrPtr->id = " << currScrPtr->id << endl;
    cout << "    " << "prev = " << currScrPtr->prev;
    cout <<         "     next = " << currScrPtr->next << endl;
  }
  cout << "...............................................................\n";
  cout << "(" << X << "," << Y << ")   k = " << k << endl;
  cout << "z = " << z << "    val = " << val << endl;
  cout << "scrTable[ k ] = " << scrTable[ k ] << "    last[k] = " << last[k] << endl;
  {
    currScrPtr = &scrStatic[ scrTable[ k ] ];

    while( currScrPtr->next != -1 ) {
      
      cout << "   currScrPtr->id = " << currScrPtr->id << endl;
      float prevZ = scrStatic[ currScrPtr->prev ].z;
      
      cout << "    " << "prev = " << currScrPtr->prev;
      cout <<         "     next = " << currScrPtr->next << endl;
      cout << "    " << "prevZ = " << prevZ << "   prevVal = " << currScrPtr->val << endl;

      currScrPtr = &scrStatic[ currScrPtr->next ];

    }
    cout << "   currScrPtr->id = " << currScrPtr->id << endl;
    cout << "    " << "prev = " << currScrPtr->prev;
    cout <<         "     next = " << currScrPtr->next << endl;
  }
  cout << "===============================================================\n";
  exit(1);
  */

}

/* //How to make the loop to scan a pixel list
   if( !s.prepareForScan() ) return;
   do {
     do {
       s.getUnit( z, val, cellIdx, isBoundary );
     } while( s.incScanX() );
   } while( s.incScanY() );
*/
//A-----------------------------------------------------------------------
bool ScreenList::prepareForScan( void ) {

  _x = rangeX[0];
  _y = rangeY[0];
  _currElem = _elem( _x, _y );
  _idxPixel = _imageArray[ _currElem ] ;
  return ( !_idxPixel || !_Pool[_idxPixel].next )? false : true;

}
//A-----------------------------------------------------------------------
bool ScreenList::incScanX( void ) {

  _x++;
  if( _x == rangeX[1] ) return false;
  _currElem++;
  _idxPixel = _imageArray[ _currElem ];
  return true;

}
//A-----------------------------------------------------------------------
bool ScreenList::incScanY( void ) {

  _y++;
  if( _y == rangeY[1] ) return false;
  _x = rangeX[0];
  _currElem = _elem( _x, _y );
  _idxPixel = _imageArray[ _currElem ] ;
  return true;

}
//A-----------------------------------------------------------------------
bool ScreenList::prepareForScan( unsigned int X, unsigned int Y ) {

  _currElem = _elem( X, Y );
  _idxPixel = _imageArray[ _currElem ] ;
  return ( !_idxPixel || !_Pool[_idxPixel].next )? false : true;

}

//A-----------------------------------------------------------------------
bool ScreenList::getUnit( float& z, float& val,
			  unsigned& cellIdx, bool& isBoundary ) {

  if( !_idxPixel ) return false;

  ScrUnit *h = &_Pool[ _idxPixel ];
  z          = h->z;
  val        = h->val;
  cellIdx    = h->cellIdx;
  isBoundary = h->isBoundary;
  _idxPixel  = h->next;
  return true;
  
}

//A-----------------------------------------------------------------------
unsigned int ScreenList::getListLength( unsigned int elem ) 
{
  unsigned k = elem;
  _idxPixel = _imageArray[ k ];
  if( !_idxPixel ) return 0;
  unsigned int count = 0;
  ScrUnit *h;
  h = &_Pool[ _idxPixel ];
  do {
    count++;
    int next = h->next;
    if( !next ) break;
    h = &_Pool[ next ];
  } while( true );
  return count;
}
//A-----------------------------------------------------------------------
unsigned int ScreenList::getListLength( unsigned x, unsigned y ) 
{
  unsigned int _len = 0;
  unsigned int _k = _elem(x,y);
  if( _imageArray[ _k ] ) _len = getListLength( _elem(x,y) );
  return _len;
}
//A-----------------------------------------------------------------------
void ScreenList::displayListsLength( void ) 
{

  for( unsigned y = 0 ; y < dimY ; y++ ) 
    {
      for( unsigned x = 0 ; x < dimX ; x++ ) 
	{
	  unsigned int k = _elem(x,y);
	  if( _imageArray[ k ] ) 
	    cout << setw(5) << getListLength(k) << " ";
	  else
	    cout << "00000 ";
	}
      cout << endl;
    }
}

// End of module zs_screenLists.cpp ------------------------------------------
//-----------------------------------------------------------------------
