/*--------------------------------------------------------------------------------------
 * 	File		: 	zs_heap.cpp
 *	Author		: 	Ashish Tiwari, 
 * (several changes, improvements and bug fixes by Claudio)
 * 	Date		: 	24 Nov, 1995
 *
 *	Description	:	Routines here implement a HEAP data-
 *                       structure. The data-structure can be changed
 *                       WITHOUT effecting the other program IF the
 *                       routines "insert", "delete_min", "heapify"
 *                       are provided. The heap is used to store the
 *                       possible NEXT vertices (ie vertices that CAN
 *                       intersect the sweep plane next) in the sweep.
 *                              Routines in here are used by sweep.c
 *
 *	Modify-History	:
 *                       Bug fixes by Claudio Silva, csilva@cs.sunysb.edu
 *                       printHeap added by Claudio.
 * 
 *                       6/10/96 -- changed hard bounds on the arrays and
 *                                  some of the function names. (Claudio)
 *
 *                       6/18/96 -- added the use of distance *functions*
 *                                  in place of the old distance array.
 *                                  This saves a lot of space and adds
 *                                  a considerable amount of flexibility
 *                                  to the data structure. (Claudio)
 *
 *                       12/9/96 -- changed to ANSI C calls, and added
 *                                  new functions for cleaning up the 
 *                                  code. Incorporated all the optimized
 *                                  code.
 *
 *                        6/10/97 -- added "reset" functions.
 *
 * ...............................................................................
 * Adapted to C++ by Ricardo Farias : rfarias@ams.sunysb.edu
 * ...............................................................................
 */
#include "zs_basic.hh"
#include "zs_heap.hh"
using namespace std;

/* Implementation of the heap data-structure. The function
 * INSERT(elmt, root) inserts elmt into the heap root. The
 * function DELETEMIN(root) returns the minimum elmt from
 * the heap and updates the heap root .
 */

/* -------------------------------------------------------------------------------- */
/*                          Vertex Heap funtions                                    */
/* -------------------------------------------------------------------------------- */

void Heap::resetHeapVertex()
{
  _vertexHeapSize = 0;
}

/*
 * Returns the top of the heap if there is anything there.
 */
Point * Heap::vertexHeapNotEmpty(void){

  if(_vertexHeapSize > 0) {
    return (Point *) _vertexIndices[0];
  }
  
  return NULL;
}

/*
 * Pops the top of the heap if there is anything there.
 */
Point * Heap::getMinVertex(void)
{
  Point *vertexPtr = NULL;
  
  if(_vertexHeapSize > 0) {
    vertexPtr = (Point *) GetMinHeap(_vertexIndices, _vertexHeapSize-- );
  }
  
  return vertexPtr;
}

/*------------------------------------------------------------------------------
 * Insert an element in the heap.
 ------------------------------------------------------------------------------*/
void Heap::insertVertex(Point *vertexPtr)
{
  InsertHeap(_vertexIndices, _vertexHeapSize++, (intptr_t) vertexPtr);
  assert( _vertexHeapSize <  SP_SWEEP_QUEUE_SIZE );
}

/*------------------------------------------------------------------------------
 * Sorts the vertex heap.
 ------------------------------------------------------------------------------*/
void Heap::heapifyVertex(int size)
{
  _vertexHeapSize = size;
  Heapify(_vertexIndices, _vertexHeapSize );
}

intptr_t * Heap::getVertexHeapArray()
{
  return _vertexIndices;
}

void Heap::heapDebug()
{
  fprintf(stderr,"size of heap = %d\n", _vertexHeapSize);
  PrintHeap(_vertexIndices, _vertexHeapSize, 0);
}


/* ----------------------------------------------------------------------------- */
/*                        General Heap functioons.                               */
/* ----------------------------------------------------------------------------- */
/* Exchange two elements in the heap */
void Heap::swap(intptr_t *i, intptr_t *j) {
  intptr_t t;
  t = *i;
  *i = *j;
  *j = t;
}

/*------------------------------------------------------------------------------
 * The heap data-structure
 *
 * The left child of heap[i] is heap[2*i+1] and the right child
 * is heap[2*i+2].						
 ------------------------------------------------------------------------------*/
/*
 * Print the heap.
 */
void Heap::PrintHeap(intptr_t   A[], int   n, int   i)
{
  if(i < n) {
    fprintf(stderr,"HEAP %d: %p\n", i, (void*)A[i]);
  }

  if((2*i+1)<n)
    PrintHeap(A, n, 2*i+1);
  
  if((2*i+2)<n)
    PrintHeap(A, n, 2*i+2);
}


