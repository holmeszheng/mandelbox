#ifndef vec3_h
#define vec3_h

#ifdef _OPENACC
#include <accelmath.h>
#else
#include <math.h>
#endif


typedef struct 
{
  double x, y, z;
}  vec3;

#define SET_POINT(p,v) { p.x=v[0]; p.y=v[1]; p.z=v[2]; }

#define SUBTRACT_POINT(p,v,u)			\
{						\
  p.x=(v[0])-(u[0]);				\
  p.y=(v[1])-(u[1]);				\
  p.z=(v[2])-(u[2]);				\
}

/* vec3 - double */
#define SUBTRACT_VEC_DOUBLE(p,v,u)			\
{						\
  p.x=v.x-(u);				\
  p.y=v.y-(u);				\
  p.z=v.z-(u);				\
}

#define NORMALIZE(p) {					\
    double fMag = ( p.x*p.x + p.y*p.y + p.z*p.z );	\
    if (fMag != 0)					\
      {							\
	double fMult = 1.0/sqrt(fMag);			\
	p.x *= fMult;					\
	p.y *= fMult;					\
	p.z *= fMult;					\
      }							\
  }


#define MAGNITUDE(m,p) 	{ m=sqrt( p.x*p.x + p.y*p.y + p.z*p.z ); }

#define DOT(d,p,q) {  d=( p.x*q.x + p.y*q.y + p.z*q.z ); }

#define MAX(a,b) ( ((a)>(b))? (a):(b))

#define VEC(v,a,b,c) { v.x = (a); v.y = (b); v.z = (c); }

#define VEC_MUL(t,v1,v2) \
{                   \
  t.x = v1.x * v2.x; \
  t.y = v1.y * v2.y; \
  t.z = v1.z * v2.z; \
}

#define VEC_SCL(t,v,d) \
{                \
  t.x = v.x * (d); \
  t.y = v.y * (d); \
  t.z = v.z * (d); \
}

#define VEC_ADD_DOUBLE(t,v,d) \
{                  \
  t.x = v.x + (d); \
  t.y = v.y + (d); \
  t.z = v.z + (d); \
}


#define VEC_ADD(t,v1,v2) \
{                    \
  t.x = v1.x + v2.x; \
  t.y = v1.y + v2.y; \
  t.z = v1.z + v2.z; \
}

#define VEC_SUB(t,v1,v2) \
{                    \
  t.x = v1.x - v2.x; \
  t.y = v1.y - v2.y; \
  t.z = v1.z - v2.z; \
}

#define VEC_ADD_SCL(t,v1,v2,s) \
{                        \
  t.x = v1.x + v2.x*(s); \
  t.y = v1.y + v2.y*(s); \
  t.z = v1.z + v2.z*(s); \
}

inline double clamp(double d, double min, double max) 
{
  const double t = d < min ? min : d;
  return t > max ? max : t;
}


inline void clamp(vec3 &v, double min, double max) 
{
  v.x = clamp(v.x,min,max);
  v.y = clamp(v.y,min,max);
  v.z = clamp(v.z,min,max);
}

#endif
