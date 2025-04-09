/* -*-C++-*-
*******************************************************************************
*
* File:         zs_fakeSPM.cc
* Description:  For SGI parallel library simulation
* Author:       Ricardo Farias
* Created:      Sat Mar 25 11:30:00pm 2000
* Modified:     Mon Mar 26 08:00:00am 2000
* Language:     C++
*
* rfarias@ams.sunysb.edu
*
*******************************************************************************
*/
#ifdef FAKE_SMP_SGI
#include "zs_fakeSMP.hh"

int __m_myId  = 0;
int __m_procs = 1;

int m_get_myid(void) 
{
  return __m_myId;
}

int m_get_numprocs(void) 
{
  return __m_procs;
}

int m_sync(void)
{
  return 0;
}
int m_lock(void)
{
  return 0;
}
int m_unlock(void)
{
  return 0;
}
int m_set_procs( int procs )
{
  __m_procs = procs;
  return procs;
}
int m_set_myid( int myid )
{
  __m_myId  = myid;
  return myid;
}
int m_fork(void)
{
  return 0;
}
#endif
/*******************************************************************************/
