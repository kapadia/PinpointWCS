#ifndef FITSIMAGE_H
#define FITSIMAGE_H

#include <QImage>
#include "fitsio.h"

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
		bool calculatePercentile(float lp, float up);
		
		// Attributes
		fitsfile *fptr;
		int status;
		int numhdus, numimgs, naxis, hdutype;
		long naxisn[2];
		long* fpixel;
		int bitpix;
		float* thedata;
		float minpix, maxpix, diff;
		float lowerPercentile;
		float upperPercentile;
		float vmin, vmax;
		
		
	};

#endif
