#include <stdlib.h>
#include <stdbool.h>
#include "myfunction1.h"
#include "showBMP.h"
#include <string.h>


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MN (m * n)
#define MN3 ((MN<<1)+MN)
#define INDEX(i,j) ((i)*m+(j))
#define limit (n-1)

/*
 * initialize_pixel_sum - Initializes all fields of sum to 0
 */
void initialize_pixel_sum(pixel_sum *sum) {
        sum->red =0;
        sum->green =0;
        sum->blue = 0;
}

/*
 * assign_sum_to_pixel - Truncates pixel's new value to match the range [0,255]
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum, int kernelScale) {
    // divide by kernel's weight
    float x = 1.0f/kernelScale;
    sum.red *= x;
    sum.green *= x;
    sum.blue *= x;
    // truncate each pixel's color values to match the range [0,255]
    current_pixel->red = (unsigned char) (MIN(MAX(sum.red, 0), 255));
    current_pixel->green = (unsigned char) (MIN(MAX(sum.green, 0), 255));
    current_pixel->blue = (unsigned char) (MIN(MAX(sum.blue, 0), 255));
}

/*
* sum_pixels_by_weight - Sums pixel values, scaled by given weight
*/
static void sum_pixels_by_weight(pixel_sum *sum, pixel p, int weight) {
    sum->red += p.red *weight;
    sum->green += p.green *weight;
    sum->blue += p.blue *weight;
}

/*
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernel(int i, int j, pixel *src, int kernel[3][3], int kernelScale, bool filter) {
    register int ii = i-1,jj=j-1;
    int runI = i + 1,runJ = j + 1,iiTemp = ii,jjTemp = jj;
    pixel_sum sum;
    pixel current_pixel;
    int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
    int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0
    int min_row, min_col, max_row, max_col,kRow, kCol;
    register int index = ii*m;
    initialize_pixel_sum(&sum);
    while (ii<=runI){
        // compute row index in kernel
        kRow = ii - i + 1;
        int* row = kernel[kRow];
        jj=jjTemp;
        while (jj<=runJ){
            // compute column index in kernel
            kCol = jj - j + 1;
            // apply kernel on pixel at [ii,jj]
            sum_pixels_by_weight(&sum, src[index+jj], row[kCol]);
            ++jj;
        }
        index+=m;
        ++ii;
    }
    ii = iiTemp;
    if (filter) {
        pixel loop_pixel;
        // find min and max coordinates
        while (ii<=runI){
            jj=jjTemp;
            while (jj<=runJ){
                // check if smaller than min or higher than max and update
                loop_pixel = src[INDEX(ii,jj)];
                int colorSum =(loop_pixel.red & 0xff) +  (loop_pixel.green&0xff) + (loop_pixel.blue & 0xff);
                if (colorSum <= min_intensity) {
                    min_intensity = colorSum;
                    min_row = ii;
                    min_col = jj;
                }
                if (colorSum > max_intensity) {
                    max_intensity = colorSum;
                    max_row = ii;
                    max_col = jj;
                }
                ++jj;
            }
            ++ii;
        }
        // filter out min and max
        sum_pixels_by_weight(&sum, src[INDEX(min_row,min_col)], -1);
        sum_pixels_by_weight(&sum, src[INDEX(max_row,max_col)], -1);
    }

    // assign kernel's result to pixel at [i,j]
    assign_sum_to_pixel(&current_pixel, sum, kernelScale);
    return current_pixel;
}

/*
* Apply the kernel over each pixel.
* Ignore pixels where the kernel exceeds bounds. These are pixels with row index smaller than 3=kernelSize/2 and/or
* column index smaller than 3/2
*/
void smooth(pixel *src, pixel *dst, int kernel[3][3], int kernelScale, bool filter) {
    register int i= 1; //kernel size always 3 and 3/2 in int is 1
    register int index = m;
    for (; i <limit ; ++i) {
        register int j = 1;
        for (; j < limit; ++j) {
            dst[index+j] = applyKernel(i, j, src, kernel, kernelScale, filter);
        }
        index+=m;
    }
}

void doConvolution(Image *image, int kernel[3][3], int kernelScale,bool filter) {
    pixel* pixelsImg = malloc(MN3);
    pixel* backupOrg = malloc(MN3);
    /*chars To Pixels*/
    memcpy(pixelsImg, image->data, MN3);
    /*copy pixels*/
    memcpy(backupOrg, pixelsImg, MN3);
    smooth(backupOrg, pixelsImg, kernel, kernelScale, filter);
    /*Pixels To chars*/
    memcpy(image->data, pixelsImg, MN3);
    free(pixelsImg);
    free(backupOrg);
}

