#ifndef _ZS_LIGHT_H
#define _ZS_LIGHT_H

/* -*-C++-*-
*******************************************************************************
*
* File:         zs_light.hh
* Description:  
* Author:       Ricardo Farias : rfarias@ams.sunysb.edu
* Created:      Fri Jan 28 2000
* Modified:     Fri Jan 28 2000
* Language:     C++
* Package:      RayTracer
* Status:       Experimental
*
* (C) Copyright 2000 
*
*******************************************************************************
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <sys/time.h>
#include <unistd.h>
#ifdef OPENGL
#include <GL/gl.h>
#include <GL/glu.h>
#endif
*/
#include "zs_general.hh"
using namespace std;


class Lighting
{
  /* Transfer function tables */
  float _red[256];
  float _green[256];
  float _blue[256];
  float _opacity[256];

  struct rgb *_currPixel;
  float _currOpacity;
  float _brightness;
  
public:

  /* Constructor: read a color/opacity tables from a file */
  Lighting(char *fname="lighting.tf", float bright = 1.0) 
    {
      FILE *f=fopen(fname,"r");
      if (f==0) 
	throw("Can not open transfer function file");
      char buff[256],*line;;
      int readOpacity=0, readRGB=0;
      int prevIndex = -1;
      float prevValue = 0.0, prevR = 0.0, prevG = 0.0, prevB = 0.0;
      _brightness = bright;

      while (fgets(buff,sizeof(buff),f)!=NULL)
	{
	  line=buff;
	  while(line[0] && isspace(line[0])) line++;
	  if (line[0]==0 || line[0]=='#') continue;
	  
	  if (line[0]=='%')
	    {
	      if (line[1]=='o') 
		{
		  readOpacity=1;
		  readRGB=0;
		  prevIndex=-1;
		}
	      else if (line[1]=='v') 
		{
		  readRGB=1;
		  readOpacity=0;
		  prevIndex=-1;
		}
	      else
		printf("Lighting: Unknown keyword: %s\n", line);
	    }
	  else if (readOpacity)
	    {
	      unsigned index, i;
	      float value;
	      if (sscanf(line,"%u %f", &index, &value)!=2)
		throw("Lighting: Bad opacity line");
	      if (prevIndex==-1) 
		for (i=0; i<=index; i++)
		  _opacity[i] = value;
	      else
		{
		  float dvalue = (value - prevValue) / (index-prevIndex);
		  for (i=prevIndex; i<=index; i++)
		    _opacity[i] = prevValue+dvalue*(i-prevIndex);
		}
	      prevValue = value;
	      prevIndex = index;
	    }
	  else if (readRGB)
	    {
	      unsigned index, i;
	      float r,g,b;
	      if (sscanf(line,"%u %f %f %f", &index, &r, &g, &b)!=4)
		throw("Lighting: Bad RGB line");
	      r*=255; g*=255; b*=255;
	      if (prevIndex==-1) 
		for (i=0; i<=index; i++)
		  {
		    _red[i] = r;
		    _green[i] = g;
		    _blue[i] = b;
		  }
	      else
		{
		  float dr = (r - prevR) / (index-prevIndex);
		  float dg = (g - prevG) / (index-prevIndex);
		  float db = (b - prevB) / (index-prevIndex);
		  for (i=prevIndex; i<=index; i++)
		    {
		      _red[i] = prevR+dr*(i-prevIndex);
		      _green[i] = prevG+dg*(i-prevIndex);
		      _blue[i] = prevB+db*(i-prevIndex);
		    }
		}
	      prevR = r; prevG = g; prevB = b;
	      prevIndex = index;
	    }
	  else
	    throw("Lighting: No keyword before non-empty line");
	}
    }

  void PrintTables(char *fname="expanded.tf")
    {
      FILE *f=fopen(fname,"w");
      if (f==0) throw("Can not open transfer function dump file");
      for(unsigned i=0; i<256; i++)
	fprintf(f,"%u\t%f\t%f\t%f\t%f\n", 
		i, _opacity[i], _red[i], _green[i], _blue[i]);
    }

  void readExpanded( char *fname="expanded.tf" )
  {
    unsigned ind;
    float o, r, g, b;
    FILE *f=fopen(fname,"r");
    if (f==0) throw("Can not open expanded transfer file.");
    for(unsigned i=0; i<256; i++) {
      fscanf(f,"%u %f %f %f %f", &ind, &o, &r, &g, &b );
      _opacity[ind] = o;
      _red[ind]   = r;
      _green[ind] = g;
      _blue[ind]  = b;
    }
  }
  
