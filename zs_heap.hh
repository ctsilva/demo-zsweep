//---------------------------------------------------------
// zs_heap.hh
//---------------------------------------------------------
#ifndef _HEAP_
#define _HEAP_

#include "zs_basic.hh"

#define SP_SWEEP_QUEUE_SIZE 250000

class Heap {

private:
  int  _vertexHeapSize;
  intptr_t *_vertexIndices;

public:

  Heap( void ) {
    _vertexHeapSize = 0;
    if( (_vertexIndices = new intptr_t[SP_SWEEP_QUEUE_SIZE]) == 0 )
      throw("Could not allocate memory for heap\n");
  }
  Heap( unsigned size ) {
    _vertexHeapSize = 0;
    if( (_vertexIndices = new intptr_t[size]) == 0 )
      throw("Could not allocate memory for heap\n");
  }
  ~Heap() { if( _vertexIndices ) delete[] _vertexIndices; }

  Point *getMinVertex(void);
  Point *vertexHeapNotEmpty(void);
  void   insertVertex(Point *);
  void   heapifyVertex(int);
  intptr_t *getVertexHeapArray(void);
  void   resetHeapVertex();

  void   InsertHeap(intptr_t A[], int, intptr_t);
  intptr_t GetMinHeap(intptr_t A[], int );
  void   Heapify(intptr_t A[], int );
  void   AdjustHeap(intptr_t A[], int, int );
  void   PrintHeap(intptr_t A[], int , int );
  void   heapDebug();
  void   swap(intptr_t *i, intptr_t *j);

};
#endif
//---------------------------------------------------------







