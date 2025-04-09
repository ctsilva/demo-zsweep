#ifndef _HASH_TABLE
#define _HASH_TABLE
// zs_hash.hh ----------------------------------------------------------------
// By Ricardo Farias : rfarias@ams.sunysb.edu
// Date : Jan 30 2000
// 
// This class was based on hash code by Claudio Silva, csilva@cs.sunysb.edu
// ---------------------------------------------------------------------------
#include "zs_general.hh"

//#define HASH_SIZE 1000
#define HASH_SIZE 201

#define HASH(x,y,z) ( ( ((int)(x<<4)) + ((int)(y<<2)) + ((int)(z)) ) % HASH_SIZE )

//#define MIN(a,b,c) ( ( (a)<(b) && (a)<(c) ) ? (a) : ( ( (b)<(c) )? (b) : (c) ) ) 
//#define MAX(a,b,c) ( ( (a)>(b) && (a)>(c) ) ? (a) : ( ( (b)>(c) )? (b) : (c) ) ) 
//#define MED(a,b,c) ( ( (a)>(b) && (a)>(c) ) ? (a) : ( ( (b)>(c) )? (b) : (c) ) ) 

//----------------------------------------------------------------------------
typedef struct _HashUnit {

  bool isBoundary;
  unsigned cellIdx;
  unsigned n0, n1, n2;  // The hash is computed over these three values

  _HashUnit *next;

} HashUnit;

typedef HashUnit *HashUnitP;

//----------------------------------------------------------------------------
class Hash {

private:

  unsigned   chunk;
  HashUnitP *hashTable;
  HashUnitP  hashPtr, hashStatic;
  unsigned   numUnits, numChunks;
  unsigned   currHashUnit;
  HashUnitP  currHashUnitPtr;

  // Private Functions
  HashUnitP _makeUnit(void);
  void      _allocPool( unsigned n );
  void      _freeUnit( HashUnit *h );
  void      _swap( unsigned *n0, unsigned *n1 );
  void      _order( unsigned *n0, unsigned *n1, unsigned *n2 );
  int       _insUnit( unsigned n0, unsigned n1, unsigned n2, unsigned cellIdx );
  void      _copyHash( HashUnitP Old, HashUnitP New, unsigned numChunks );
  void      _initList( HashUnitP New, unsigned numChunks );

public:

  Hash( unsigned n ) {

    int i;
    hashTable = new HashUnitP [ HASH_SIZE ];
    if( hashTable == NULL )
      throw( "Could not allocate memory for hashTable.");
    for( i = 0 ; i < HASH_SIZE ; i++ )
      hashTable[i] = NULL;
    hashStatic = hashPtr = NULL;
    numUnits = numChunks = 0;
    _allocPool( n );

  }

  ~Hash() {
    if( hashStatic ) delete [] hashStatic;
    if( hashTable ) delete [] hashTable;
  }

  int insertUnit( unsigned n0, unsigned n1, unsigned n2, unsigned cellIdx );

  unsigned getNumOfUnits( void) { return numUnits; }

  void prepareForScan(void);
  bool getUnit( unsigned& n0, unsigned& n1, unsigned& n2, unsigned& cellIdx );
  void resetHashTable( void );

};

#endif
// End of zs_hash.hh ---------------------------------------------------------
