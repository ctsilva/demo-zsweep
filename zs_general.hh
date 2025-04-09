#ifndef _ZS_GENERAL_H
#define _ZS_GENERAL_H

/* -*-C++-*-
*******************************************************************************
*
* File:         zs_general.hh
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

#include <algorithm>
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
#include <stdint.h> // For intptr_t
//#include <GL/gl.h>
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
//#include <sys/time.h> // gettimeofday
//#include <unistd.h>

double get_clock();

using namespace std;

#endif
/*******************************************************************************/
