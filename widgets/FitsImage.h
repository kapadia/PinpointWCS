#ifndef FITSIMAGE_H
#define FITSIMAGE_H

#include <QImage>
#include "fitsio.h"

#define LINEAR_STRETCH 0
#define LOG_STRETCH 1
#define SQRT_STRETCH 2
#define ARCSINH_STRETCH 3
#define POWER_STRETCH 4

class FitsImage
	{
	public:
		// Methods
		FitsImage(QString & fileName);
		~FitsImage();
		
		// Attributes
		QImage* image; // why does this have to be a pointer to a QImage object???
		
	private:
		// Methods
		void calculateExtremals();
		void downsample(float** imagedata, int W, int H, int S, int* newW, int* newH);
		bool calculatePercentile(float lp, float up);
		void calibrateImage(int stretch);
		void normalize();
		
		// Attributes
		fitsfile *fptr;
		int status;
		int numhdus, numimgs, naxis, hdutype;
		long naxisn[2];
		long numelements;
		long* fpixel;
		int bitpix;
		float* imagedata;
		float* renderdata;
		float minpix, maxpix, difference;
		float lowerPercentile;
		float upperPercentile;
		float vmin, vmax;
	};

#endif
