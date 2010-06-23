/*
 *  PinpointWCS is developed by the Chandra X-ray Center
 *  Education and Public Outreach Group
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FITSIMAGE_H
#define FITSIMAGE_H

#include <QImage>
#include "fitsio.h"
#include "wcshdr.h"

#define DOWNSAMPLE_SIZE 2048

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
		double* pixelToCelestialCoordinates(QPointF pos);
		
		// Attributes
		QImage* image;
		struct wcsprm *wcs;
		
	private:
		// Methods
		bool checkWorldCoordinateSystem();
		void calculateExtremals();
		void downsample(float** arr, int W, int H, int S, int* newW, int* newH);
		bool calculatePercentile(float lp, float up);
		void calibrateImage(int stretch);
		
		// Attributes
		fitsfile *fptr;
		int status, wcsstatus;
		int numhdus, numimgs, naxis, hdutype;
		long naxisn[2];
		long width, height;
		long numelements;
		long* fpixel;
		int bitpix;
		float* imagedata;
		float* renderdata;
		float minpix, maxpix, difference;
		float lowerPercentile;
		float upperPercentile;
		float vmin, vmax;
		bool downsampled;
		int M;
	};

#endif
