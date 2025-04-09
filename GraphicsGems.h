/* 
 * GraphicsGems.h  
 * Version 1.0 - Andrew Glassner
 * from "Graphics Gems", Academic Press, 1990
 *
 * (modifications by Claudio T. Silva, csilva@ams.sunysb.edu)
 */

#ifndef GG_H

#include <stdio.h>

#ifdef UNIX
#include <malloc.h>
#endif

#define GG_H 1

/*********************/
/* 2d geometry types */
/*********************/

typedef struct Point2Struct {   /* 2d point */
  float x, y;
} Point2;
typedef Point2 Vector2;

typedef struct IntPoint2Struct {        /* 2d integer point */
  int x, y;
} IntPoint2;

typedef struct Matrix3Struct {  /* 3-by-3 matrix */
  float element[3][3];
} Matrix3;

typedef struct Box2dStruct {            /* 2d box */
  Point2 min, max;
} Box2;


/*********************/
/* 3d geometry types */
/*********************/

typedef struct Point3Struct {   /* 3d point */
  float x, y, z;
} Point3;
typedef Point3 Vector3;

typedef struct IntPoint3Struct {        /* 3d integer point */
  int x, y, z;
} IntPoint3;


typedef struct Matrix4Struct {  /* 4-by-4 matrix */
  float element[4][4];
} Matrix4;

typedef struct Box3dStruct {            /* 3d box */
  Point3 min, max;
} Box3;



/***********************/
/* one-argument macros */
/***********************/

/* absolute value of a */
#define ABS(a)          (((a)<0) ? -(a) : (a))

/* round a to nearest integer towards 0 */
#define FLOOR(a)                ((a)>0 ? (int)(a) : -(int)(-a))

/* round a to nearest integer away from 0 */
#define CEILING(a) \
  ((a)==(int)(a) ? (a) : (a)>0 ? 1+(int)(a) : -(1+(int)(-a)))

/* round a to nearest int */
#define ROUND(a)        ((a)>0 ? (int)(a+0.5) : -(int)(0.5-a))          

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a)         (((a)<0) ? -1 : (a)>0 ? 1 : 0)  

/* take binary sign of a, either -1, or 1 if >= 0 */
#define SGN(a)          (((a)<0) ? -1 : 1)

/* shout if something that should be true isn't */
#define ASSERT(x) \
  if (!(x)) fprintf(stderr," Assert failed: x\n");

/* square a */
#define SQR(a)          ((a)*(a))       


/***********************/
/* two-argument macros */
/***********************/

/* find minimum of a and b */
#define MIN(a,b)        (((a)<(b))?(a):(b))     

/* find maximum of a and b */
#define MAX(a,b)        (((a)>(b))?(a):(b))     

/* swap a and b (see Gem by Wyvill) */
#define SWAP(a,b)       { a^=b; b^=a; a^=b; }

/* linear interpolation from l (when a=0) to h (when a=1)*/
/* (equal to (a*h)+((1-a)*l) */
#define LERP(a,l,h)     ((l)+(((h)-(l))*(a)))

/* clamp the input to the specified range */
#define CLAMP(v,l,h)    ((v)<(l) ? (l) : (v) > (h) ? (h) : v)


/****************************/
/* memory allocation macros */
/****************************/

/* create a new instance of a structure (see Gem by Hultquist) */
#define NEWSTRUCT(x)    (struct x *)(malloc((unsigned)sizeof(struct x)))

/* create a new instance of a type */
#define NEWTYPE(x)      (x *)(malloc((unsigned)sizeof(x)))


/********************/
/* useful constants */
/********************/

#ifndef PI
#define PI              3.141592        /* the venerable pi */
#endif
#define PITIMES2        6.283185        /* 2 * pi */
#define PIOVER2         1.570796        /* pi / 2 */
#define E               2.718282        /* the venerable e */
#define SQRT2           1.414214        /* sqrt(2) */
#define SQRT3           1.732051        /* sqrt(3) */
#define GOLDEN          1.618034        /* the golden ratio */
#define DTOR(x)         ((x)*0.017453)  /* convert degrees to radians */
#define RTOD(x)         ((x)*57.29578)  /* convert radians to degrees */


