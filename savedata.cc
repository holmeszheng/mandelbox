#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "color.h"
#include "hitdata.h"

void savedata(const char* filename, pixelData *pix_data, int n, hitdata hitdata_min, hitdata hitdata_max){

	FILE *f;
	int i;
	f = fopen(filename,"wb");
	fprintf(f,"------hitdata_min:\thitpoint:%0.3lf,%0.3lf,%0.3lf\t\tdistance:%0.3lf\n",hitdata_min.x,hitdata_min.y,hitdata_min.z, hitdata_min.distance);
    fprintf(f,"------hitdata_max:\thitpoint:%0.3lf,%0.3lf,%0.3lf\t\tdistance:%0.3lf\n",hitdata_max.x,hitdata_max.y,hitdata_max.z, hitdata_max.distance);
	for(i=0; i<n; i++)
	{
        if(pix_data[i].escaped==false)
        {
            fprintf(f, "hitpoint:%0.3lf,%0.3lf,%0.3lf\t\tdistance:%0.3lf\n",pix_data[i].hit.x,pix_data[i].hit.y,pix_data[i].hit.z,pix_data[i].distance);
        }
	}
	fclose(f);
}

