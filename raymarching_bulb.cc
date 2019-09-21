/*
   This file is part of the Mandelbox program developed for the course
    CS/SE  Distributed Computer Systems taught by N. Nedialkov in the
    Winter of 2015-2016 at McMaster University.

    Copyright (C) 2015-2016 T. Gwosdz and N. Nedialkov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <assert.h>
#include <stdio.h>

#include "color.h"
#include "renderer.h"
#include "mandelbox.h"

#ifdef _OPENACC
#include <accelmath.h>
#else
#include <math.h>
#endif

inline double DE(const vec3 &p0, const MandelBoxParams &params)
{
  vec3 z;
  z = p0;

  double dr = 1.0;
  double r = 0.0;

  double Bailout = params.rMin;
  double Power = params.rFixed;

  for (int i=0; i < params.num_iter; i++)
    {
      MAGNITUDE(r,z);
      if(r > Bailout) break;

      double theta = acos(z.z/r);
      double phi   = atan2(z.y, z.x);
      dr = pow(r, Power - 1.0) * Power * dr + 1.0;

      double zr = pow(r, Power);
      theta     = theta * Power;
      phi       = phi * Power;

      z.x = zr*sin(theta)*cos(phi);
      z.y = zr*sin(phi)*sin(theta);
      z.z = zr*cos(theta);

      z.x = z.x + p0.x;
      z.y = z.y + p0.y;
      z.z = z.z + p0.z;
    }

  return 0.5*log(r)*r/dr;
}

//void normal(const vec3 & p, vec3 & normal, const MandelBoxParams &mPar)
#define NORMAL(p, normal, mPar) { \
  const double sqrt_mach_eps = 1.4901e-08; \
  double m; \
  MAGNITUDE(m,p) \
  double eps = MAX( m, 1.0 )*sqrt_mach_eps; \
  vec3 e1; \
  VEC(e1, eps, 0, 0) \
  vec3 e2; \
  VEC(e2, 0, eps, 0) \
  vec3 e3; \
  VEC(e3, 0, 0,  eps) \
  vec3 p1, n1, p2, n2, p3, n3; \
  VEC_ADD(p1, p, e1) \
  VEC_ADD(p2, p, e2) \
  VEC_ADD(p3, p, e3) \
  VEC_SUB(n1, p, e1) \
  VEC_SUB(n2, p, e2) \
  VEC_SUB(n3, p, e3) \
  VEC(normal, DE(p1, mPar)-DE(n1, mPar), DE(p2, mPar)-DE(n2, mPar), DE(p3, mPar)-DE(n3, mPar)) \
  NORMALIZE(normal) \
}

#pragma acc routine seq
void rayMarch(const RenderParams &render_params, const vec3 &from, 
     const vec3  &direction, double eps, const MandelBoxParams& mPar,
     pixelData& pix_data)
{
  double dist = 0.0;
  double totalDist = 0.0;
  
  // We will adjust the minimum distance based on the current zoom

  double epsModified = 0.0;
  
  int steps=0;
  vec3 p;
  do 
    {      
      //p = from + direction * totalDist;
      VEC_ADD_SCL(p, from, direction, totalDist)
      dist = DE(p, mPar);
      
      totalDist += .95*dist;
      
      epsModified = totalDist;
      epsModified*=eps;
      steps++;
    }
  while (dist > epsModified && totalDist <= render_params.maxDistance && steps < render_params.maxRaySteps);
  
  //vec3 hitNormal;
  if (dist < epsModified) 
    {
      //we didnt escape
      pix_data.escaped = false;
      
      // We hit something, or reached MaxRaySteps
      pix_data.hit = p;
      
      //figure out the normal of the surface at this point
      vec3 normPos;// = p - direction * epsModified;
      VEC_ADD_SCL(normPos, p, direction, -epsModified)
      NORMAL(normPos, pix_data.normal, mPar);
    }
  else {
    //we have the background colour
    pix_data.escaped = true;
  }
}
