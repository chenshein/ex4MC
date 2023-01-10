#include <stdlib.h>
#include <stdbool.h>
#include "myfunction1.h"
#include "showBMP.h"
#include <string.h>

// define marco
#define MN (m * n) //define marco to multiply m and n, MN is used in some other macros
#define MN3 ((MN<<1)+MN) //define marco for MN3, it used for malloc
#define INDEX(i,j) ((i)*m+(j)) //define marco to calculate the index of a pixel given its i and j
#define limit (m-1)  //define marco limit as m-1
#define CLAMP(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

//global values
int kernelScale;
bool filter;

/*
 * assign_sum_to_pixel - Truncates pixel's new value to match the range [0,255]
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) {
    // divide by kernel's weight
    float x = 1.0f/kernelScale;
    sum.red *= x;
    sum.green *= x;
    sum.blue *= x;
    // truncate each pixel's color values to match the range [0,255]
    current_pixel->red = CLAMP(sum.red, 0, 255);// clamp the red value of the current pixel in the range [0, 255]
    current_pixel->green = CLAMP(sum.green, 0, 255);// clamp the green value of the current pixel in the range [0, 255]
    current_pixel->blue = CLAMP(sum.blue, 0, 255);// clamp the blue value of the current pixel in the range [0, 255]

}

/*
* sum_pixels_by_weight - Sums pixel values, scaled by given weight which is -1
*/
static void sum_pixels_by_weight(pixel_sum *sum, pixel p) {
    sum->red -= p.red;
    sum->green -= p.green;
    sum->blue -= p.blue;
}

/*
 * kernel =[0, 0, 0]
 *         [-2, 6, -2]
 *         [0, 0, 0]
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernelRowSharp(int i, int j, pixel *src) {
    //Declaration of variables
    register int ii = i-1,jj=j-1,sumRed,sumBlue=0,sumGreen=0;
    int index1 = ii*m+m;// calculate the index of the first pixel in the kernel
    pixel_sum sum;
    pixel current_pixel,temp;

    // goes throw kernel to calcu weight in order to insert red,blue,green in sum
    temp= src[index1+jj];
    //sum_pixels_by_weight
    sumRed += temp.red * (-2);
    sumBlue += temp.blue * (-2);
    sumGreen += temp.green * (-2);
    temp= src[index1+jj+1];
    //sum_pixels_by_weight
    sumRed += temp.red * 6;
    sumBlue += temp.blue * 6;
    sumGreen += temp.green * 6;
    temp= src[index1+jj+2];
    //sum_pixels_by_weight
    sumRed += temp.red * (-2);
    sumBlue += temp.blue * (-2);
    sumGreen += temp.green * (-2);

    //apply sum of the color to sum
    sum.red=sumRed;
    sum.blue=sumBlue;
    sum.green=sumGreen;
    // assign kernel's result to pixel at [i,j]
    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/*
 * kernel = [-1, -1, -1]
 *          [-1, 9, -1]
 *          [-1, -1, -1]
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernelSharp(int i, int j, pixel *src) {
    pixel_sum sum;
    int index1 = (i-1)*m;
    pixel current_pixel,temp;
    // goes throw kernel to calcu weight in order to insert red,blue,green in sum
    temp = src[index1+j-1];
    //sum_pixels_by_weight
    sum.red -= temp.red;
    sum.blue -= temp.blue ;
    sum.green -= temp.green;
    temp = src[index1+j];
    //sum_pixels_by_weight
    sum.red-= temp.red;
    sum.blue-= temp.blue;
    sum.green-= temp.green;
    temp = src[index1+j+1];
    //sum_pixels_by_weight
    sum.red-= temp.red ;
    sum.blue-= temp.blue;
    sum.green-= temp.green;
    index1+=m;
    temp = src[index1+j-1];
    //sum_pixels_by_weight
    sum.red-= temp.red ;
    sum.blue-= temp.blue ;
    sum.green-= temp.green ;
    temp = src[index1+j];
    //sum_pixels_by_weight
    sum.red+= temp.red * 9;
    sum.blue+= temp.blue * 9;
    sum.green+= temp.green * 9;
    temp = src[index1+j+1];
    //sum_pixels_by_weight
    sum.red-= temp.red ;
    sum.blue-= temp.blue ;
    sum.green-= temp.green ;
    index1+=m;
    temp = src[index1+j-1];
    //sum_pixels_by_weight
    sum.red-= temp.red ;
    sum.blue-= temp.blue ;
    sum.green-= temp.green ;
    temp = src[index1+j];
    //sum_pixels_by_weight
    sum.red-= temp.red ;
    sum.blue-= temp.blue ;
    sum.green-= temp.green;
    temp = src[index1+j+1];
    //sum_pixels_by_weight
    sum.red-= temp.red;
    sum.blue-= temp.blue;
    sum.green-= temp.green;

    // assign kernel's result to pixel at [i,j]
    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;

}


/*
 * kernel = [0, 0, 0]
            [1, 2, 1]
            [0, 0, 0]
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernelRowBlur(int i, int j, pixel *src) {
    //Declaration of variables
    register int jj=j-1,sumRed=0,sumBlue=0,sumGreen=0;
    register int index1 = (i-1)*m+m;
    pixel_sum sum;
    pixel current_pixel,temp;
// goes throw kernel to calcu weight in order to insert red,blue,green in sum
    temp = src[index1+jj];
    //sum_pixels_by_weight
    sumRed += temp.red * 1;
    sumBlue += temp.blue * 1;
    sumGreen += temp.green * 1;
    temp = src[index1+jj+1];
    //sum_pixels_by_weight
    sumRed += temp.red * 2;
    sumBlue += temp.blue * 2;
    sumGreen += temp.green * 2;
    temp = src[index1+jj+2];
    //sum_pixels_by_weight
    sumRed += temp.red * 1;
    sumBlue += temp.blue * 1;
    sumGreen += temp.green * 1;

    //apply sum of the color to sum
    sum.red=sumRed;
    sum.blue=sumBlue;
    sum.green=sumGreen;
    // assign kernel's result to pixel at [i,j]
    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}


/*
 * kernel = [1, 1, 1]
            [1, 1, 1]
            [1, 1, 1]
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernel(int i, int j, pixel *src) {
    //Declaration of variables
    register int ii = i-1,jj=j-1,sumRed=0,sumBlue=0,sumGreen=0;
    int runI = i + 1,runJ = j + 1,iiTemp = ii,jjTemp = jj;
    int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
    int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0
    int min_row, min_col, max_row, max_col;
    register int index1 = ii*m;
    pixel_sum sum;
    pixel current_pixel,temp;
    for(;ii<=runI;++ii){
        // compute row index in kernel
        jj=jjTemp;
        for(;jj<=runJ;++jj){
            //apply kernel on pixel at [ii,jj]
            temp= src[index1+jj];
            //sum_pixels_by_weight(weight=1, so we sum)
            sumRed+= temp.red;
            sumBlue+= temp.blue;
            sumGreen += temp.green;
        }
        index1+=m;
    }
    //apply sum of the color to sum
    sum.red=sumRed;
    sum.blue=sumBlue;
    sum.green=sumGreen;
    // initialization for 'for' loop
    ii = iiTemp;
    index1 = ii*m;
    int colorSum;
    if(filter) {
        pixel loop_pixel;
        for (; ii <= runI; ++ii) {
            jj = jjTemp;
            for (; jj <= runJ; ++jj) {
                loop_pixel = src[index1 + jj];
                //calculate color sum in order noe to calculate it in every if condition
                colorSum = (loop_pixel.red & 0xff) + (loop_pixel.green & 0xff) + (loop_pixel.blue & 0xff);
                /* check if smaller than min or higher than max and update.if we leave the original condition,
                * in the assembly translation it will be a condition with a jump and here it is a placement*/
                min_row = (colorSum <= min_intensity) ? ii : min_row;
                min_col = (colorSum <= min_intensity) ? jj : min_col;
                min_intensity = (colorSum <= min_intensity) ? colorSum : min_intensity;

                max_row = (colorSum > max_intensity) ? ii : max_row;
                max_col = (colorSum > max_intensity) ? jj : max_col;
                max_intensity = (colorSum > max_intensity) ? colorSum : max_intensity;
            }
            index1 += m;

        }
        // filter out min and max
        sum_pixels_by_weight(&sum, src[INDEX(min_row, min_col)]);
        sum_pixels_by_weight(&sum, src[INDEX(max_row, max_col)]);
    }
    // assign kernel's result to pixel at [i,j]
    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}


