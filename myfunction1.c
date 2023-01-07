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
* sum_pixels_by_weight - Sums pixel values, scaled by given weight
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
    register int ii = i-1,jj=j-1;
    int runI = i + 1,runJ = j + 1,iiTemp = ii,jjTemp = jj;
    int min_intensity = 766; // arbitrary value that is higher than maximum possible intensity, which is 255*3=765
    int max_intensity = -1; // arbitrary value that is lower than minimum possible intensity, which is 0
    int min_row, min_col, max_row, max_col,kRow, kCol;
    register int index1 = ii*m;
    register int sumRed=0,sumBlue=0,sumGreen=0;
    pixel_sum sum;
    pixel current_pixel;
    kRow = 0;
    for(;ii<=runI;++ii){
        kCol = 0;
        // compute row index in kernel
        int* row = kernel[kRow];
        jj=jjTemp;
        for(;jj<=runJ;++jj){
            // apply kernel on pixel at [ii,jj]
            pixel temp= src[index1+jj];
            int weight = row[kCol];
            sumRed+= temp.red * weight;
            sumBlue+= temp.blue * weight;
            sumGreen += temp.green * weight;
            kCol+=1;
        }
        kRow+=1;
        index1+=m;
    }
    sum.red=sumRed;
    sum.blue=sumBlue;
    sum.green=sumGreen;
    ii = iiTemp;
    index1 = ii*m;
    int colorSum;
    if (filter) {
        pixel loop_pixel;
        // find min and max coordinates
        for(;ii<=runI;++ii){
            jj=jjTemp;
            for(;jj<=runJ;++jj){
                // check if smaller than min or higher than max and update
                loop_pixel = src[index1+jj];
                colorSum =(loop_pixel.red & 0xff) +  (loop_pixel.green&0xff) + (loop_pixel.blue & 0xff);
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
    int index = m;
    for (; i < limit ; ++i) {
        j = 1;
        for (; j < limit; ++j) {
            dst[index+j] = applyKernel(i, j, src);
        }
        index+=m;
    }
}

void doConvolution(Image *image, int tmpKernel[3][3], int kernelScale1,bool filter1) {
    kernelScale = kernelScale1;
    filter=filter1;
    unsigned long mn3 = MN3;
    memcpy(kernel, tmpKernel, 36);
    pixel* pixelsImg = malloc(mn3);
    memcpy(pixelsImg, image->data, mn3);
    smooth((pixel*)image->data, pixelsImg);
    memcpy(image->data, pixelsImg, mn3);
    free(pixelsImg);
}

