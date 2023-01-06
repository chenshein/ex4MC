#include <stdlib.h>
#include <stdbool.h>
#include "myfunction1.h"
#include "showBMP.h"
#include <string.h>


#define MN (m * n)
#define MN3 ((MN<<1)+MN)
#define INDEX(i,j) ((i)*m+(j))
#define limit (n-1)
#define CLAMP(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

int kernel[3][3];

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
    current_pixel->red = CLAMP(sum.red, 0, 255);
    current_pixel->green = CLAMP(sum.green, 0, 255);
    current_pixel->blue = CLAMP(sum.blue, 0, 255);

}

/*
* sum_pixels_by_weight - Sums pixel values, scaled by given weight
*/
static void sum_pixels_by_weight(pixel_sum *sum, pixel p, int weight) {
    sum->red += p.red * weight;
    sum->green += p.green * weight;
    sum->blue += p.blue * weight;
}
/*
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernel(int i, int j, pixel *src, int kernelScale, bool filter) {
    // Use local variables instead of global variables
    int ii = i-1, jj = j-1;
    int runI = i + 1, runJ = j + 1;
    int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
    int min_row, min_col, max_row, max_col,kRow,kCol,iim,iiRow;
    int sumRed=0, sumBlue=0, sumGreen=0;
    pixel_sum sum;
    pixel current_pixel;

    // Precompute the kernel sum
    for (kRow = 0; kRow < 3; ++kRow) {
        register int *row = kernel[kRow];
        register int calc = m*(ii+kRow)+jj;
        for (kCol = 0; kCol < 3; ++kCol) {
            register int weight = row[kCol];
            register pixel temp = src[calc+kCol];
            sumRed += temp.red * weight;
            sumBlue += temp.blue * weight;
            sumGreen += temp.green * weight;
        }
    }
    int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0
    sum.red = sumRed;
    sum.blue = sumBlue;
    sum.green = sumGreen;

    int index1 = ii*m;
    if (filter) {
        pixel loop_pixel;
        // find min and max coordinates
        for (ii = i - 1; ii <= runI; ++ii) {
            for (jj = j - 1; jj <= runJ; ++jj) {
                // check if smaller than min or higher than max and update
                loop_pixel = src[index1+jj];
                int colorSum = (loop_pixel.red & 0xff) + (loop_pixel.green & 0xff) + (loop_pixel.blue & 0xff);
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
            }
            index1+=m;
        }
        // filter out min and max
        sum_pixels_by_weight(&sum, src[INDEX(min_row, min_col)], -1);
        sum_pixels_by_weight(&sum, src[INDEX(max_row, max_col)], -1);
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
void smooth(pixel *src, pixel *dst, int kernelScale, bool filter) {
    int i= 1,j; //kernel size always 3 and 3/2 in int is 1
    int index = m;
    for (; i < limit ; ++i) {
        j = 1;
        for (; j < limit; ++j) {
            dst[index+j] = applyKernel(i, j, src, kernelScale, filter);
        }
        index+=m;
    }
}

void doConvolution(Image *image, int tmpKernel[3][3], int kernelScale,bool filter) {
    unsigned long mn3 = MN3;
    memcpy(kernel, tmpKernel, sizeof(int)*9);
    pixel* pixelsImg = malloc(mn3);
    memcpy(pixelsImg, image->data, mn3);
    smooth((pixel*)image->data, pixelsImg, kernelScale, filter);
    memcpy(image->data, pixelsImg, mn3);
    free(pixelsImg);
}