  /* Show the color table as stripes on the right of ViewPlane */
  void ShowLegend(ViewPlane *vp, int width = 7, int gap=3)
    {
      unsigned x,y;
      for (y=0; y<vp->_yres; y++) 
	{
	  unsigned i = 255-255*y/vp->_yres;
	  for (x=vp->_xres-width-gap-1; x<vp->_xres-gap; x++)
	    {
	      vp->Raster(x,y).r = (unsigned char) _red[i];
	      vp->Raster(x,y).g = (unsigned char) _green[i];
	      vp->Raster(x,y).b = (unsigned char) _blue[i];
	    }
	}
    }

  void SetPixel(struct rgb *pixel)
    {
      _currPixel = pixel;
      _currOpacity = 0.0;
      _currPixel->r = _currPixel->g = _currPixel->b = 0;
    }
  
  void SetCurrPixel(struct rgb *pixel) {
    _currPixel = pixel;
  }


#ifdef ORIGINAL
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
  /* Update a pixel in rgb
   */
#define TRUELIGHT
  inline GLubyte
  UpdateColor(GLubyte c, float *table, 
	      unsigned i1, unsigned i2, 
	      float o1, float o2, float dz)
    {
      float c1 = table[i1], c2 = table[i2]; 
      unsigned C = (unsigned) (c + 
			       _brightness*(
					   (c1+c2)*dz/2.0*(1-_currOpacity) 
					   - dz*dz/24.0 * 
					   (3.0*c1*o1+5.0*c2*o1+c1*o2+3.0*c2*o2))); 
      return (C>255) ? 255:C; 
    }

  // Returns true  : Opacity is less than 1.0
  //         false : Pixel saturated
  int Update(float v1, float v2, float dz)
    {
      /* Note: Values are assumed to be scaled to [0:255] */
      unsigned i1 = (unsigned) v1;
      unsigned i2 = (unsigned) v2;
      if (i1>255) i1=255;
      if (i2>255) i2=255;

      //      if( v1<6 || v2 ) return 0;
      
#ifdef TRUELIGHT
      if (i1>i2)
	{
	  unsigned tmp = i2;
	  i2 = i1;
	  i1 = tmp;
	}
      if (i1==i2)
	{
	  float o1 = _opacity[i1];
	  _currPixel->r = UpdateColor(_currPixel->r, _red,   i1, i1, o1, o1, dz);
	  _currPixel->g = UpdateColor(_currPixel->g, _green, i1, i1, o1, o1, dz);
	  _currPixel->b = UpdateColor(_currPixel->b, _blue,  i1, i1, o1, o1, dz);
	}
      else
	{
	  dz /= i2-i1;
	  for (unsigned i=i1; i<i2; i++)
	    {
	      float o1 = _opacity[i], o2=_opacity[i+1];
	      _currPixel->r = UpdateColor(_currPixel->r, _red,   i, i+1, o1, o2, dz);
	      _currPixel->g = UpdateColor(_currPixel->g, _green, i, i+1, o1, o2, dz);
	      _currPixel->b = UpdateColor(_currPixel->b, _blue,  i, i+1, o1, o2, dz);
#else
	      float o1 = _opacity[i1], o2=_opacity[i2];
	      _currPixel->r = UpdateColor(_currPixel->r, _red,   i1, i2, o1, o2, dz);
	      _currPixel->r = UpdateColor(_currPixel->g, _green, i1, i2, o1, o2, dz);
	      _currPixel->r = UpdateColor(_currPixel->b, _blue,  i1, i2, o1, o2, dz);
#endif	    
	      _currOpacity = _currOpacity + (o1+o2)*dz/2.0;
	      if (_currOpacity>1.0) {
		_currOpacity=1.0;
		return false;
	      }
#ifdef TRUELIGHT
	    }
	}
#endif
      return true;
    }
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
#endif

  inline void
  UpdateColor(GLubyte &r, GLubyte &g, GLubyte &b,
	      float *tableR, float *tableG, float *tableB, 
	      unsigned i1, unsigned i2, float o1, float o2, float dz)
    {
      float r1 = tableR[i1];
      float r2 = tableR[i2];
      float g1 = tableG[i1];
      float g2 = tableG[i2];
      float b1 = tableB[i1];
      float b2 = tableB[i2];

      float transparency = (1-_currOpacity);
#define INV_24 0.04166666666
      unsigned R = (unsigned) (r + _brightness*( (r1+r2)*dz*transparency*0.5- 
						 dz*dz*INV_24 * 
						 (3.0*r1*o1+
						  5.0*r2*o1+
						  r1*o2+
						  3.0*r2*o2))); 
      r = (R>255) ? 255:R;

      unsigned G = (unsigned) (g + _brightness*( (g1+g2)*dz*transparency*0.5- 
						 dz*dz*INV_24 * 
						 (3.0*g1*o1+
						  5.0*g2*o1+
						  g1*o2+
						  3.0*g2*o2))); 
      g = (G>255) ? 255:G;

      unsigned B = (unsigned) (b + _brightness*( (b1+b2)*dz*transparency*0.5- 
						 dz*dz*INV_24 * 
						 (3.0*b1*o1+
						  5.0*b2*o1+
						  b1*o2+
						  3.0*b2*o2))); 
      b = (B>255) ? 255:B;
    }


