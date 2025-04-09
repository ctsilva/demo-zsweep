// hashTable.cpp -------------------------------------------------------------
// By Ricardo Farias : rfarias@ams.sunysb.edu
// Date : Jan 30 2000
// 
// This class was based on hash code by Claudio Silva, csilva@cs.sunysb.edu
// ---------------------------------------------------------------------------
#include "zs_hash.hh"
using namespace std;


void Hash::_copyHash( HashUnitP Old, HashUnitP New, unsigned numChunks )
{
  unsigned i;
  for( i = 0 ; i < numChunks ; i++ ) 
    {
      New->isBoundary = Old->isBoundary;
      New->isBoundary = Old->isBoundary;
      New->cellIdx    = Old->cellIdx;
      New->n0         = Old->n0;
      New->n1         = Old->n1;
      New->n2         = Old->n2;
      New->next       = Old->next;
    };

}

void Hash::_initList( HashUnitP New, unsigned numChunks )
{
  unsigned i;
  for( i = 0 ; i < numChunks-1 ; i++ ) 
    {

      New[i].next = &New[i+1];

    }
  New[i].next = NULL; // End of list

}

//-----------------------------------------------------------------------
void Hash::_allocPool( unsigned n ) 
{
  
  HashUnitP tmp = new HashUnit[n*(numChunks+1)];
  if( tmp == NULL ) 
    {
      cout << "Error allocating memory for hash" << endl;
      exit(1);
    }
  
  if( numChunks == 0 ) 
    {

      chunk = n;
      _initList( tmp, chunk );
      hashStatic = hashPtr = tmp;
      
    }
  else
    {
      
      _copyHash( hashStatic, tmp, numChunks*chunk );
      _initList( tmp+chunk*numChunks, chunk );
      if( hashStatic ) delete [] hashStatic;
      hashStatic = tmp;
      hashPtr    = tmp + chunk*numChunks;

    }
  
  numChunks++;
  
}


//-----------------------------------------------------------------------
HashUnit *Hash::_makeUnit( void ) {

  HashUnit *tmp;
  
  if(hashPtr == NULL) _allocPool( chunk  );

  tmp = hashPtr;
  hashPtr = hashPtr->next;

  tmp->next = NULL;
  
  numUnits++;
  
  if( !tmp ) {
    cout << "Error: in makeUnit function." << endl;
    exit(1);
  }

  return tmp;

}

//-----------------------------------------------------------------------
void Hash::_freeUnit( HashUnit *h ) {
  h->next = hashPtr;
  hashPtr = h;
}

//-----------------------------------------------------------------------
void Hash::resetHashTable( void ) 
{
  unsigned i;
  for( i = 0 ; i < HASH_SIZE ; i++ ) {
    
    if( hashTable[i] ) 
      {
	HashUnit *h, *tmp;
	tmp = h = hashTable[i];
	while( h ) {
	  h = h->next;
	  _freeUnit( tmp );
	  tmp = h;
	}
      }
    hashTable[i] = NULL;
  }
  numUnits = 0;
}

//-----------------------------------------------------------------------
void Hash::_swap( unsigned *n0, unsigned *n1 ) {
  unsigned n = *n0; *n0 = *n1; *n1 = n;
}
//-----------------------------------------------------------------------
void Hash::_order( unsigned *n0, unsigned *n1, unsigned *n2 ) {

  if( *n0 > *n1 ) _swap( n0, n1 );
  if( *n1 > *n2 ) _swap( n1, n2 );
  if( *n0 > *n1 ) _swap( n0, n1 );

}

//-----------------------------------------------------------------------
// This function expects the values to be ordered
int Hash::_insUnit( unsigned n0, unsigned n1, unsigned n2, unsigned cellIdx ) {
  
  unsigned int hashNumber;
  HashUnit *tmp;
  
  assert( !( n0 == n1 && n1 == n2 ) );
  assert( n0 < n1 && n1 < n2 );
  
  hashNumber = HASH( n0, n1, n2 );
  
  tmp = hashTable[ hashNumber ];
  while(tmp != NULL) {
    
    if(( tmp->n0 == n0 )&&(tmp->n1 == n1)&&( tmp->n2 == n2 )) {
      // When a face is found, we set it as interior
      tmp->isBoundary = false;
      return false; // Could not insert, was already there.
    }
    tmp = tmp->next;
    
  }
  
  // If not there insert it.
  assert(tmp == NULL);
    
  tmp = _makeUnit();
  tmp->isBoundary = true; // When inserted all faces are considered boundary
  tmp->cellIdx  = cellIdx;
  tmp->n0 = n0;
  tmp->n1 = n1;
  tmp->n2 = n2;
  tmp->next = hashTable[ hashNumber ];
  hashTable[ hashNumber ] = tmp;
  return true; // Means that the Unit could be inserted into the hash table
  
}
//-----------------------------------------------------------------------
int Hash::insertUnit( unsigned n0, unsigned n1, unsigned n2, unsigned cellIdx ) {
  
  if( n0 == n1 && n1 == n2 ) {
    cout << "ERROR, received the same vertex three times : " <<  n0 << endl;
    exit(1);
  }

  _order( &n0, &n1, &n2 );
  assert( n0 < n1 && n1 < n2 );
  return _insUnit( n0, n1, n2, cellIdx );

}

//-----------------------------------------------------------------------
void Hash::prepareForScan(void) {
  if( numUnits == 0 ) return;
  currHashUnit    = 0;
  currHashUnitPtr = hashTable[ currHashUnit ];
  while( currHashUnitPtr == NULL )
    currHashUnitPtr = hashTable[ ++currHashUnit ];
  
}

//-----------------------------------------------------------------------
bool Hash::getUnit( unsigned& n0, unsigned& n1, unsigned& n2, unsigned& cellIdx ) {

  bool retValue = false;
  unsigned i;

  if( numUnits == 0) return retValue;

  if( currHashUnitPtr != NULL ) {

    retValue = currHashUnitPtr->isBoundary;
    n0       = currHashUnitPtr->n0;
    n1       = currHashUnitPtr->n1;
    n2       = currHashUnitPtr->n2;
    cellIdx  = currHashUnitPtr->cellIdx;

    if( currHashUnitPtr->next != NULL )
      currHashUnitPtr = currHashUnitPtr->next;
    else {
      for( i = currHashUnit+1 ; i < HASH_SIZE ; i++ ) {
	if( hashTable[i] != NULL ) {
	  currHashUnitPtr= hashTable[i];
	  currHashUnit = i;
	  break;
	}
      }
      if( i == HASH_SIZE )
	currHashUnitPtr = NULL;
    }
    
  }

  return retValue;
  
}

// End of module hashTable.cpp -----------------------------------------------
