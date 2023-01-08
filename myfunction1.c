#include <stdlib.h>
#include <stdbool.h>
#include "myfunction1.h"
#include "showBMP.h"
#include <string.h>

// define marco
#define MN (m * n)
#define MN3 ((MN<<1)+MN)
#define INDEX(i,j) ((i)*m+(j))
#define limit (m-1)
#define CLAMP(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

//global values
int kernelScale;
bool filter;
int kernel[3][3];

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
    current_pixel->red = CLAMP(sum.red, 0, 255);
    current_pixel->green = CLAMP(sum.green, 0, 255);
    current_pixel->blue = CLAMP(sum.blue, 0, 255);

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
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernel(int i, int j, pixel *src) {
    //Declaration of variables
    register int ii = i-1,jj=j-1,sumRed=0,sumBlue=0,sumGreen=0;
    int runI = i + 1,runJ = j + 1,iiTemp = ii,jjTemp = jj;
    int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
    int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0
    int min_row, min_col, max_row, max_col,kRow=0, kCol,weight;
    register int index1 = ii*m;
    pixel_sum sum;
    pixel current_pixel,temp;
    for(;ii<=runI;++ii){
        kCol = 0;
        // compute row index in kernel
        int* row = kernel[kRow];
        jj=jjTemp;
        for(;jj<=runJ;++jj){
            // apply kernel on pixel at [ii,jj]
            temp= src[index1+jj];
            weight = row[kCol];
            //sum_pixels_by_weight
            sumRed+= temp.red * weight;
            sumBlue+= temp.blue * weight;
            sumGreen += temp.green * weight;
            kCol+=1;
        }
        kRow+=1;
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
    if (filter) {
        pixel loop_pixel;
        for(;ii<=runI;++ii){
            jj=jjTemp;
            for(;jj<=runJ;++jj){
                loop_pixel = src[index1+jj];
                //calculate color sum in order noe to calculate it in every if condition
                colorSum =(loop_pixel.red & 0xff) +  (loop_pixel.green&0xff) + (loop_pixel.blue & 0xff);
                /* check if smaller than min or higher than max and update.if we leave the original condition,
                 * in the assembly translation it will be a condition with a jump and here it is a placement*/
                min_row = (colorSum <= min_intensity) ? ii : min_row;
                min_col = (colorSum <= min_intensity) ? jj : min_col;
                min_intensity = (colorSum <= min_intensity) ? colorSum : min_intensity;

                max_row = (colorSum > max_intensity) ? ii : max_row;
                max_col = (colorSum > max_intensity) ? jj : max_col;
                max_intensity = (colorSum > max_intensity) ? colorSum : max_intensity;
            }
            index1+=m;

        }
        // filter out min and max
        sum_pixels_by_weight(&sum, src[INDEX(min_row,min_col)]);
        sum_pixels_by_weight(&sum, src[INDEX(max_row,max_col)]);
    }

    // assign kernel's result to pixel at [i,j]
    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/*
* Apply the kernel over each pixel.
* Ignore pixels where the kernel exceeds bounds. These are pixels with row index smaller than 3=kernelSize/2 and/or
* column index smaller than 3/2
*/
void smooth(pixel *src, pixel *dst) {
    int i= 1,j; //kernel size always 3 and 3/2 in int is 1
    int index = m; // in order not to calculate dst index every time
    for (; i < limit ; ++i) { //limit = m-1 (which is dim - kernelSize / 2)
        j = 1;
        for (; j < limit; ++j) {
            dst[index+j] = applyKernel(i, j, src);
        }
        index+=m;
    }
}

void doConvolution(Image *image, int tmpKernel[3][3], int kernelScale1,bool filter1) {
    // initialization global vals
    kernelScale = kernelScale1;
    filter=filter1;
    unsigned long mn3 = MN3;
    //kernel(global)=tempKernel
    memcpy(kernel, tmpKernel, 36);
    pixel* pixelsImg = malloc(mn3);
    //charsToPixels
    memcpy(pixelsImg, image->data, mn3);
    smooth((pixel*)image->data, pixelsImg);
    //pixelsToChars
    memcpy(image->data, pixelsImg, mn3);
    free(pixelsImg);
}

