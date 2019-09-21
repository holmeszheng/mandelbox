#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include "hitdata.h"
#include "camera.h"

extern pixelData* pix_data;
void savedata(const char* filename, pixelData *pix_data, int n, hitdata hitdata_min, hitdata hitdata_max);

void gethitdata(hitdata &hitdata_min, hitdata &hitdata_max, int image_size)
{
    hitdata_min.distance=1000.0;
    hitdata_max.distance=0.0;
    for(int i=0; i<image_size; i++)
    {
        if(pix_data[i].escaped==false)
        {
            if(pix_data[i].distance<hitdata_min.distance)
            {
                hitdata_min.distance = pix_data[i].distance;
                hitdata_min.x=pix_data[i].hit.x;
                hitdata_min.y=pix_data[i].hit.y;
                hitdata_min.z=pix_data[i].hit.z;
            }
            if(pix_data[i].distance>hitdata_max.distance)
            {
                hitdata_max.distance = pix_data[i].distance;
                hitdata_max.x=pix_data[i].hit.x;
                hitdata_max.y=pix_data[i].hit.y;
                hitdata_max.z=pix_data[i].hit.z;
            }
        }
    }
    savedata("pixdata.dat",pix_data,image_size, hitdata_min, hitdata_max);
}

void select_hitdata(hitdata &hitdata_current, hitdata &hitdata_selected, CameraParams &camera_params)
{
    hitdata_current.x=hitdata_selected.x;
    hitdata_current.y=hitdata_selected.y;
    hitdata_current.z=hitdata_selected.z;
    hitdata_current.distance=hitdata_selected.distance;
    camera_params.camTarget[0]=hitdata_selected.x;
    camera_params.camTarget[1]=hitdata_selected.y;
    camera_params.camTarget[2]=hitdata_selected.z;
}
