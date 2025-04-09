#ifndef _SWP_DEBUG_H
#define _SWP_DEBUG_H

/* My own parameter checking */
#define DEBUG 0

/* for assert() */
#define NDEBUG 1

#include <assert.h>

#ifdef BROKENEXCEPTIONS
#define throw THROW
extern int THROW(char *str);
#endif

#endif

