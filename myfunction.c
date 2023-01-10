#include <stdbool.h>
#include "myutil.h"
#include "myfunction1.h"
#include "myfunction.h"
#include "writeBMP.h"
#include "showBMP.h"
#include <stdio.h>


void myfunction(Image *image, char* srcImgpName, char* blurRsltImgName, char* sharpRsltImgName, char* rowBlurRsltImgName, char* rowSharpRsltImgName, char* filteredBlurRsltImgName, char* filteredSharpRsltImgName, char flag) {

    // change it to switch case, so we don't need to check each if condition, here we go to the wanted condition
    // and end the case by goto done because there is no need to continue
    switch (flag) {
        case '1':
            // blur image
            doConvolution(image, 4, 9,false);

            // write result image to file
            writeBMP(image, srcImgpName, blurRsltImgName);

            // sharpen the resulting image
            doConvolution(image,  2, 1,false);

            // write result image to file
            writeBMP(image, srcImgpName, sharpRsltImgName);
            goto done;
        case '2':
            // blur image with row-blurring kernel
            doConvolution(image, 3, 4,false);

            // write result image to file
            writeBMP(image, srcImgpName, rowBlurRsltImgName);

            // sharpen the resulting image with row-sharpening kernel
            doConvolution(image, 1, 2,false);

            // write result image to file
            writeBMP(image, srcImgpName, rowSharpRsltImgName);
            goto done;
        default:
            // apply extermum filtered kernel to blur image
            doConvolution(image, 4, 7,true);

            // write result image to file
            writeBMP(image, srcImgpName, filteredBlurRsltImgName);

            // sharpen the resulting image
            doConvolution(image, 2, 1,false);

            // write result image to file
            writeBMP(image, srcImgpName, filteredSharpRsltImgName);
            goto done;
    }
    done:
}