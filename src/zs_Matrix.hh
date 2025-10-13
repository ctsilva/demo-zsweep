/* -*-C++-*-
*******************************************************************************
*
* File:         matrix.h
* Description:  
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Sun 10:00:00 1999
* Modified:     
* Language:     C++
* Package:      RayTracer
* Status:       Under development
*
* (C) Copyright 1999
*
*******************************************************************************
*/

#ifndef _RF_MATRIX_H
#define _RF_MATRIX_H
/*
#include <vector>
#include <list>
#include <map>
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <sys/time.h>
#include <unistd.h>
*/
#include "zs_general.hh"

#ifndef M_PI
//#define PI 3.1415926535897
#define M_PI 3.1415926535897
#endif

class Matrix
{
public:

  float _m[3][3];

  Matrix()
    {
      _m[0][0] = 1; _m[0][1] = 0; _m[0][2] = 0;
      _m[1][0] = 0; _m[1][1] = 1; _m[1][2] = 0;
      _m[2][0] = 0; _m[2][1] = 0; _m[2][2] = 1;
    }
  
  // Angles are accepted in degree
  Matrix( float phix, float phiy, float phiz )
    {
      // Calculate angles in radian
      phix *= (float)M_PI/180.0f;
      phiy *= (float)M_PI/180.0f;
      phiz *= (float)M_PI/180.0f;

      float cos_phix = cos(phix);
      float cos_phiy = cos(phiy);
      float sin_phix = sin(phix);
      float sin_phiy = sin(phiy);

      _m[0][0] = cos_phiy;
      _m[0][1] = 0;
      _m[0][2] = -sin_phiy;
      _m[1][0] = sin_phix*sin_phiy;
      _m[1][1] = cos_phix;
      _m[1][2] = cos_phiy*sin_phix;
      _m[2][0] = cos_phix*sin_phiy;
      _m[2][1] = -sin_phix;
      _m[2][2] = cos_phix*cos_phiy;

      /* Original matrix generation
      float cos_phix = cos(phix);
      float cos_phiy = cos(phiy);
      float cos_phiz = cos(phiz);
      float sin_phix = sin(phix);
      float sin_phiy = sin(phiy);
      float sin_phiz = sin(phiz);
      _m[0][0] = cos_phiy*cos_phiz;
      _m[0][1] = cos_phiy*sin_phiz;
      _m[0][2] = -sin_phiy;
      _m[1][0] = cos_phiz*sin_phix*sin_phiy - cos_phix*sin_phiz;
      _m[1][1] = cos_phix*cos_phiz + sin_phix*sin_phiy*sin_phiz;
      _m[1][2] = cos_phiy*sin_phix;
      _m[2][0] = cos_phix*cos_phiz*sin_phiy + sin_phix*sin_phiz;
      _m[2][1] = -(cos_phiz*sin_phix) + cos_phix*sin_phiy*sin_phiz;
      _m[2][2] = cos_phix*cos_phiy;
      */
    }

  Matrix operator=( Matrix& m )
    {
      Matrix mat = *this;
      mat._m[0][0] = m._m[0][0]; mat._m[0][1] = m._m[0][1]; mat._m[0][2] = m._m[0][2];
      mat._m[1][0] = m._m[1][0]; mat._m[1][1] = m._m[1][1]; mat._m[1][2] = m._m[1][2];
      mat._m[2][0] = m._m[2][0]; mat._m[2][1] = m._m[2][1]; mat._m[2][2] = m._m[2][2];
      return mat;
    }
  
  void UpdateRotationMatrix( float phix, float phiy, float phiz ) 
    {

      Matrix update( phix, phiy, phiz );
      for( int i = 0 ; i < 3 ; i++ )
	{
	  for( int j = 0 ; j < 3 ; j++ )
	    {
	      this->_m[i][j] = update._m[i][j];
	    }
	}
    }
  void UpdateRotationMatrix( float *update ) 
    {

      for( int i = 0 ; i < 3 ; i++ )
	{
	  for( int j = 0 ; j < 3 ; j++ )
	    {
	      this->_m[i][j] = *(update + i*4 + j );
	    }
	}
    }
/*
  void UpdateRotationMatrix( float phix, float phiy, float phiz ) 
    {
      Matrix tmp;
      Matrix update( phix, phiy, phiz );
      
      for( int k = 0 ; k < 3 ; k++ )
	{
	  for( int i = 0 ; i < 3 ; i++ )
	    {
	      float acum = 0;
	      for( int j = 0 ; j < 3 ; j++ )
		{
		  acum += update._m[k][j] * this->_m[j][i];
		}
	      tmp._m[k][i] = acum;
	    }
	}
      for( int i = 0 ; i < 3 ; i++ )
	{
	  for( int j = 0 ; j < 3 ; j++ )
	    {
	      this->_m[i][j] = tmp._m[i][j];
	    }
	}
    }
*/  
  void Display()
    {
      cout << "[ " << _m[0][0] << " | " << _m[0][1] << " | " << _m[0][2] << " ]\n";
      cout << "[ " << _m[1][0] << " | " << _m[1][1] << " | " << _m[1][2] << " ]\n";
      cout << "[ " << _m[2][0] << " | " << _m[2][1] << " | " << _m[2][2] << " ]\n\n";
    }
};

#endif
