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
#include <stdio.h>

#include "color.h"
#include "mandelbox.h"
#include "camera.h"
#include "vector3d.h"
#include "3d.h"

#ifdef _OPENMP
#include <omp.h>
#else
#include <openacc.h>
#endif

extern double getTime();
extern void   printProgress( double perc, double time );

extern void rayMarch (const RenderParams &render_params, const vec3 &from, 
    const vec3  &to, double eps, pixelData &pix_data, const MandelBoxParams& mPar);
#pragma acc routine seq
extern void getColour(const pixelData &pixData, const RenderParams &render_params,
	       const vec3 &from, const vec3  &direction, const ColorData& color_data,
               vec3& hitColor);
#pragma acc routine seq
extern int UnProject(double winX, double winY, const CameraParams& camP, double *obj);

void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params, 
                     unsigned char* image, const MandelBoxParams& mPar)
{
  ColorData       color_data;
  VEC(color_data.CamLight,  1.0, 1.0, 1.0);
  VEC(color_data.baseColor, 1.0, 1.0, 1.0);
  VEC(color_data.backColor, 0.4, 0.4, 0.4);
  color_data.CamLightW   = 1.8;
  color_data.CamLightMin = 0.3;

  const double eps = pow(10.0, renderer_params.detail); 
  
  //from.SetDoublePoint(camera_params.camPos);
  vec3 from;
  SET_POINT(from, camera_params.camPos);
  
  const int height = renderer_params.height;
  const int width  = renderer_params.width;
  
  
  double time = getTime();
  struct timeval start, end;
  gettimeofday(&start, NULL);
#ifdef _OPENMP
  double Wtime = omp_get_wtime();
#endif
  
  int j,i;
  #pragma omp parallel \
  shared(from, time, image, start, end) \
  private(j,i)
  for(j = 0; j < height; j++)
    {
      //for each column pixel in the row
      #pragma omp for schedule(static,1) nowait
      for(i = 0; i <width; i++)
	{
	  // get point on the 'far' plane
	  // since we render one frame only, we can use the more specialized method
          double farPoint[3];
	  UnProject(i, j, camera_params, farPoint);
	  
	  // to = farPoint - camera_params.camPos
          vec3 to;
          SUBTRACT_POINT(to, farPoint, camera_params.camPos);
          NORMALIZE(to);
	  
          pixelData pix_data;
	  //render the pixel
	  rayMarch(renderer_params, from, to, eps, pix_data, mPar);
	  
	  //get the colour at this pixel
	  vec3 color;
          getColour(pix_data, renderer_params, from, to, color_data, color);
      
	  //save colour into texture
	  int k = (j * width + i)*3;
	  image[k+2] = (unsigned char)(color.x * 255);
	  image[k+1] = (unsigned char)(color.y * 255);
	  image[k]   = (unsigned char)(color.z * 255);
	}
      //#pragma omp single
      //{
      //  //printProgress((j+1)/(double)height,getTime()-time);
      //  gettimeofday(&end, NULL);
      //  printProgress((j+1)/(double)height,getWtime(start, end));
      //}
    }
#ifdef _OPENMP
  Wtime = omp_get_wtime() - Wtime;
  printf("\n rendering done: CPU time %.2e  Wall time %.2e\n", getTime()-time, Wtime);
#else
  printf("\n rendering done: CPU time %.2e\n", getTime()-time);
#endif
}

