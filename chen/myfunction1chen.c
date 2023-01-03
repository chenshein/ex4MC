#include <stdlib.h>
#include <stdbool.h>
#include "myfunction1chen.h"
#include "showBMP.h"
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CUL (m * n * sizeof(pixel))
#define dim m

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
    //TODO make sure because i,j is always > 0. was before-> int ii = MAX(i-1, 0),iiTemp = ii,jj= MAX(j-1, 0),jjTemp = jj;
    int ii = i-1,iiTemp = ii,jj=j-1,jjTemp = jj;
    int runI = MIN(i + 1, dim - 1),runJ = MIN(j + 1, dim - 1);
    pixel_sum sum;
    pixel current_pixel;
    int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
    int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0
    int min_row, min_col, max_row, max_col,kRow, kCol;
    initialize_pixel_sum(&sum);
    while (ii<=runI){
        int* row = kernel[ii];
        // compute row index in kernel
        kRow = ii - i + 1;
        jj=jjTemp;
        while (jj<=runJ){
            // compute column index in kernel
            kCol = jj - j + 1;
            // apply kernel on pixel at [ii,jj]
            sum_pixels_by_weight(&sum, src[ii*dim+jj], kernel[kRow][kCol]);
            ++jj;
        }
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
                loop_pixel = src[ii*dim+jj];
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
        sum_pixels_by_weight(&sum, src[min_row*dim+min_col], -1);
        sum_pixels_by_weight(&sum, src[max_row*dim+ max_col], -1);
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
    int i= 1; //kernel size always 3 and 3/2 in int is 1
    int rumLoop = dim - i;
    while (i < rumLoop){
        int j=1;
        while (j< rumLoop){
            dst[i*dim+j] = applyKernel(i, j, src,kernel, kernelScale, filter);
            ++j;
        }
        ++i;
    }
}

void charsToPixels(pixel* pixels) {
    char *data1 = image->data;
    int row= 0, col ,index,index3,rn=0;
    for (row ; row < m ; ++row) {
        col =0;
        for (col; col < n ; ++col) {
            index =rn + col;
            index3= index + index +index;
            (*(pixels + index)).red = *(data1+index3);
            (*(pixels + index)).green = *(data1+(index3+1));
            (*(pixels + index)).blue = *(data1+(index3+2));
        }
        rn += n;
    }
}

void pixelsToChars(pixel* pixels, Image *charsImg) {
    char *data1 = image->data;
    int row= 0, col ,index,index3,rn=0;
    for (row ; row < m ; ++row) {
        col = 0;
        for (col; col < n ; ++col) {
            index =rn + col;
            index3= index + index +index;
            *(data1+index3) = (*(pixels + index)).red;
            *(data1+(index3+1)) = (*(pixels + index)).green;
            *(data1+(index3+2)) = (*(pixels + index)).blue;
        }
        rn += n;
    }
}


void copyPixels(pixel* src, pixel* dst) {
    memcpy(dst, src, CUL);
}

void doConvolution(Image *image, int kernel[3][3], int kernelScale, bool filter) {
    pixel* pixelsImg = malloc(CUL);
    pixel* backupOrg = malloc(CUL);
    charsToPixels(pixelsImg);
    copyPixels(pixelsImg, backupOrg);
    smooth(backupOrg, pixelsImg, kernel, kernelScale, filter);
    pixelsToChars(pixelsImg, image);
    free(pixelsImg);
    free(backupOrg);
}
