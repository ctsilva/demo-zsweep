/* -*-C++-*-
*******************************************************************************
*
* File:         zs_general.cc
* Description:  For SGI library compatibility
* Author:       Ricardo Farias
* Created:      Fri Mar 10 11:30:00 2000
* Modified:     Mon Mar 20  5:00:00 2000
* Language:     C++
*
* rfarias@ams.sunysb.edu
*
*******************************************************************************
*/
#include "zs_general.hh"

double get_clock()
{
/*
#ifdef SMP_SGI
#ifndef FAKE_SMP_SGI
  struct timeval t;
  gettimeofday(&t);
  return (double) t.tv_sec + (double) t.tv_usec * 1E-6;
#else
  struct timeval t;
  struct timezone tz;
  gettimeofday(&t,&tz);
  return (double) t.tv_sec + (double) t.tv_usec * 1E-6;
#endif
#else
  struct timeval t;
  struct timezone tz;
  gettimeofday(&t,&tz);
  return (double) t.tv_sec + (double) t.tv_usec * 1E-6;
#endif
*/
return 0.0;
  }

/*******************************************************************************/
