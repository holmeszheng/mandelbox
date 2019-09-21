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

#define ROTATE_STEP 0.02

void saveBMP      (const char* filename, const unsigned char* image, int width, int height);

void init3D       (CameraParams *camera_params, const RenderParams *renderer_params);
void renderFractal(const CameraParams &camera_params, const RenderParams &renderer_params,
                   const MandelBoxParams& mandelBox_params, unsigned char* image);

void smooth_move(CameraParams &camera_params, RenderParams &renderer_params, MandelBoxParams &mandelBox_params,
        hitdata &hitdata_max, vec3 &direction, unsigned char *image , int &img_no)
{
    struct timeval start, end;
    char filename[30];


    double old_target[3];
    old_target[0] = camera_params.camTarget[0];
    old_target[1] = camera_params.camTarget[1];
    old_target[2] = camera_params.camTarget[2];

    double new_target[3];
    new_target[0] = hitdata_max.x;
    new_target[1] = hitdata_max.y;
    new_target[2] = hitdata_max.z;

    double camera_position[3];
    camera_position[0] = camera_params.camPos[0];
    camera_position[1] = camera_params.camPos[1];
    camera_position[2] = camera_params.camPos[2];

    double old_direction[3];
    old_direction[0] = old_target[0] - camera_position[0];
    old_direction[1] = old_target[1] - camera_position[1];
    old_direction[2] = old_target[2] - camera_position[2];
    NormalizeVector(old_direction);

    double new_direction[3];
    new_direction[0] = new_target[0] - camera_position[0];
    new_direction[1] = new_target[1] - camera_position[1];
    new_direction[2] = new_target[2] - camera_position[2];
    NormalizeVector(new_direction);

    double old_up[3];
    old_up[0] = camera_params.camUp[0];
    old_up[1] = camera_params.camUp[1];
    old_up[2] = camera_params.camUp[2];
    double temp[3];
    //Side = forward x up
    ComputeNormalOfPlane(temp, old_direction, old_up);
    NormalizeVector(temp);
    //Recompute up as: up = side x forward
    ComputeNormalOfPlane(old_up, temp, old_direction);

    double new_up[3];
    new_up[0] = direction.x;
    new_up[1] = direction.y;
    new_up[2] = direction.z;
    //Side = forward x up
    ComputeNormalOfPlane(temp, new_direction, new_up);
    NormalizeVector(temp);
    //Recompute up as: up = side x forward
    ComputeNormalOfPlane(new_up, temp, new_direction);

    double direction_diff[3];
    direction_diff[0] = new_direction[0] - old_direction[0];
    direction_diff[1] = new_direction[1] - old_direction[1];
    direction_diff[2] = new_direction[2] - old_direction[2];

    double direction_dis;
    direction_dis = sqrt(direction_diff[0]*direction_diff[0]+direction_diff[1]*direction_diff[1]+direction_diff[2]*direction_diff[2]);

    double up_diff[3];
    up_diff[0] = new_up[0] - old_up[0];
    up_diff[1] = new_up[1] - old_up[1];
    up_diff[2] = new_up[2] - old_up[2];

    double up_dis;
    up_dis = sqrt(up_diff[0]*up_diff[0]+up_diff[1]*up_diff[1]+up_diff[2]*up_diff[2]);

    NormalizeVector(direction_diff);
    NormalizeVector(up_diff);

    int rotate_count = 0;
    double up_step = 0.0;
    double direction_step = 0.0;
    if(up_dis/ROTATE_STEP > direction_dis/ROTATE_STEP)
    {
        rotate_count = up_dis/ROTATE_STEP;
        up_step = up_dis/rotate_count;
        direction_step = direction_dis/rotate_count;
    }
    else
    {
        rotate_count = direction_dis/ROTATE_STEP;
        up_step = up_dis/rotate_count;
        direction_step = direction_dis/rotate_count;
    }
    //test
    printf("rotate_count:   %d  up_step:    %.3lf   direction_step: %.3lf\n",rotate_count, up_step, direction_step);
    for(int rotate_i = 1; rotate_i < rotate_count; rotate_i++)
    {
        double direction_temp[3];
        direction_temp[0] = old_direction[0] + direction_step*rotate_i*direction_diff[0];
        direction_temp[1] = old_direction[1] + direction_step*rotate_i*direction_diff[1];
        direction_temp[2] = old_direction[2] + direction_step*rotate_i*direction_diff[2];
        camera_params.camTarget[0] = camera_params.camPos[0] + 10*direction_temp[0];
        camera_params.camTarget[1] = camera_params.camPos[1] + 10*direction_temp[1];
        camera_params.camTarget[2] = camera_params.camPos[2] + 10*direction_temp[2];

        double up_temp[3];
        up_temp[0] = old_up[0] + up_step*rotate_i*up_diff[0];
        up_temp[1] = old_up[1] + up_step*rotate_i*up_diff[1];
        up_temp[2] = old_up[2] + up_step*rotate_i*up_diff[2];
        camera_params.camUp[0] = up_temp[0];
        camera_params.camUp[1] = up_temp[1];
        camera_params.camUp[2] = up_temp[2];

        memset(filename, 0, 30);
        sprintf(filename,"image%d.bmp",img_no);
        printf("camera_position:    x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camPos[0],camera_params.camPos[1],camera_params.camPos[2]);
        printf("camera_target:   x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camTarget[0], camera_params.camTarget[1], camera_params.camTarget[2]);
        printf("camera_Up:   x:%.3lf,y:%.3lf,z:%.3lf\n",camera_params.camUp[0], camera_params.camUp[1], camera_params.camUp[2]);

        init3D(&camera_params, &renderer_params);
        printf("  Rendering %dth  %d x %d image\n",img_no, renderer_params.width, renderer_params.height);
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
    }
}
