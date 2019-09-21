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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "camera.h"
#include "renderer.h"
#include "mandelbox.h"
#include "3d.h"
#include "color.h"
#include "hitdata.h"
#include "vector3d.h"

#define MAX_IMG_NO 1000
#define FORWARD_STEP 0.02
#define COLLISION_FACTOR 1.5
void getParameters(char *filename, CameraParams *camera_params, RenderParams *renderer_params,
                   MandelBoxParams *mandelBox_paramsP);
void saveBMP      (const char* filename, const unsigned char* image, int width, int height);

void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params,
                   const MandelBoxParams& mandelBox_params, unsigned char* image);

void savedata(const char* filename, pixelData *pix_data, int n, hitdata hitdata_min, hitdata hitdata_max);
void gethitdata(hitdata &hitdata_min, hitdata &hitdata_max, int image_size);
void select_hitdata(hitdata &hitdata_current, hitdata &hitdata_selected, CameraParams &camera_params);
void smooth_move(CameraParams &camera_params, RenderParams &renderer_params, MandelBoxParams &mandelBox_params,
        hitdata &hitdata_max, vec3 &direction, unsigned char *image , int &img_no);
extern pixelData *pix_data = NULL;

int main(int argc, char** argv)
{
    double time = getTime();
    struct timeval start_all,start, end;
    int img_no = 0;

    CameraParams    camera_params;
    RenderParams    renderer_params;
    MandelBoxParams mandelBox_params;

    char filename[30];

    getParameters(argv[1], &camera_params, &renderer_params, &mandelBox_params);

    int image_size = renderer_params.width * renderer_params.height;
    unsigned char *image = (unsigned char*)malloc(3*image_size*sizeof(unsigned char));
    pix_data = (pixelData *) malloc(image_size*sizeof(pixelData));

    hitdata hitdata_min, hitdata_max, hitdata_current;

    vec3 camp_former;
    vec3 camp_current;
    vec3 direction;

    double step = FORWARD_STEP;
    bool ct_flag = false;  //change target flag
    //first frame
    memset(filename, 0, 30);
    sprintf(filename,"image%d.bmp",img_no);
    printf("camera_position:    x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2]);
    printf("camera_target:   x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camTarget[0], camera_params.camTarget[1], camera_params.camTarget[2]);
    printf("camera_Up:   x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camUp[0], camera_params.camUp[1], camera_params.camUp[2]);

    init3D(&camera_params, &renderer_params);
    printf("  Rendering %dth  %d x %d image\n",img_no, renderer_params.width, renderer_params.height);
    printf("  ------------------------------\n");
    /* start timing */
    gettimeofday(&start_all, NULL);

    renderFractal(camera_params, renderer_params, mandelBox_params, image);
    /* timing progress*/
    gettimeofday(&end, NULL);
    printf("  %dth frame done: Wall time %.2e\n\n", img_no, getWtime(start_all, end));

    //saveBMP(renderer_params.file_name, image, renderer_params.width, renderer_params.height);
    saveBMP(filename, image, renderer_params.width, renderer_params.height);
    img_no++;

    SUBTRACT_POINT(direction, camera_params.camTarget, camera_params.camPos);
    NORMALIZE(direction);

    //gethitdata
    gethitdata(hitdata_min, hitdata_max, image_size);
    //camera smooth move
    smooth_move(camera_params, renderer_params, mandelBox_params, hitdata_max, direction, image, img_no);
    //change camera_target
    select_hitdata(hitdata_current, hitdata_max, camera_params);
    //change camUp
    camera_params.camUp[0] = direction.x;
    camera_params.camUp[1] = direction.y;
    camera_params.camUp[2] = direction.z;
    //set ct_flag
    ct_flag = true;

    //frame iterations
    for(; img_no<MAX_IMG_NO; )
    {
        if(hitdata_current.distance < COLLISION_FACTOR)
        {
            //gethitdata
            gethitdata(hitdata_min, hitdata_max, image_size);
            //camera smooth move
            smooth_move(camera_params, renderer_params, mandelBox_params, hitdata_max, direction, image, img_no);
            //change camera_target
            select_hitdata(hitdata_current, hitdata_max, camera_params);
            //change camUp
            camera_params.camUp[0] = direction.x;
            camera_params.camUp[1] = direction.y;
            camera_params.camUp[2] = direction.z;
            //set ct_flag
            ct_flag = true;
        }
        memset(filename, 0, 30);
        sprintf(filename,"image%d.bmp",img_no);
        if(ct_flag == false)
        {
            //get current camer position
            SET_POINT(camp_former,camera_params.camPos)

            //get direction
            SUBTRACT_POINT(direction, camera_params.camTarget, camera_params.camPos);
            NORMALIZE(direction);

            //camp_current =  camp_former + direction * step
            VEC_ADD_SCL(camp_current, camp_former, direction, step)
            hitdata_current.distance -= step;

            camera_params.camPos[0]=camp_current.x;
            camera_params.camPos[1]=camp_current.y;
            camera_params.camPos[2]=camp_current.z;
        }

        printf("-------%dth frame current camera position finished!\n", img_no);
        printf("camera_position:    x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2]);
        printf("camera_target:   x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camTarget[0], camera_params.camTarget[1], camera_params.camTarget[2]);
        printf("camera_Up:   x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camUp[0], camera_params.camUp[1], camera_params.camUp[2]);
        printf("distance:   %.3lf", hitdata_current.distance);

        init3D(&camera_params, &renderer_params);
        printf("  Rendering %dth  %d x %d image\n", img_no, renderer_params.width, renderer_params.height);
        printf("  ------------------------------\n");
        /* start timing */
        gettimeofday(&start, NULL);

        renderFractal(camera_params, renderer_params, mandelBox_params, image);
        /* timing progress*/
        gettimeofday(&end, NULL);
        printf("  %dth frame done: Wall time %.2e\n\n", img_no, getWtime(start, end));

        //saveBMP(renderer_params.file_name, image, renderer_params.width, renderer_params.height);
        saveBMP(filename, image, renderer_params.width, renderer_params.height);
        img_no++;
        ct_flag = false;
    }

    free(image);
    free(pix_data);
    /* end timing */
    gettimeofday(&end, NULL);
    printf("  Rendering done: CPU time %.2e  Wall time %.2e\n", getTime()-time, getWtime(start_all, end));
    printf("  ------------------------------\n");

    return 0;
}