/*------------------------------------------------------------------------------
 * 	Proc Name	: 	heapify
 *
 *	Return Values	: 	void
 *
 * 	Input Variables	: 	A (array containing elements)
 *                              n (size of heap)
 *                              D (distance array, measure)
 *
 *	Output Variables:	none
 *
 *	Update Variables:	A (the heap)
 *
 *	Global Variables:	none
 *
 *	Description	:	Readjust the elements in A[0:n-1] to 
 *                              form a heap
 *
 ------------------------------------------------------------------------------*/
void Heap::Heapify(intptr_t   A[], int   n ) {
  int i;

  for (i = (n-1)/2; i >= 0; i--) {
    AdjustHeap(A, i, n );
  }

#if 0
  printHeap(A,n,D,0);
#endif
}


/*------------------------------------------------------------------------------
 * 	Proc Name	: 	adjust
 *
 *	Return Values	: 	void
 *
 * 	Input Variables	: 	A (the heap)
 *                              i (root where to adjust)
 *                              n (size of heap)
 *                              D (the distance array, measure)
 *
 *	Output Variables:	none
 *
 *	Update Variables:	A (the heap)
 *
 *	Global Variables:	none
 *
 *	Description	:	The complete binary trees with roots 
 *                              A[2*i+1] and A[2*i+2] are combined with 
 *                              A[i] to form a single heap, 0 <= i < n.
 *                              No node has an address greater than n-1 
 *                              or less than 0
 ------------------------------------------------------------------------------*/
void Heap::AdjustHeap(intptr_t   A[], int   i, int n ) {
  int j;

  j = 2*i + 1;
  while (j < n) {

    /* VERTEX */
    if ((j < n-1) && (((Point *) (A[j]))->getZ() >  ((Point *) (A[j+1]))->getZ())) {
      j++;
    }

    /* VERTEX */
    if ((((Point *) (A[(j-1)/2]))->getZ() <=  ((Point *) (A[j]))->getZ())) {	  
      break;
    }
    else {
      swap(&A[(j-1)/2], &A[j]);
    }

    j *= 2;
    j++;

  }

}

/*------------------------------------------------------------------------------
 * 	Proc Name	: 	delete_min
 *
 *	Return Values	: 	int (the index of min elmt in heap)
 *
 * 	Input Variables	: 	n (size of heap)
 *                              D (distance array, measure)
 *
 *	Output Variables:	none
 *
 *	Update Variables:	A (the heap)
 *
 *	Global Variables:	none
 *
 *	Description	:	The function delete_min(A, n) deletes the 
 *                              minimum element index * * from the heap A 
 *                              containing n elements and where the key 
 *                              values  * * of the elements are stored in 
 *                              the array D indexed by the indices* * and 
 *                              returns the index of the minimum element.
 ------------------------------------------------------------------------------*/
intptr_t Heap::GetMinHeap(intptr_t   A[], int   n ) {
  swap(&A[0], &A[n-1]);
  AdjustHeap(A, 0, n-1 );
  return(A[n-1]);
}


/*------------------------------------------------------------------------------
 * 	Proc Name	: 	insert
 *
 *	Return Values	: 	void
 *
 * 	Input Variables	: 	n (size of heap)
 *                              D (distance array)
 *                              item_index (item to be inserted)
 *
 *	Output Variables:	none
 *
 *	Update Variables:	A (the heap)
 *
 *	Global Variables:	none
 *
 *	Description	:	Insert the element with index "item-index"
 *                              into heap A[] containing * * n elements. 
 *                              The key values of items are stored in the 
 *                              array D[].
 *
 ------------------------------------------------------------------------------*/
void Heap::InsertHeap(intptr_t   A[], int   n, intptr_t   item_index) {
  int i, j;

  i = (n-1) / 2;
  j = n;
  A[n] = item_index;

  while (j > 0) {

    /*
     * Bug fix by Claudio Silva, 3/21/96
     * changed a "continue" to a "break" statement.
     */
    /* VERTEX */
    if ((((Point *) (A[j]))->getZ() > ((Point *) (A[i]))->getZ())) {	  
      break;
    }

    swap(&A[i], &A[j]);

    j = i;
    i = (j-1) / 2;
  }
}

/*------------------------------------------------------------------------------
 * End of file.
 ------------------------------------------------------------------------------*/
