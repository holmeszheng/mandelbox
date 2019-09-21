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

#ifdef _OPENACC
#include <openacc.h>
#endif

#pragma acc routine seq
extern void rayMarch (const RenderParams &render_params, const vec3 &from,
    const vec3  &to, double eps, const MandelBoxParams& mPar, pixelData &pix_data);
#pragma acc routine seq
extern void getColour(const pixelData &pixData, const RenderParams &render_params,
	       const vec3 &from, const ColorData& color_data, vec3& hitColor);
#pragma acc routine seq
extern int UnProject(double winX, double winY, const CameraParams& camP, double *obj);

extern pixelData* pix_data;

void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params,
                   const MandelBoxParams& mPar, unsigned char* image)
{
  ColorData color_data;
  VEC(color_data.CamLight,  1.0, 1.0, 1.0);
  VEC(color_data.baseColor, 1.0, 1.0, 1.0);
  VEC(color_data.backColor, 0.4, 0.4, 0.4);
  color_data.CamLightW   = 1.8;
  color_data.CamLightMin = 0.3;

  const double eps = pow(10.0, renderer_params.detail);

  vec3 from;
  SET_POINT(from, camera_params.camPos);

  int height = renderer_params.height;
  int width  = renderer_params.width;
  int num_pixels = height * width * 3;
  double    *farPoint = (double    *) acc_malloc(num_pixels*sizeof(double));
  //pixelData *pix_data = (pixelData *) acc_malloc(width*height*sizeof(pixelData));
  vec3      *to       = (vec3      *) acc_malloc(width*height*sizeof(vec3));

  #pragma acc data \
  deviceptr(farPoint, to) \
  copyout(image[0:num_pixels], pix_data[0:height*width]) \
  copyin(from, eps) pcopyin(color_data) \
  copyin(mPar[0:1], camera_params[0:1], renderer_params[0:1])
  {
    #pragma acc parallel loop gang num_workers(4) vector_length(32)
    for(int j = 0; j < height; j++) {
      //for each column pixel in the row
      #pragma acc loop worker vector
      for(int i = 0; i <width; i++) {
        // get point on the 'far' plane
        // since we render one frame only, we can use the more specialized method
        int k = (j * width + i)*3;
        UnProject(i, j, camera_params, farPoint+k);

        // to = farPoint - camera_params.camPos
        SUBTRACT_POINT(to[j*width+i], (farPoint+k), camera_params.camPos);
        NORMALIZE(to[j*width+i]);

        //render the pixel
        rayMarch(renderer_params, from, to[j*width+i], eps, mPar, pix_data[j*width+i]);

        //get the colour at this pixel
        /*note to(in/out) is modified in place such that hitcolor is stored in to*/
        getColour(pix_data[j*width+i], renderer_params, from, color_data, to[j*width+i]);

        //save colour into texture
        image[k+2] = (unsigned char)(to[j*width+i].x * 255);
        image[k+1] = (unsigned char)(to[j*width+i].y * 255);
        image[k]   = (unsigned char)(to[j*width+i].z * 255);
      }
      //gettimeofday(&end, NULL);
      //printProgress((j+1)/(double)height,getWtime(start, end));
    }
  }

  acc_free(to);
  //acc_free(pix_data);
  acc_free(farPoint);
}
