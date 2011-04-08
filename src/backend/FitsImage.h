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

QT_BEGIN_NAMESPACE
class QPixmap;
QT_END_NAMESPACE

// External libraries
#include "fitsio.h"
#include "wcs.h"
#include "fitsfile.h"

#include "PPWcsImage.h"

#define DOWNSAMPLE_SIZE 2048

#define LINEAR_STRETCH 0
#define LOG_STRETCH 1
#define SQRT_STRETCH 2
#define ARCSINH_STRETCH 3
#define POWER_STRETCH 4

class FitsImage : public PPWcsImage {
	
	Q_OBJECT
	
public:
	// Methods
	FitsImage(QString &fileName);
	~FitsImage();
	float minpixel, maxpixel, difference;
	float vmin, vmax, lowerLimit, upperLimit;
	bool downsampled;
	float* imagedata;
	int stretch;
	QPixmap pixmap;
	
	// Public Methods
	QPointF fpix2pix(QPointF fpix);
	
public slots:
	void setStretch(int s);
	void setVmin(float minpix);
	void setVmax(float maxpix);
	void invert();
	void fitCentroid(QPointF pos);
	double* pix2sky(QPointF pos);
	
signals:
	void pixmapChanged(QPixmap *pm);
	void centroid(QPointF pos);
	
private:
	// Methods
	bool verifyWCS();
	void calculateExtremals();
	void downsample(float** arr, int W, int H, int S, int* newW, int* newH);
	bool calculatePercentile(float lp, float up);
	bool calibrateImage(int s, float minpix, float maxpix);
	float pixelIntensity(QPointF pos);
		
	// Attributes
	fitsfile *fptr;
	int status, wcsstatus;
	int numhdus, numimgs, naxis, hdutype;
	long width, height;
	long numelements;
	long* fpixel;
	int bitpix;
	float* renderdata;
	float lowerPercentile;
	float upperPercentile;
	char alt;
	bool inverted;
};

#endif