/*
* Apply the kernel over each pixel.
* Ignore pixels where the kernel exceeds bounds. These are pixels with row index smaller than 3=kernelSize/2 and/or
* column index smaller than 3/2
 * for case 1 -rowSharpKernel
 * case 2 -sharpKernel
 * case 3- rowBlurKernel
 * case 4- blurKernel
 * I separated each kernel because there are matrices that have 2 zero rows and there is no need to include them
 * in the function, there is a function where everything is unity ,and it is faster to sum than multiply by
 * one and then sum
*/
void smooth(pixel *src, pixel *dst,int flag) {
    int i= 1,j; //kernel size always 3 and 3/2 in int is 1
    int index = m; // in order not to calculate dst index every time
    switch (flag) {
        case 1:
            for (; i < limit ; ++i) { //limit = m-1 (which is dim - kernelSize / 2)
                j = 1;
                for (; j < limit; ++j) {
                    dst[index+j] = applyKernelRowSharp(i, j, src);
                }
                index+=m;
            }
        case 2:
            for (; i < limit ; ++i) { //limit = m-1 (which is dim - kernelSize / 2)
                j = 1;
                for (; j < limit; ++j) {
                    dst[index+j] = applyKernelSharp(i, j, src);
                }
                index+=m;
            }
        case 3:
            for (; i < limit ; ++i) { //limit = m-1 (which is dim - kernelSize / 2)
                j = 1;
                for (; j < limit; ++j) {
                    dst[index+j] = applyKernelRowBlur(i, j, src);
                }
                index+=m;
            }
        case 4:
            for (; i < limit ; ++i) { //limit = m-1 (which is dim - kernelSize / 2)
                j = 1;
                for (; j < limit; ++j) {
                    dst[index+j] = applyKernel(i, j, src);
                }
                index+=m;
            }
        default:
            break;
    }
}

void doConvolution(Image *image, int flag, int kernelScale1,bool filter1) {
    // initialization global vals
    kernelScale = kernelScale1;
    filter=filter1;
    unsigned long mn3 = MN3;
    pixel* pixelsImg = malloc(mn3);
    //charsToPixels
    memcpy(pixelsImg, image->data, mn3);
    smooth((pixel*)image->data, pixelsImg,flag);
    //pixelsToChars
    memcpy(image->data, pixelsImg, mn3);
    free(pixelsImg);
}