  // Returns true  : Opacity is less than 1.0
  //         false : Pixel saturated
  int Update(float v1, float v2, float dz)
    {
      /* Note: Values are assumed to be scaled to [0:255] */
      unsigned i1 = (unsigned) v1;
      unsigned i2 = (unsigned) v2;
      if (i1>255) i1=255;
      if (i2>255) i2=255;

      if (i1>i2)
	{
	  unsigned tmp = i2;
	  i2 = i1;
	  i1 = tmp;
	}
      
      if (i1==i2)
	{
	  float o1 = _opacity[i1];
	  UpdateColor(_currPixel->r, _currPixel->g, _currPixel->b, 
		      _red, _green, _blue,
		      i1, i1, o1, o1, dz);
	}
      else
	{
	  dz /= i2-i1;
	  for (unsigned i=i1; i<i2; i++)
	    {
	      float o1 = _opacity[i], o2=_opacity[i+1];

	      UpdateColor(_currPixel->r, _currPixel->g, _currPixel->b, 
			  _red, _green, _blue,
			  i, i+1, o1, o2, dz);

	      _currOpacity = _currOpacity + (o1+o2)*dz*0.5;
	      if (_currOpacity>1.0) {
		_currOpacity=1.0;
		return false;
	      }
	    }
	}
      
      return true;
    }

  inline void
  UpdateColor2(GLubyte &r, GLubyte &g, GLubyte &b,
	      float *tableR, float *tableG, float *tableB, 
	      unsigned i1, unsigned i2, float o1, float o2, float dz)
    {
      float r1 = tableR[i1];
      float r2 = tableR[i2];
      float g1 = tableG[i1];
      float g2 = tableG[i2];
      float b1 = tableB[i1];
      float b2 = tableB[i2];

      float transparency = (1-_currOpacity);
      
      unsigned R = (unsigned) (r + _brightness*((r1+r2)*transparency*0.5));
      r = (R>255) ? 255:R;

      unsigned G = (unsigned) (g + _brightness*((g1+g2)*transparency*0.5));
      g = (G>255) ? 255:G;

      unsigned B = (unsigned) (b + _brightness*((b1+b2)*transparency*0.5));
      b = (B>255) ? 255:B;
    }


  // Returns true  : Opacity is less than 1.0
  //         false : Pixel saturated
  int Update2(float v1, float v2, float dz)
    {
      /* Note: Values are assumed to be scaled to [0:255] */
      unsigned i1 = (unsigned) v1;
      unsigned i2 = (unsigned) v2;
      if (i1>255) i1=255;
      if (i2>255) i2=255;

      if (i1>i2)
	{
	  unsigned tmp = i2;
	  i2 = i1;
	  i1 = tmp;
	}
      
      if (i1==i2)
	{
	  float o1 = _opacity[i1];
	  UpdateColor2(_currPixel->r, _currPixel->g, _currPixel->b, 
		      _red, _green, _blue,
		      i1, i1, o1, o1, dz);
	}
      else
	{
	  dz /= i2-i1;
	  for (unsigned i=i1; i<i2; i++)
	    {
	      float o1 = _opacity[i], o2=_opacity[i+1];

	      UpdateColor2(_currPixel->r, _currPixel->g, _currPixel->b, 
			   _red, _green, _blue,
			   i, i+1, o1, o2, dz);

	      _currOpacity = _currOpacity + (o1+o2)*dz*0.5;
	      if (_currOpacity>1.0) {
		_currOpacity=1.0;
		return false;
	      }
	    }
	}
      
      return true;
    }

  inline void copyPixel( struct rgb *pixel ) 
  {
    pixel->r = _currPixel->r;
    pixel->g = _currPixel->g;
    pixel->b = _currPixel->b;
  }

  inline void setColor( unsigned int i, float r, float g, float b ) 
  {
    _red[i] = r; _green[i] = g; _blue[i]  = b;
  }
  inline void setColor( unsigned int i, float r, float g, float b, float s ) 
  {
    _red[i] = r; _green[i] = g; _blue[i]  = b; _opacity[i] = s;
  }
  inline void zeroOpacity( void )
  {
    for( unsigned int i = 0 ; i < 256 ; i++ )
      _opacity[i] = 0;
  }

  /* "Invalidate" current pixel --- some error has occured! 
   */
  inline void SetBad()
  {
    // _currPixel->r = _currPixel->g = _currPixel->b = 0;
  }
};

#endif
