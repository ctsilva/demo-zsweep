#ifndef _SCREEN_LISTS
#define _SCREEN_LISTS
// zs_screenLists.hh ---------------------------------------------------------
// By Ricardo Farias : rfarias@ams.sunysb.edu
// Date : Jan 30 2000
// 
// Create an array of pointers for a specified structure
// ---------------------------------------------------------------------------

#include "zs_general.hh"

using namespace std;

// Screen List Max Length
#define SL_MAX_LENGTH  16

//----------------------------------------------------------------------------
typedef struct _ScrUnit {

  unsigned int id, prev, next; 
  unsigned int cellIdx;
  bool     isBoundary;
  float    z,val;

} ScrUnit;

typedef ScrUnit *ScrUnitP;

//----------------------------------------------------------------------------
class ScreenList {

private:

  ScrUnitP  _Pool;
  unsigned  int  _idxPool;
  unsigned  int  _chunk;

  unsigned  int  *_imageArray;
  unsigned  int  *_imageLast;
  unsigned  int  _idxPixel;

  unsigned  dimX, dimY;
  unsigned  rangeX[2], rangeY[2];
  unsigned   _x, _y, _currElem;
  ScrUnitP  _currUnit;

private:  // Private Functions

  void  _allocPool( void );
  void  _freeUnit( ScrUnit *h );
  void  _getUnit( void );
  inline unsigned _elem( unsigned X, unsigned Y ) { return X+Y*dimX; }

public:

  ScreenList( unsigned x, unsigned y );
  ~ScreenList() {
    resetImageArray();
    if( _imageLast  ) delete [] _imageLast;
    if( _imageArray ) delete [] _imageArray;
    realloc(_Pool,0); _Pool = 0;
  }

  // Low level Functions -------------------------------------------
  inline ScrUnitP  getUnitTable() { return _Pool; }
  inline unsigned *getScrTable()  { return _imageArray; }
  inline unsigned  getDimX()      { return dimX; }

  // --------------------------------------------------------------
  bool getUnit(float& z, float& val, unsigned& cellIdx, bool& isBoundary);
  void insertUnit( unsigned X, unsigned Y, float z, float val, 
		   unsigned cellIdx, bool isBoundary );

  void resetPixelList( unsigned k );
  void resetImageArray( void );
  void resetImageRange( unsigned int *r );
  void freePixelFirstUnit( unsigned X, unsigned Y );

  void setRange( unsigned int *r );
  void getRange( unsigned int *r );
  bool noRange( void );

  void cleanPixelUptoZ( float zMax, unsigned k );
  void cleanPixelUptoZ( float zMax, unsigned X, unsigned Y );
  void cleanRangeUptoZ( float zMax );

  bool prepareForScan( void );
  bool incScanX( void );
  bool incScanY( void );
  bool prepareForScan( unsigned int X, unsigned int Y );

  unsigned int getListLength( unsigned elem );
  unsigned int getListLength( unsigned x, unsigned y );
  void displayListsLength( void );

};

#endif
// zs_screenLists.hh ---------------------------------------------------------
