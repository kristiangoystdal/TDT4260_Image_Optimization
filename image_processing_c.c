#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "ppm.h"

typedef struct {
     float red,green,blue;
} AccuratePixel;

typedef struct {
     int x, y;
     AccuratePixel *data;
} AccurateImage;

AccurateImage *convertToAccurateImage(PPMImage *image) {
	AccurateImage *imageAccurate = (AccurateImage *)malloc(sizeof(AccurateImage));
	imageAccurate->data = (AccuratePixel*)malloc(image->x * image->y * sizeof(AccuratePixel));
	for(int i = 0; i < image->x * image->y; i++) {
		imageAccurate->data[i].red   = (float) image->data[i].red;
		imageAccurate->data[i].green = (float) image->data[i].green;
		imageAccurate->data[i].blue  = (float) image->data[i].blue;
	}
	imageAccurate->x = image->x;
	imageAccurate->y = image->y;
	return imageAccurate;
}

PPMImage * convertToPPPMImage(AccurateImage *imageIn) {
    PPMImage *imageOut = (PPMImage *)malloc(sizeof(PPMImage));
    imageOut->data = (PPMPixel*)malloc(imageIn->x * imageIn->y * sizeof(PPMPixel));
    imageOut->x = imageIn->x;
    imageOut->y = imageIn->y;
    for(int i = 0; i < imageIn->x * imageIn->y; i++) {
        imageOut->data[i].red = imageIn->data[i].red;
        imageOut->data[i].green = imageIn->data[i].green;
        imageOut->data[i].blue = imageIn->data[i].blue;
    }
    return imageOut;
}

void separableBlurIteration(AccurateImage *imageOut, AccurateImage *imageIn, int colourType, int size) {
    int width = imageIn->x;
    int height = imageIn->y;

    AccurateImage *temp = (AccurateImage *)malloc(sizeof(AccurateImage));
    temp->x = width;
    temp->y = height;
    temp->data = (AccuratePixel *)malloc(width * height * sizeof(AccuratePixel));

    #pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float sum = 0;
            int count = 0;
            for (int dx = -size; dx <= size; dx++) {
                int nx = x + dx;
                if (nx >= 0 && nx < width) {
                    int idx = y * width + nx;
                    if (colourType == 0) sum += imageIn->data[idx].red;
                    else if (colourType == 1) sum += imageIn->data[idx].green;
                    else sum += imageIn->data[idx].blue;
                    count++;
                }
            }
            int idx = y * width + x;
            if (colourType == 0) temp->data[idx].red = sum / count;
            else if (colourType == 1) temp->data[idx].green = sum / count;
            else temp->data[idx].blue = sum / count;
        }
    }

    #pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float sum = 0;
            int count = 0;
            for (int dy = -size; dy <= size; dy++) {
                int ny = y + dy;
                if (ny >= 0 && ny < height) {
                    int idx = ny * width + x;
                    if (colourType == 0) sum += temp->data[idx].red;
                    else if (colourType == 1) sum += temp->data[idx].green;
                    else sum += temp->data[idx].blue;
                    count++;
                }
            }
            int idx = y * width + x;
            if (colourType == 0) imageOut->data[idx].red = sum / count;
            else if (colourType == 1) imageOut->data[idx].green = sum / count;
            else imageOut->data[idx].blue = sum / count;
        }
    }

    free(temp->data);
    free(temp);
}

PPMImage * imageDifference(AccurateImage *imageInSmall, AccurateImage *imageInLarge) {
    PPMImage *imageOut = (PPMImage *)malloc(sizeof(PPMImage));
    imageOut->data = (PPMPixel*)malloc(imageInSmall->x * imageInSmall->y * sizeof(PPMPixel));
    imageOut->x = imageInSmall->x;
    imageOut->y = imageInSmall->y;

    for(int i = 0; i < imageInSmall->x * imageInSmall->y; i++) {
        float value = imageInLarge->data[i].red - imageInSmall->data[i].red;
        if(value > 255) imageOut->data[i].red = 255;
        else if (value < -1.0) {
            value = 257.0 + value;
            imageOut->data[i].red = value > 255 ? 255 : floor(value);
        } else if (value > -1.0 && value < 0.0) imageOut->data[i].red = 0;
        else imageOut->data[i].red = floor(value);

        value = imageInLarge->data[i].green - imageInSmall->data[i].green;
        if(value > 255) imageOut->data[i].green = 255;
        else if (value < -1.0) {
            value = 257.0 + value;
            imageOut->data[i].green = value > 255 ? 255 : floor(value);
        } else if (value > -1.0 && value < 0.0) imageOut->data[i].green = 0;
        else imageOut->data[i].green = floor(value);

        value = imageInLarge->data[i].blue - imageInSmall->data[i].blue;
        if(value > 255) imageOut->data[i].blue = 255;
        else if (value < -1.0) {
            value = 257.0 + value;
            imageOut->data[i].blue = value > 255 ? 255 : floor(value);
        } else if (value > -1.0 && value < 0.0) imageOut->data[i].blue = 0;
        else imageOut->data[i].blue = floor(value);
    }
    return imageOut;
}

