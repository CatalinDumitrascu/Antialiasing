#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
	unsigned char r, g, b, grey;
}pixel;

typedef struct {
	short isColoured;
	int width, height;
	int maxval;
	//int greyScale;
	pixel** pixels; 
}image;



void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image *out);

int medianGrey(int start_i, int stop_i, int start_j, int stop_j, pixel **in);

int medianRed(int start_i, int stop_i, int start_j, int stop_j, pixel **in);

int medianGreen(int start_i, int stop_i, int start_j, int stop_j, pixel **in);

int medianBlue(int start_i, int stop_i, int start_j, int stop_j, pixel **in);

int medianGaussGrey(int start_i, int stop_i, int start_j, int stop_j, pixel **in);

int medianGaussRed(int start_i, int stop_i, int start_j, int stop_j, pixel **in);

int medianGaussGreen(int start_i, int stop_i, int start_j, int stop_j, pixel **in);

int medianGaussBlue(int start_i, int stop_i, int start_j, int stop_j, pixel **in);




#endif /* HOMEWORK_H */