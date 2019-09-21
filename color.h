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
#ifndef COLOR_H
#define COLOR_H

#include "vector3d.h"

typedef struct
{
  bool escaped;
  vec3 hit;
  vec3 normal;
  double distance;
} pixelData;

typedef struct
{
//---lightning and colouring---------
  vec3 CamLight;// = {1.0,1.0,1.0};
  double CamLightW;// = 1.8;// 1.27536;
  double CamLightMin;// = 0.3;// 0.48193;
//-----------------------------------
  vec3 baseColor;// = {1.0, 1.0, 1.0};
  vec3 backColor;// = {0.4,0.4,0.4};
//-----------------------------------
} ColorData;

#endif