int main(int argc, char** argv) {
    PPMImage *image = argc > 1 ? readPPM("flower.ppm") : readStreamPPM(stdin);

		
    AccurateImage *imageAccurate1_tiny = convertToAccurateImage(image);
    AccurateImage *imageAccurate2_tiny = convertToAccurateImage(image);
		#pragma omp parallel
	 {
				int size = 2;
				
				#pragma omp single
				{
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 0, size);
						separableBlurIteration(imageAccurate1_tiny, imageAccurate2_tiny, 0, size);
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 0, size);
						separableBlurIteration(imageAccurate1_tiny, imageAccurate2_tiny, 0, size);
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 0, size);
				}

				#pragma omp single
				{
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 1, size);
						separableBlurIteration(imageAccurate1_tiny, imageAccurate2_tiny, 1, size);
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 1, size);
						separableBlurIteration(imageAccurate1_tiny, imageAccurate2_tiny, 1, size);
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 1, size);
				}

				#pragma omp single
				{
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 2, size);
						separableBlurIteration(imageAccurate1_tiny, imageAccurate2_tiny, 2, size);
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 2, size);
						separableBlurIteration(imageAccurate1_tiny, imageAccurate2_tiny, 2, size);
						separableBlurIteration(imageAccurate2_tiny, imageAccurate1_tiny, 2, size);
				}
		}

    AccurateImage *imageAccurate1_small = convertToAccurateImage(image);
    AccurateImage *imageAccurate2_small = convertToAccurateImage(image);
    for(int colour = 0; colour < 3; colour++) {
        int size = 3;
        separableBlurIteration(imageAccurate2_small, imageAccurate1_small, colour, size);
        separableBlurIteration(imageAccurate1_small, imageAccurate2_small, colour, size);
        separableBlurIteration(imageAccurate2_small, imageAccurate1_small, colour, size);
        separableBlurIteration(imageAccurate1_small, imageAccurate2_small, colour, size);
        separableBlurIteration(imageAccurate2_small, imageAccurate1_small, colour, size);
    }

    AccurateImage *imageAccurate1_medium = convertToAccurateImage(image);
    AccurateImage *imageAccurate2_medium = convertToAccurateImage(image);
    for(int colour = 0; colour < 3; colour++) {
        int size = 5;
        separableBlurIteration(imageAccurate2_medium, imageAccurate1_medium, colour, size);
        separableBlurIteration(imageAccurate1_medium, imageAccurate2_medium, colour, size);
        separableBlurIteration(imageAccurate2_medium, imageAccurate1_medium, colour, size);
        separableBlurIteration(imageAccurate1_medium, imageAccurate2_medium, colour, size);
        separableBlurIteration(imageAccurate2_medium, imageAccurate1_medium, colour, size);
    }

    AccurateImage *imageAccurate1_large = convertToAccurateImage(image);
    AccurateImage *imageAccurate2_large = convertToAccurateImage(image);
    for(int colour = 0; colour < 3; colour++) {
        int size = 8;
        separableBlurIteration(imageAccurate2_large, imageAccurate1_large, colour, size);
        separableBlurIteration(imageAccurate1_large, imageAccurate2_large, colour, size);
        separableBlurIteration(imageAccurate2_large, imageAccurate1_large, colour, size);
        separableBlurIteration(imageAccurate1_large, imageAccurate2_large, colour, size);
        separableBlurIteration(imageAccurate2_large, imageAccurate1_large, colour, size);
    }

    PPMImage *final_tiny = imageDifference(imageAccurate2_tiny, imageAccurate2_small);
    PPMImage *final_small = imageDifference(imageAccurate2_small, imageAccurate2_medium);
    PPMImage *final_medium = imageDifference(imageAccurate2_medium, imageAccurate2_large);

    if(argc > 1) {
        writePPM("flower_tiny.ppm", final_tiny);
        writePPM("flower_small.ppm", final_small);
        writePPM("flower_medium.ppm", final_medium);
    } else {
        writeStreamPPM(stdout, final_tiny);
        writeStreamPPM(stdout, final_small);
        writeStreamPPM(stdout, final_medium);
    }
}