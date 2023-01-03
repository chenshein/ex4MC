
#ifndef __myfunc1_h
#define __myfunc1_h

#include "myutil.h"
#include "readBMP.h"

void initialize_pixel_sum(pixel_sum *sum);
void doConvolution(Image *image, int kernel[3][3], int kernelScale,bool filter);



#endif

