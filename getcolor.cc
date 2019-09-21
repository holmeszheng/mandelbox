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
#include "color.h"
#include "renderer.h"
#include "vector3d.h"


//---lightning and colouring---------
//static vec3 CamLight = {1.0,1.0,1.0};
//static double CamLightW = 1.8;// 1.27536;
//static double CamLightMin = 0.3;// 0.48193;
#define CamLight    color_data.CamLight
#define CamLightW   color_data.CamLightW
#define CamLightMin color_data.CamLightMin
//-----------------------------------
//static const vec3 baseColor = {1.0, 1.0, 1.0};
//static const vec3 backColor = {0.4,0.4,0.4};
#define baseColor color_data.baseColor
#define backColor color_data.backColor
//-----------------------------------

inline void lighting(const vec3 &n, const vec3 &color, const vec3 &pos, 
    const vec3 &direction, const ColorData& color_data, vec3 &outV)
{
  vec3 nn;
  SUBTRACT_VEC_DOUBLE(nn, n, 1.0);
  double d;
  DOT(d, nn, direction);
  double ambient = MAX( CamLightMin, d )*CamLightW;
  vec3 tv; 
  VEC_SCL(tv, CamLight, ambient);
  VEC_MUL(outV, tv, color);
}

//void getColour(const pixelData &pixData, const RenderParams &render_params,
//	       const vec3 &from, const vec3  &direction, const ColorData& color_data,
//               vec3& hitColor)
#pragma acc routine seq
void getColour(const pixelData &pixData, const RenderParams &render_params,
	       const vec3 &from, const ColorData& color_data,
               vec3& hitColor) //the initial hit color is direction
{
  const vec3& direction = hitColor; /* for readability */
  //colouring and lightning
  if (pixData.escaped == false) 
    {
      //apply lighting
      lighting(pixData.normal, baseColor, pixData.hit, direction, color_data, hitColor);
      
      //add normal based colouring
      if(render_params.colourType == 0 || render_params.colourType == 1)
	{
	  //hitColor = hitColor * pixData.normal;
          VEC_MUL(hitColor, hitColor, pixData.normal);
	  //hitColor = (hitColor + 1.0)/2.0;
          VEC_ADD_DOUBLE(hitColor, hitColor, 1.0);
          VEC_SCL(hitColor, hitColor, 0.5);
	  //hitColor = hitColor*render_params.brightness;
          VEC_SCL(hitColor, hitColor, render_params.brightness);
	  
	  //gamma correction
	  clamp(hitColor, 0.0, 1.0);
	  //hitColor = hitColor*hitColor;
          VEC_MUL(hitColor, hitColor, hitColor);
	}
      if(render_params.colourType == 1)
	{
	  //"swap" colors
	  double t = hitColor.x;
	  hitColor.x = hitColor.z;
	  hitColor.z = t;
	}
    }
  else 
    //we have the background colour
    hitColor = backColor;
  
  //return hitColor;
}
