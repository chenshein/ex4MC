
#include <stdbool.h>
#include "readBMP.h"
#include <stdbool.h>
#include "myutil.h"
#include "myfunction1.h"
#include "writeBMP.h"
#include <stdlib.h>
#include <string.h>
#include "showBMP.h"
#include <stdio.h>



#define NM (n*m)
#define limit (n-1)


/*
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernel(pixel *src, pixel *dst, int kernel[3][3], int kernelScale) {

	pixel_sum sum;
	pixel current_pixel;

	int weight = kernel[0][0];
	int weight_middle = kernel[1][1];

	register int zred;
	register int zgreen;
	register int zblue;

	int mred;
	int mgreen;
	int mblue;

	int i, j;
	// int start = kernelSize / 2;
	register int index1 = 0;
	for (i = 1 ; i < limit; i++) {
		index1 += n;
		for (j =  1 ; j < limit ; j++) {
			// for i-1
			register int index = (index1 - n) + j;
			// for i-1 and j-1
			pixel p = src[index - 1];
			zred = ((int) p.red);
			zgreen = ((int) p.green);
			zblue =  ((int) p.blue);

			// for i-1 and j

			p = src[index];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i-1 and j+1
			p = src[index+1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i
			index += n;
			// for i and j-1

			p = src[index-1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i and j

			p = src[index];
			mred = ((int) p.red)*weight_middle;
			mgreen = ((int) p.green)*weight_middle;
			mblue = ((int) p.blue)*weight_middle;

			// for i and j+1

			p = src[index+1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i+1
			index += n;
			// for i+1 and j-1
			p = src[index-1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i+1 and j

			p = src[index];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i+1 and j+1
			p = src[index+1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			sum.red = (zred*weight) + mred;
			sum.green = (zgreen*weight) + mgreen;
			sum.blue = (zblue*weight) + mblue;

			// assign kernel's result to pixel at [i,j]
			register int red = sum.red / kernelScale;
			register int green = sum.green / kernelScale;
			register int blue = sum.blue / kernelScale;

			if (red >=0 && red <= 255) {
				current_pixel.red = red;
			} else if (red < 0 ) {
				current_pixel.red = 0;
			} else if (red > 255) {
				current_pixel.red = 255;
			}

			if (green >=0 && green <= 255) {
				current_pixel.green = green;
			} else if (green < 0 ) {
				current_pixel.green = 0;
			} else if (green > 255) {
				current_pixel.green = 255;
			}

			if (blue >=0 && blue <= 255) {
				current_pixel.blue = blue;
			} else if (blue < 0 ) {
				current_pixel.blue = 0;
			} else if (blue > 255) {
				current_pixel.blue = 255;
			}
			dst[index1 + j] =  current_pixel;
		}
	}


}


/*
 *  Applies kernel for pixel at (i,j)
 */