/************/
/* booleans */
/************/

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#define ON              1
#define OFF             0
typedef int boolean;                    /* boolean data type */
typedef boolean flag;                   /* flag data type */

float V2SquaredLength(Vector2 *a);
float V2Length(Vector2 *a);
Vector2 *V2Negate(Vector2 *v);
Vector2 *V2Normalize(Vector2 *v);
Vector2 *V2Scale(Vector2 *v, float newlen);
Vector2 *V2Add(Vector2 *a, Vector2  *b, Vector2 *c);
Vector2 *V2Sub(Vector2 *a, Vector2 *b, Vector2 *c);
float V2Dot(Vector2 *a, Vector2 *b);
Vector2 *V2Lerp(Vector2 *lo, Vector2 *hi, Vector2 *result, float alpha);
Vector2 *V2Combine (Vector2 *a, Vector2 *b, Vector2 *result, 
		    float ascl, float bscl);
Vector2 *V2Mul (Vector2 *a, Vector2  *b, Vector2 *result);
float V2DistanceBetween2Points(Point2 *a, Point2 *b);
Vector2 *V2MakePerpendicular(Vector2 *a, Vector2 *ap);
Vector2 *V2New(float x, float y);
Vector2 *V2Set(Vector2 *v, float x, float y);
Vector2 *V2Duplicate(Vector2 *a);
Point2 *V2MulPointByProjMatrix(Point2 *pin, Point2 *pout, Matrix3 *m);
Matrix3 *V2MatMul(Matrix3 *a, Matrix3 *b, Matrix3 *c);
Matrix3 *TransposeMatrix3(Matrix3 *a, Matrix3 *b);

float V3SquaredLength(Vector3 *a);
float V3Length(Vector3 *a);
Vector3 *V3Negate(Vector3 *v);
Vector3 *V3Normalize(Vector3 *v);
Vector3 *V3Scale(Vector3 *v, float newlen);
Vector3 *V3Add(Vector3 *a, Vector3 *b, Vector3 *c);
Vector3 *V3Sub(Vector3 *a, Vector3 *b, Vector3 *c);
float V3Dot(Vector3 *a, Vector3 *b); 
Vector3 *V3Lerp(Vector3 *lo, Vector3 *hi, float alpha, Vector3 *result);
Vector3 *V3Combine (Vector3 *a, Vector3 *b, Vector3 *result, 
		    float ascl, float bscl);
Vector3 *V3Mul (Vector3 *a, Vector3 *b, Vector3 *result);
float V3DistanceBetween2Points(Point3 *a, Point3 *b);
Vector3 *V3Cross(Vector3 *a, Vector3 *b, Vector3 *c);
Vector3 *V3Set(Vector3 *v, float x, float y, float z); 
Vector3 *V3New(float x, float y, float z);
Vector3 *V3Duplicate(Vector3 *a);
Point3 *V3MulPointByMatrix(Point3 *pin, Point3 *pout, Matrix3 *m);
Point3 *V3MulPointByProjMatrix(Point3 *pin, Point3 *pout, Matrix4 *m);
Matrix4 *V3MatMul(Matrix4 *a, Matrix4 *b, Matrix4 *c);
int gcd(int u, int v);
int quadraticRoots(float a, float b, float c, float *roots);
float RegulaFalsi(float (*f)(float), float left, float right);
float NewtonRaphson(float (*f)(float), float (*df)(float), float x);
float findroot(float left, float right, float tolerance, 
	       float (*f)(float), float (*df)(float));
void V2Print(Point2 *v);
void V3Print(Point3 *v); 

#endif /* GC_H */

/* end of file */
