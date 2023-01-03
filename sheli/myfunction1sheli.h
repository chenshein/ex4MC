//
// Created by chenshein on 1/1/23.
//
#include "readBMP.h"


#ifndef UNTITLED1_MYFUNCTION1_H
#define UNTITLED1_MYFUNCTION1_H
static pixel applyKernel(pixel *src, pixel *dst, int kernel[3][3], int kernelScale);
static pixel applyKernelWFilter(pixel *src, pixel *dst, int kernel[3][3], int kernelScale);
void doConvolution(Image *image, int kernel[3][3], int kernelScale);
void doConvolutionWFilter(Image *image, int kernel[3][3], int kernelScale);
#endif //UNTITLED1_MYFUNCTION1_H
