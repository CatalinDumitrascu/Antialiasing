#include "homework.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int num_threads;
int resize_factor;
//pixel **inputImage->pixels;
pixel **globalOut;

image* inputImage = NULL;
image* outputImage = NULL;

int nrBlocks;
int width;
int height;
int outHeight;
int outWidth;
int step;
int coloured;

int gaussKernel[3][3] =  {
							{1, 2, 1},
							{2, 4, 2},
							{1, 2, 1}
						 };

void initImg(image *img) {
	img->pixels = malloc(img->height * sizeof(pixel*));
	for(int i = 0; i < img->height; i++) {
		img->pixels[i] = malloc(img->width * sizeof(pixel));
	}
}

void initMat(pixel ***global, int height, int width) {
	(*global) = malloc(height * sizeof(pixel*));
	for(int i = 0; i < height; i++) {
		(*global)[i] = malloc(width * sizeof(pixel));
	}

}

pixel median(int start_i,int start_j) {

	int i, j, k = 0, l = 0;
	int stop_i = start_i + resize_factor;
	int stop_j = start_j + resize_factor;
	int sumGrey = 0;
	int sumRed = 0;
	int sumBlue = 0;
	int sumGreen = 0;
	int div = 0;

	if(resize_factor % 2 == 0) {
		div = resize_factor * resize_factor;
	} else {
		div = 16;
	}
	
	if(coloured == 0) {
		if(resize_factor % 2 == 0) {
			for(i = start_i; i < stop_i; i++) {
				for(j = start_j; j < stop_j; j++) {
					sumGrey += inputImage->pixels[i][j].grey;
				}
			}
		} else {
			for(i = start_i; i < stop_i; i++) {

				for(j = start_j; j < stop_j; j++) {
					
					sumGrey += inputImage->pixels[i][j].grey * gaussKernel[k][l];
					l++;
				}
				k++;
				l = 0;
			}
			
		}
	} else {
		if(resize_factor % 2 == 0) {
			for(i = start_i; i < stop_i; i++) {
				for(j = start_j; j < stop_j; j++) {
					sumRed += inputImage->pixels[i][j].r;
					sumGreen += inputImage->pixels[i][j].g;
					sumBlue += inputImage->pixels[i][j].b;
				}
			}
		} else {
			for(i = start_i; i < stop_i; i++) {
				for(j = start_j; j < stop_j; j++) {

					sumRed += inputImage->pixels[i][j].r * gaussKernel[k][l];
					sumGreen += inputImage->pixels[i][j].g * gaussKernel[k][l];
					sumBlue += inputImage->pixels[i][j].b * gaussKernel[k][l];
					l++;
				}
				k++;
				l = 0; 
			}
		}
	}

	pixel new;
	new.grey = sumGrey / div;
	new.r = sumRed / div;
	new.g = sumGreen / div;
	new.b = sumBlue / div;
	return new;
}

void* threadFunction(void *var) {
	int thread_id = *(int*)var;
	int i, j;
	int l = 0;

	int start = thread_id * height/num_threads;
	int k = start / resize_factor;
	int stop = (((thread_id+1)* height/num_threads) / resize_factor) * resize_factor;

	if(thread_id == num_threads - 1) {
		stop = height;
	}

	for(i = start; i < stop; i+=resize_factor) {
		for(j = 0; j < width; j+=resize_factor) {
			if(i + resize_factor <= height && j + resize_factor <= width)  {
				globalOut[k][l] = median(i,j);
			}
			l++;
		}
		l = 0;
		k++;
	}
			
	return 0;			
}


void readInput(const char * fileName, image *img) {
	FILE *file;
	file = fopen(fileName, "r");

	char buf[2];
	fread(&buf, sizeof(char), 2, file);

	if(buf[1] == '5') {
		img->isColoured = 0;
	} else {
		img->isColoured = 1;
	}

	char fmm;

	fscanf(file, "%d", &img->width);
	fscanf(file, "%d", &img->height);
	fscanf(file, "%d", &img->maxval);
	fscanf(file, "%c", &fmm);

	initImg(img);

	if(img->isColoured == 0) {
		unsigned char bufImg[img->height * img->width];
		fread(&bufImg, sizeof(unsigned char), img->height * img->width, file);
		for(int i = 0; i < img->height; i++) {
			for(int j = 0; j < img->width; j++) {
				img->pixels[i][j].grey = bufImg[i * img->width + j];
				
			}
		}

	} else {
		unsigned char bufImg[img->height * img->width * 3];
		fread(&bufImg, sizeof(unsigned char), img->height * img->width * 3, file);
		for(int i = 0; i < img->height; i++) {
			for(int j = 0; j < img->width; j++) {
				img->pixels[i][j].r = bufImg[i * img->width * 3 + j * 3];
				img->pixels[i][j].g = bufImg[i * img->width * 3 + j * 3 + 1];
				img->pixels[i][j].b = bufImg[i * img->width * 3 + j * 3 + 2];
			}
		}
	}

	fclose(file);

}