static pixel applyKernelWFilter(pixel *src, pixel *dst, int kernel[3][3], int kernelScale) {
	int weight = kernel[0][0];
	int weight_middle = kernel[1][1];

	register int zred;
	register int zgreen;
	register int zblue;

	int mred;
	int mgreen;
	int mblue;

	pixel_sum sum;
	pixel current_pixel;
	register int i, j;
	// int start = kernelSize / 2;
	register int index1 = 0;
	for (i = 1 ; i < limit; i++) {
		index1 += n;
		for (j =  1 ; j < limit ; j++) {
			// for i-1
			register int index = (index1 - n) + j;
			// for i-1 and j-1
			pixel p = src[index - 1];
			zred = ((int) p.red);
			zgreen = ((int) p.green);
			zblue = ((int) p.blue);

			// for i-1 and j

			p = src[index];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i-1 and j+1
			p = src[index+1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i
			index += n;
			// for i and j-1

			p = src[index-1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i and j

			p = src[index];
			mred = ((int) p.red)*weight_middle;
			mgreen = ((int) p.green)*weight_middle;
			mblue = ((int) p.blue)*weight_middle;

			// for i and j+1

			p = src[index+1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i+1
			index += n;
			// for i+1 and j-1
			p = src[index-1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i+1 and j

			p = src[index];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			// for i+1 and j+1
			p = src[index+1];
			zred += ((int) p.red);
			zgreen += ((int) p.green);
			zblue += ((int) p.blue);

			sum.red = (zred*weight) + mred;
			sum.green = (zgreen*weight) + mgreen;
			sum.blue = (zblue*weight) + mblue;

			// find min and max coordinates
			// for i-1
			register int findex = ((i-1) * n) + j;
			// for i-1 and j-1
			pixel loop_pixel = src[findex-1];
			register int min_intensity = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			register int max_intensity = min_intensity;
			register int min_row = i - 1;
			register int max_row = min_row;
			register int min_col = j-1;
			register int max_col = min_col;
			// for  i-1 and j
			loop_pixel = src[findex];
			register int sumOfColors = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			if (sumOfColors <= min_intensity) {
				min_intensity = sumOfColors;
				min_col = j;
			} else if (sumOfColors > max_intensity) {
				max_intensity = sumOfColors;
				max_col = j;
			}
			// for  i-1 and j+1
			loop_pixel = src[findex + 1];
			sumOfColors = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			if (sumOfColors <= min_intensity) {
				min_intensity = sumOfColors;
				min_col = j + 1;
			} else if (sumOfColors > max_intensity) {
				max_intensity = sumOfColors;
				max_col = j + 1;
			}
			// for i
			findex += n;
			// for  i and j-1
			loop_pixel = src[findex-1];
			sumOfColors = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			if (sumOfColors <= min_intensity) {
				min_intensity = sumOfColors;
				min_row = i;
				min_col = j - 1;
			} else if (sumOfColors > max_intensity) {
				max_intensity = sumOfColors;
				max_row = i;
				max_col = j - 1;
			}
			// for  i and j
			loop_pixel = src[findex];
			sumOfColors = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			if (sumOfColors <= min_intensity) {
				min_intensity = sumOfColors;
				min_row = i;
				min_col = j;
			} else if (sumOfColors > max_intensity) {
				max_intensity = sumOfColors;
				max_row = i;
				max_col = j;
			}
			// for  i and j+1
			loop_pixel = src[findex+1];
			sumOfColors = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			if (sumOfColors <= min_intensity) {
				min_intensity = sumOfColors;
				min_row = i;
				min_col = j+1;
			} else if (sumOfColors > max_intensity) {
				max_intensity = sumOfColors;
				max_row = i;
				max_col = j+1;
			}
			//for i+1
			findex += n;
			// for  i+1 and j-1
			loop_pixel = src[findex-1];
			sumOfColors = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			if (sumOfColors <= min_intensity) {
				min_intensity = sumOfColors;
				min_row = i+1;
				min_col = j-1;
			} else if (sumOfColors > max_intensity) {
				max_intensity = sumOfColors;
				max_row = i+1;
				max_col = j-1;
			}
			// for  i+1 and j
			loop_pixel = src[findex];
			sumOfColors = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			if (sumOfColors <= min_intensity) {
				min_intensity = sumOfColors;
				min_row = i+1;
				min_col = j;
			} else if (sumOfColors > max_intensity) {
				max_intensity = sumOfColors;
				max_row = i+1;
				max_col = j;
			}
			// for  i+1 and j+1
			loop_pixel = src[findex+1];
			sumOfColors = ((int) loop_pixel.red) + ((int) loop_pixel.green) + ((int) loop_pixel.blue);
			if (sumOfColors <= min_intensity) {
				min_intensity = sumOfColors;
				min_row = i+1;
				min_col = j+1;
			} else if (sumOfColors > max_intensity) {
				max_intensity = sumOfColors;
				max_row = i+1;
				max_col = j+1;
			}
			// filter out min and max
			// filter out min and max
			pixel p1 = src[(min_row*n)+min_col];
			pixel p2 = src[(max_row*n)+max_col];
			sum.red += (-((int) p1.red) - ((int) p2.red));
			sum.green += (-((int) p1.green) - ((int) p2.green));
			sum.blue += (-((int) p1.blue) - ((int) p2.blue));

			// assign kernel's result to pixel at [i,j]
			register int red = sum.red / kernelScale;
			register int green = sum.green / kernelScale;
			register int blue = sum.blue / kernelScale;

			if (red >=0 && red <= 255) {
				current_pixel.red = red;
			} else if (red < 0 ) {
				current_pixel.red = 0;
			} else if (red > 255) {
				current_pixel.red = 255;
			}

			if (green >=0 && green <= 255) {
				current_pixel.green = green;
			} else if (green < 0 ) {
				current_pixel.green = 0;
			} else if (green > 255) {
				current_pixel.green = 255;
			}

			if (blue >=0 && blue <= 255) {
				current_pixel.blue = blue;
			} else if (blue < 0 ) {
				current_pixel.blue = 0;
			} else if (blue > 255) {
				current_pixel.blue = 255;
			}
			dst[index1 + j] = current_pixel;
		}
	}
}



void doConvolution(Image *image, int kernel[3][3], int kernelScale) {
	pixel* pixelsImg = malloc(3*NM);
	pixel* backupOrg = malloc(3*NM);

	// charsToPixels(image, pixelsImg);

	int row3n = 0;
	int stop1 = NM - 10;
	int i;
	char *data = image->data;
	for (i = 0; i <= stop1; i+=10){
		pixelsImg[i].red = data[row3n];
		pixelsImg[i].green = data[row3n + 1];
		pixelsImg[i].blue = data[row3n + 2];

		pixelsImg[i+1].red = data[row3n+3];
		pixelsImg[i+1].green = data[row3n + 4];
		pixelsImg[i+1].blue = data[row3n +5];

		pixelsImg[i+2].red = data[row3n+6];
		pixelsImg[i+2].green = data[row3n +7];
		pixelsImg[i+2].blue = data[row3n +8];

		pixelsImg[i+3].red = data[row3n+9];
		pixelsImg[i+3].green = data[row3n + 10];
		pixelsImg[i+3].blue = data[row3n + 11];

		pixelsImg[i+4].red = data[row3n+12];
		pixelsImg[i+4].green = data[row3n + 13];
		pixelsImg[i+4].blue = data[row3n +14];

		pixelsImg[i+5].red = data[row3n+15];
		pixelsImg[i+5].green = data[row3n + 16];
		pixelsImg[i+5].blue = data[row3n + 17];

		pixelsImg[i+6].red = data[row3n + 18];
		pixelsImg[i+6].green = data[row3n +19];
		pixelsImg[i+6].blue = data[row3n +20];

		pixelsImg[i+7].red = data[row3n +21];
		pixelsImg[i+7].green = data[row3n +22];
		pixelsImg[i+7].blue = data[row3n +23];

		pixelsImg[i+8].red = data[row3n +24];
		pixelsImg[i+8].green = data[row3n +25];
		pixelsImg[i+8].blue = data[row3n +26];

		pixelsImg[i+9].red = data[row3n +27];
		pixelsImg[i+9].green = data[row3n +28];
		pixelsImg[i+9].blue = data[row3n +29];

		row3n+=30;
	}

	for (;i < NM; i++) {
		pixelsImg[i].red = data[row3n];
		pixelsImg[i].green = data[row3n + 1];
		pixelsImg[i].blue = data[row3n + 2];
		row3n+=3;
	}

	memcpy(backupOrg, pixelsImg, 3*NM);

	// copyPixels(pixelsImg, backupOrg);
	applyKernel(backupOrg, pixelsImg, kernel, kernelScale);

	// pixelsToChars(pixelsImg, image);

	row3n = 0;
	for (i = 0; i <= stop1; i+=10){
		data[row3n] = pixelsImg[i].red;
		data[row3n + 1] = pixelsImg[i].green;
		data[row3n + 2] = pixelsImg[i].blue;

		data[row3n+3] = pixelsImg[i+1].red;
		data[row3n + 4] = pixelsImg[i+1].green;
		data[row3n +5] = pixelsImg[i+1].blue;

		data[row3n+6] = pixelsImg[i+2].red;
		data[row3n +7] = pixelsImg[i+2].green;
		data[row3n +8] = pixelsImg[i+2].blue;

		data[row3n+9] = pixelsImg[i+3].red;
		data[row3n + 10] = pixelsImg[i+3].green;
		data[row3n + 11] = pixelsImg[i+3].blue;

		data[row3n+12] = pixelsImg[i+4].red;
		data[row3n + 13] = pixelsImg[i+4].green;
		data[row3n +14] = pixelsImg[i+4].blue;

		data[row3n+15] = pixelsImg[i+5].red;
		data[row3n + 16] = pixelsImg[i+5].green;
		data[row3n + 17] = pixelsImg[i+5].blue;

		data[row3n + 18] = pixelsImg[i+6].red;
		data[row3n +19] = pixelsImg[i+6].green;
		data[row3n +20] = pixelsImg[i+6].blue;

		data[row3n +21] = pixelsImg[i+7].red;
		data[row3n +22] = pixelsImg[i+7].green;
		data[row3n +23] = pixelsImg[i+7].blue;

		data[row3n +24] = pixelsImg[i+8].red;
		data[row3n +25] = pixelsImg[i+8].green;
		data[row3n +26] = pixelsImg[i+8].blue;

		data[row3n +27] = pixelsImg[i+9].red;
		data[row3n +28] = pixelsImg[i+9].green;
		data[row3n +29] = pixelsImg[i+9].blue;

		row3n+=30;
	}

	for (;i < NM; i++) {
		data[row3n] = pixelsImg[i].red;
		data[row3n + 1] = pixelsImg[i].green;
		data[row3n + 2] = pixelsImg[i].blue;
		row3n+=3;
	}

	free(pixelsImg);
	free(backupOrg);
}

void doConvolutionWFilter(Image *image, int kernel[3][3], int kernelScale) {
	pixel* pixelsImg = malloc(3*NM);
	pixel* backupOrg = malloc(3*NM);

	int row3n = 0;
	int stop1 = NM - 10;
	int i;
	char *data = image->data;
	for (i = 0; i <= stop1; i+=10){
		pixelsImg[i].red = data[row3n];
		pixelsImg[i].green = data[row3n + 1];
		pixelsImg[i].blue = data[row3n + 2];

		pixelsImg[i+1].red = data[row3n+3];
		pixelsImg[i+1].green = data[row3n + 4];
		pixelsImg[i+1].blue = data[row3n +5];

		pixelsImg[i+2].red = data[row3n+6];
		pixelsImg[i+2].green = data[row3n +7];
		pixelsImg[i+2].blue = data[row3n +8];

		pixelsImg[i+3].red = data[row3n+9];
		pixelsImg[i+3].green = data[row3n + 10];
		pixelsImg[i+3].blue = data[row3n + 11];

		pixelsImg[i+4].red = data[row3n+12];
		pixelsImg[i+4].green = data[row3n + 13];
		pixelsImg[i+4].blue = data[row3n +14];

		pixelsImg[i+5].red = data[row3n+15];
		pixelsImg[i+5].green = data[row3n + 16];
		pixelsImg[i+5].blue = data[row3n + 17];

		pixelsImg[i+6].red = data[row3n + 18];
		pixelsImg[i+6].green = data[row3n +19];
		pixelsImg[i+6].blue = data[row3n +20];

		pixelsImg[i+7].red = data[row3n +21];
		pixelsImg[i+7].green = data[row3n +22];
		pixelsImg[i+7].blue = data[row3n +23];

		pixelsImg[i+8].red = data[row3n +24];
		pixelsImg[i+8].green = data[row3n +25];
		pixelsImg[i+8].blue = data[row3n +26];

		pixelsImg[i+9].red = data[row3n +27];
		pixelsImg[i+9].green = data[row3n +28];
		pixelsImg[i+9].blue = data[row3n +29];

		row3n+=30;
	}

	for (;i < NM; i++) {
		pixelsImg[i].red = data[row3n];
		pixelsImg[i].green = data[row3n + 1];
		pixelsImg[i].blue = data[row3n + 2];
		row3n+=3;
	}

	memcpy(backupOrg, pixelsImg, 3*NM);

	applyKernelWFilter(backupOrg, pixelsImg, kernel, kernelScale);

	row3n = 0;
	for (i = 0; i <= stop1; i+=10){
		data[row3n] = pixelsImg[i].red;
		data[row3n + 1] = pixelsImg[i].green;
		data[row3n + 2] = pixelsImg[i].blue;

		data[row3n+3] = pixelsImg[i+1].red;
		data[row3n + 4] = pixelsImg[i+1].green;
		data[row3n +5] = pixelsImg[i+1].blue;

		data[row3n+6] = pixelsImg[i+2].red;
		data[row3n +7] = pixelsImg[i+2].green;
		data[row3n +8] = pixelsImg[i+2].blue;

		data[row3n+9] = pixelsImg[i+3].red;
		data[row3n + 10] = pixelsImg[i+3].green;
		data[row3n + 11] = pixelsImg[i+3].blue;

		data[row3n+12] = pixelsImg[i+4].red;
		data[row3n + 13] = pixelsImg[i+4].green;
		data[row3n +14] = pixelsImg[i+4].blue;

		data[row3n+15] = pixelsImg[i+5].red;
		data[row3n + 16] = pixelsImg[i+5].green;
		data[row3n + 17] = pixelsImg[i+5].blue;

		data[row3n + 18] = pixelsImg[i+6].red;
		data[row3n +19] = pixelsImg[i+6].green;
		data[row3n +20] = pixelsImg[i+6].blue;

		data[row3n +21] = pixelsImg[i+7].red;
		data[row3n +22] = pixelsImg[i+7].green;
		data[row3n +23] = pixelsImg[i+7].blue;

		data[row3n +24] = pixelsImg[i+8].red;
		data[row3n +25] = pixelsImg[i+8].green;
		data[row3n +26] = pixelsImg[i+8].blue;

		data[row3n +27] = pixelsImg[i+9].red;
		data[row3n +28] = pixelsImg[i+9].green;
		data[row3n +29] = pixelsImg[i+9].blue;

		row3n+=30;
	}

	for (;i < NM; i++) {
		data[row3n] = pixelsImg[i].red;
		data[row3n + 1] = pixelsImg[i].green;
		data[row3n + 2] = pixelsImg[i].blue;
		row3n+=3;
	}
	free(pixelsImg);
	free(backupOrg);
}

//void myfunction(Image *image, char* srcImgpName, char* blurRsltImgName, char* sharpRsltImgName, char* filteredBlurRsltImgName, char* filteredSharpRsltImgName, char flag) {
//
//	/*
//	* [1, 1, 1]
//	* [1, 1, 1]
//	* [1, 1, 1]
//	*/
//	int blurKernel[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
//
//	/*
//	* [-1, -1, -1]
//	* [-1, 9, -1]
//	* [-1, -1, -1]
//	*/
//	int sharpKernel[3][3] = {{-1,-1,-1},{-1,9,-1},{-1,-1,-1}};
//
//	if (flag == '1') {
//		// blur image
//		doConvolution(image, blurKernel, 9);
//
//		// write result image to file
//		writeBMP(image, srcImgpName, blurRsltImgName);
//
//		// sharpen the resulting image
//		doConvolution(image, sharpKernel, 1);
//
//		// write result image to file
//		writeBMP(image, srcImgpName, sharpRsltImgName);
//	} else {
//		// apply extermum filtered kernel to blur image
//		doConvolutionWFilter(image, blurKernel, 7);
//
//		// write result image to file
//		writeBMP(image, srcImgpName, filteredBlurRsltImgName);
//
//		// sharpen the resulting image
//		doConvolution(image, sharpKernel, 1);
//
//		// write result image to file
//		writeBMP(image, srcImgpName, filteredSharpRsltImgName);
//	}
//}
