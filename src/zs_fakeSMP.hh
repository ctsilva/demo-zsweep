#ifndef _FAKE_SMP_SGI
#define _FAKE_SMP_SGI

/* -*-C++-*-
*******************************************************************************
*
* File:         zs_fakeSPM.hh
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

#include <fstream>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

int m_get_myid(void);
int m_get_numprocs(void);
int m_sync(void);
int m_lock(void);
int m_unlock(void);
int m_set_procs( int procs );
int m_set_myid( int myid );
int m_fork(void);

#endif
#endif
/*******************************************************************************/