void writeData(const char * fileName, image *img) {
	FILE *file;
	file = fopen(fileName, "w");

	if(img->isColoured == 0) {
		fprintf(file, "P5\n");

		fprintf(file, "%d %d\n", img->width, img->height);
		fprintf(file, "%d\n", img->maxval);

		for(int i = 0; i < img->height; i++) {
			for(int j = 0; j < img->width; j++) {
				fprintf(file, "%c", img->pixels[i][j].grey);
			}
		}
	} else {
		fprintf(file, "P6\n");

		fprintf(file, "%d %d\n", img->width, img->height);
		fprintf(file, "%d\n", img->maxval);

		for(int i = 0; i < img->height; i++) {
			for(int j = 0; j < img->width; j++) {
				fprintf(file, "%c%c%c", img->pixels[i][j].r, img->pixels[i][j].g, img->pixels[i][j].b);
			}
		}
	}

	fclose(file);
}

void resize(image *in, image *out) { 
	int i;
	out->isColoured = in->isColoured;
	out->width = in->width / resize_factor;
	out->height = in->height / resize_factor;
	out->maxval = in->maxval;

	width = in->width;
	height = in->height;
	outWidth = out->width;
	outHeight = out->height;
	coloured = in->isColoured;


	inputImage = in;

	outputImage = out;

	globalOut = malloc (outHeight * sizeof(pixel*));
	for (int i = 0; i < outHeight; i++) {
		globalOut[i] = malloc(outWidth * sizeof(pixel));
	}

	pthread_t tid[num_threads];
	int thread_id[num_threads];
	for(i = 0;i < num_threads; i++)
		thread_id[i] = i;

	for(i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
		
	}

	for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

	out->pixels = globalOut;
}

int medianGaussGrey(int start_i, int stop_i, int start_j, int stop_j, pixel **in) {
	int i, j, k = 0, l = 0;
	int sum = 0;
	for(i = start_i; i < stop_i; i++) {
		for(j = start_j; j < stop_j; j++) {
			sum += (int) in[i][j].grey * gaussKernel[k][l];
			l++;
		}
		l = 0;
		k++;
	}

	return sum / 16;
}

int medianGaussRed(int start_i, int stop_i, int start_j, int stop_j, pixel **in) {
	int i, j, k = 0, l = 0;
	int sum = 0;
	for(i = start_i; i < stop_i; i++) {
		for(j = start_j; j < stop_j; j++) {
			sum += (int) in[i][j].r * gaussKernel[k][l];
			l++;
		}
		l = 0;
		k++;
	}

	return sum / 16;
}

int medianGaussGreen(int start_i, int stop_i, int start_j, int stop_j, pixel **in) {
	int i, j, k = 0, l = 0;
	int sum = 0;
	for(i = start_i; i < stop_i; i++) {
		for(j = start_j; j < stop_j; j++) {
			sum += (int) in[i][j].g * gaussKernel[k][l];
			l++;
		}
		l = 0;
		k++;
	}

	return sum / 16;
}

int medianGaussBlue(int start_i, int stop_i, int start_j, int stop_j, pixel **in) {
	int i, j, k = 0, l = 0;
	int sum = 0;
	for(i = start_i; i < stop_i; i++) {
		for(j = start_j; j < stop_j; j++) {
			sum += (int) in[i][j].b * gaussKernel[k][l];
			l++;
		}
		l = 0;
		k++;
	}

	return sum / 16;
}

int medianGrey(int start_i, int stop_i, int start_j, int stop_j, pixel **in) {

	int i, j;
	int sum = 0;
	for(i = start_i; i < stop_i; i++) {
		for(j = start_j; j < stop_j; j++) {
			sum += (int) in[i][j].grey;
		}
	}
	return sum / (resize_factor * resize_factor);
}

int medianRed(int start_i, int stop_i, int start_j, int stop_j, pixel **in) {
	
	int i, j;
	int sum = 0;
	for(i = start_i; i < stop_i; i++) {
		for(j = start_j; j < stop_j; j++) {
			sum += (int) in[i][j].r;
		}
	}

	return sum / (resize_factor * resize_factor);
}

int medianGreen(int start_i, int stop_i, int start_j, int stop_j, pixel **in) {
	
	int i, j;
	int sum = 0;
	for(i = start_i; i < stop_i; i++) {
		for(j = start_j; j < stop_j; j++) {
			sum += (int)in[i][j].g;
		}
	}

	return sum / (resize_factor * resize_factor);
}

int medianBlue(int start_i, int stop_i, int start_j, int stop_j, pixel **in) {
	
	int i, j;
	int sum = 0;
	for(i = start_i; i < stop_i; i++) {
		for(j = start_j; j < stop_j; j++) {
			sum += (int)in[i][j].b;
		}
	}

	return sum / (resize_factor * resize_factor);
}