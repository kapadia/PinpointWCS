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

#include <QDebug>

#include <algorithm>
#include <exception>

#include "math.h"
#include "FitsImage.h"
#include "PinpointWCSUtils.h"


FitsImage::FitsImage(QString &fileName) : PPWcsImage()
{
	qDebug() << "Initializing FitsImage object ...";
	
	// Initialize some attributes
	filename = fileName;
	status = 0;
	lowerPercentile = 0.0025;
	upperPercentile = 0.9975;
	downsampled = false;
}


FitsImage::~FitsImage()
{
	qDebug() << "~FitsImage";
	free(imagedata);
	free(fptr);
}


bool FitsImage::setup()
{	
	qDebug() << "Setting up FitsImage object ...";
	
	// Initialize some attributes
	fptr = NULL;
	imagedata = NULL;
	renderdata = NULL;
	
	// Open FITS file
	fits_open_file(&fptr, filename.toStdString().c_str(), READONLY, &status);
	if (status)
	{
		fits_report_error(stderr, status);
		return false;
	}
	
	// Check the number of HDUs
	fits_get_num_hdus(fptr, &numhdus, &status);
	if (status)
	{
		fits_report_error(stderr, status);
		return false;
	}
	
	// Set number of images to zero
	numimgs = 0;
	
	// Begin looping over each HDU
	for (int kk=1; kk <= numhdus; kk++)
	{
		int ii;
		status = 0;
		
		// Change to another HDU and check type
		fits_movabs_hdu(fptr, kk, &hdutype, &status);
		fits_get_hdu_type(fptr, &hdutype, &status);
		
		qDebug() << "Header Number: " << kk;
		
		if (hdutype != IMAGE_HDU)
			continue;
		
		// Check image dimensions and size
		fits_get_img_dim(fptr, &naxis, &status);
		if (status)
		{
			qDebug() << "fits_get_img_dim";
			fits_report_error(stderr, status);
			continue;
		}
		
		if (naxis != 2)
		{
			qDebug() << "Image does not have the correct dimensions ...";
			continue;
		}
		
		fits_get_img_size(fptr, 2, naxisn, &status);
		if (status)
		{
			qDebug() << "fits_get_img_size";
			fits_report_error(stderr, status);
			continue;
		}
		width = naxisn[0];
		height = naxisn[1];
		
		// Check that the image contains sufficient WCS
		if ( !verifyWCS() )
			continue;
		
		// Compute the total number of pixels in image array
		numelements = width*height;
		
		// The HDU is a valid image, increment
		numimgs++;
		
		// Determine the data type of image (i.e. bitpix)
		fits_get_img_type(fptr, &bitpix, &status);
		if (status)
		{
			fits_report_error(stderr, status);
			continue;
		}
		qDebug() << "BITPIX: " << bitpix;
		
		// Allocate memory for the first pixel
		fpixel = (long *) malloc(naxis * sizeof(long));
		
		// Initialize the first pixel location to {1, ..., 1}
		for (ii=0; ii<naxis; ii++)
			fpixel[ii] = 1;
		
		// Allocate memory for the image pixels
		imagedata = (float *) malloc(numelements * sizeof(float));
		if (!imagedata)
		{
			qDebug() << "Failed to allocate memory for the image array ...";
			free(fpixel);
			continue;
		}
		
		fits_read_pix(fptr, TFLOAT, fpixel, numelements, NULL, imagedata, NULL, &status);
		free(fpixel);		
		if (status)
		{
			// Free the allocated memory
			free(imagedata);
			qDebug() << "fits_read_pix";
			fits_report_error(stderr, status);
			continue;
		}
		
		// FITS data retrieved!!!
		
		// FIXME: Adjust for Spitzer data.  As is, the quantiles are mis-calculated since it 
		// weighs on zeroed pixels.
		// Check that pixel values are not NAN
		/*
		for (int iii=0; iii<numelements; iii++)
			if (isnan(imagedata[iii]))
			{
				qDebug() << "NAN";
				imagedata[iii] = 0;
			}
		*/
		// Set some default parameters (or only one for now)
		inverted = false;
		
		// Downsample the image if either axis is too large
		if (width > DOWNSAMPLE_SIZE or height > DOWNSAMPLE_SIZE){
			if (width > height)
				M = width / DOWNSAMPLE_SIZE;
			else
				M = height / DOWNSAMPLE_SIZE;
			
			// Begin downsampling
			int newW, newH;
			downsample(&imagedata, width, height, M, &newW, &newH);
			width = newW;
			height = newH;
			numelements = width * height;
		}

		// Calculate the minimum and maximum pixel values
		calculateExtremals();
		
		// Calcuate percentiles
		calculatePercentile(lowerPercentile, upperPercentile);
		
		// Initialize a working array
		renderdata = (float *) malloc(numelements * sizeof(float));
		if (!renderdata)
		{
			qDebug() << "Failed to allocate memory for the render array ...";
			free(imagedata);
			continue;
		}
		
		// Calibrate Image
		if ( !calibrateImage(LINEAR_STRETCH, vmin, vmax) )
			continue;
		
		break;

		// Found a good HDU
		qDebug() << "BAM!";
		break;
	}
	
	// Seems that no HDU was appropriate ...
	fits_close_file(fptr, &status);
	
	// Call finishInitialization from base class
	finishInitialization();
	
	return true;
}


bool FitsImage::verifyWCS()
{

	qDebug() << "Verifying World Coordinate System ...";
	// Define all the variables needed for complete WCS
	char *header;
	int ncards;
	alt = NULL;
	
	// Call cfitsio routines to get the header
	fits_hdr2str(fptr, 1, NULL, 0, &header, &ncards, &status);
	if (status)
	{
		// Free the allocated memory
		free(header);
		qDebug() << "fits_hdr2str";
		fits_report_error(stderr, status);
		return false;
	}
	
	// Pass header to WCSTools
	wcs = wcsinit(header);
	if (nowcs(wcs))
	{
		// No primary WCS found, check alternates
		int ii;
		char *alts = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		for (ii=0; ii<26; ii++)
		{
			wcs = wcsinitc(header, &alts[ii]);
			if (nowcs(wcs))
			{
				// Check if the last possible alternate
				if (ii==25)
				{
					qDebug() << "No WCS found ...";
					free(header);
					return false;
				}
				// Try next alternate
				continue;
			}
			
			// Alternate WCS found
			alt = alts[ii];
			qDebug() << "Alternate WCS found, " << alt;
			break;			
		}
	}
	
	// Set output coordinates, needed by pix2wcs
	wcsoutinit(wcs, "FK5");
	
	qDebug() << "WCS found!!!";\
	return true;
}


void FitsImage::calculateExtremals()
{
	int i;
	// Determine min and max
	for (i=0; i<numelements; i++)
	{
		if (imagedata[i] < minpixel)
			minpixel = imagedata[i];
		if (imagedata[i] > maxpixel)
			maxpixel = imagedata[i];
	}
}

// Borrowed from Astrometry.net code
void FitsImage::downsample(float** arr, int W, int H, int S, int* newW, int* newH)
{
	qDebug() << "Downsampling Factor:" << S;
	
	int i, j, I, J;
	
	*newW = (W + S-1) / S;
	*newH = (H + S-1) / S;
	
	// Average SxS blocks, placing the result in the bottom (newW * newH) first pixels.
	for (j=0; j<*newH; j++) {
        for (i=0; i<*newW; i++) {
            float sum = 0.0;
            int N = 0;
            for (J=0; J<S; J++) {
                if (j*S + J >= H)
                    break;
                for (I=0; I<S; I++) {
                    if (i*S + I >= W)
                        break;
					sum += (*arr)[(j*S + J)*W + (i*S + I)];
					N++;
                }
            }
            (*arr)[j * (*newW) + i] = sum / (float)N;
        }
    }
	*arr = (float *) realloc(*arr, (*newW) * (*newH) * sizeof(float));
	downsampled = true;
}

bool FitsImage::calculatePercentile(float lp, float up)
{		
	// Set some variables and parameters
	int ii;
	float* sample = NULL;
	long samplesize;
	int nth;
	if (downsampled)
	{
		// Determine sample size
		nth = floor(numelements/(numelements*0.004));
		samplesize = numelements / nth + 1;		
	}
	else
	{
		samplesize = numelements;
		nth = 1;
	}
	
	// Allocate memory for sample
	sample = (float *) malloc(samplesize * sizeof(float));
	if (!sample)
	{
		qDebug() << "Failed to allocate memory for the sample array ...";
		return false;
	}
	
	// Copy every nth element
	for (ii=0; ii<samplesize; ii++)
		sample[ii] = imagedata[ii*nth];
	
	// Sort using the standard library
	std::sort(&(sample)[0], &(sample)[samplesize]);
	
	// Determine quantile indices
	int vminIndex = floor(lp*(samplesize-1)+1);
	int vmaxIndex = floor(up*(samplesize-1)+1);
	int lowerLimitIndex = floor((lp-0.0015)*(samplesize-1)+1);
	int upperLimitIndex = floor((up+0.0015)*(samplesize-1)+1);
	
	// Select quantiles
	vmin = sample[vminIndex];
	vmax = sample[vmaxIndex];
	lowerLimit = sample[lowerLimitIndex];
	upperLimit = sample[upperLimitIndex];
	difference = vmax - vmin;
	
	// Free some memory and return
	free(sample);
	return true;
}


bool FitsImage::calibrateImage(int s, float minpix, float maxpix)
{
	qDebug() << "Calibrating image for display ...";
	
	// Initialize a working array
	renderdata = (float *) malloc(numelements * sizeof(float));
	if (!renderdata)
	{
		qDebug() << "Failed to allocate memory for the render array ...";
		free(imagedata);
		return false;
	}
	
	// Compute difference
	difference = maxpix - minpix;
	stretch = s;
	int i;
	switch (stretch) {
		case LOG_STRETCH:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [minpix, maxpix]
				if (renderdata[i] < minpix)
					renderdata[i] = minpix;
				if (renderdata[i] > maxpix)
					renderdata[i] = maxpix;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - minpix)/difference;
				renderdata[i] = log10(renderdata[i]/0.05 + 1.0) / log10(1.0/0.05 +1.0);
			}
			break;
		case SQRT_STRETCH:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < minpix)
					renderdata[i] = minpix;
				if (renderdata[i] > maxpix)
					renderdata[i] = maxpix;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - minpix)/difference;
				renderdata[i] = sqrt(renderdata[i]);
			}
			break;
		case ARCSINH_STRETCH:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < minpix)
					renderdata[i] = minpix;
				if (renderdata[i] > maxpix)
					renderdata[i] = maxpix;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - minpix)/difference;
				renderdata[i] = asinh(renderdata[i]/-0.033) / asinh(1.0/-0.033);
			}
			break;
		case POWER_STRETCH:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < minpix)
					renderdata[i] = minpix;
				if (renderdata[i] > maxpix)
					renderdata[i] = maxpix;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - minpix)/difference;
				renderdata[i] = pow(renderdata[i], 2);
			}
			break;
		default:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < minpix)
					renderdata[i] = minpix;
				if (renderdata[i] > maxpix)
					renderdata[i] = maxpix;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - minpix)/difference;
			}
			break;
	}
	
	// Initialize QImage with correct dimensions and data type
	QImage *image = new QImage(width, height, QImage::Format_RGB32);
	
	// Set pixels, depending on invert and BITPIX
	int ii, jj;
	if (inverted)
	{
		if (bitpix < 0)
		{
			for (ii=0; ii<height; ii++)
			{
				for (jj=0; jj<width; jj++)
				{
					long index = jj+width*(height-ii-1);
					int pixel = 255 - floor(255.0 * renderdata[index] + 0.5);
					image->setPixel(jj, ii, qRgb(pixel, pixel, pixel));
				}
			}
		}
		else
		{
			for (ii=0; ii<height; ii++)
			{
				for (jj=0; jj<width; jj++)
				{
					long index = jj+width*(height-ii-1);
					int pixel = 255 - floor(255.0 * renderdata[index] + 0.5);
					QRgb *p = (QRgb *) image->scanLine(ii) + jj;
					*p = qRgb(pixel, pixel, pixel);
				}
			}	
		}
	}
	else
	{
		if (bitpix < 0)
		{
			for (ii=0; ii<height; ii++)
			{
				for (jj=0; jj<width; jj++)
				{
					long index = jj+width*(height-ii-1);
					int pixel = floor(255.0 * renderdata[index] + 0.5);
					image->setPixel(jj, ii, qRgb(pixel, pixel, pixel));
				}
			}
		}
		else
		{
			for (ii=0; ii<height; ii++)
			{
				for (jj=0; jj<width; jj++)
				{
					long index = jj+width*(height-ii-1);
					int pixel = floor(255.0 * renderdata[index] + 0.5);
					QRgb *p = (QRgb *) image->scanLine(ii) + jj;
					*p = qRgb(pixel, pixel, pixel);
				}
			}	
		}
	}
	
	// Free some memory
	free(renderdata);
	
	// Set pixmap
	pixmap = QPixmap(QPixmap::fromImage(*image, Qt::DiffuseDither));
	
	// Delete the image
	delete image;
	
	// Emit signal to broadcast the new pixmap and slider values
	emit pixmapChanged(&pixmap);
	
	return true;
}

void FitsImage::setStretch(int s)
{
	qDebug() << "Setting Stretch";
	calibrateImage(s, vmin, vmax);
}

void FitsImage::setVmin(float minpix)
{
	qDebug() << "Setting vmin";
	vmin = minpix;
	calibrateImage(stretch, vmin, vmax);
}

void FitsImage::setVmax(float maxpix)
{
	qDebug() << "Setting vmax";
	vmax = maxpix;
	calibrateImage(stretch, vmin, vmax);	
}

void FitsImage::invert()
{
	qDebug() << "Inverting";
	QImage image = pixmap.toImage();
	image.invertPixels();
	pixmap = QPixmap::fromImage(image, Qt::DiffuseDither);
	inverted = !inverted;
	emit pixmapChanged(&pixmap);
}

// TODO: Double check this to make sure the mapping between pixels is correct.
QPointF FitsImage::fpix2pix(QPointF pos)
{
	float xf, yf;
	
	// First unbin the pixel
	xf = M*(pos.x()-1)+1;
	yf = M*(pos.y()-1)+1;
	
	// Transform QGraphicsScene pixels to FITS pixels
	xf = xf+0.5;
	yf = (naxisn[1]-yf)+0.5;
	
	return QPointF(xf, yf);
}


float FitsImage::pixelIntensity(QPointF pos)
{
	if (pos.x() < naxisn[0] && pos.y() < naxisn[1])
	{
		float xf, yf;
		
		// First unbin the pixel
		xf = M*(pos.x()-1)+1;
		yf = M*(pos.y()-1)+1;
		
		// Transform QGraphicsScene pixels to FITS pixels
		xf = xf+0.5;
		yf = (naxisn[1]-yf)+0.5;
		
		// Get the intensity of the pixel value
		int index = naxisn[0]*(floor(yf+0.5)-1) + (floor(xf+0.5)-1);
		
		return imagedata[index];
	}
	return 0;
}


void FitsImage::getCentroid(QPointF pos)
{
	qDebug() << "getCentroid starting at" << pos;
	
	// Crawl within a predefined radius (say 10 pixels) to find the brightest pixel
	float max;
	QPointF intPos = pos;
	QPointF maxPos = pos;
	QPointF testPos;
	bool changed;
	
	for (int ii=0; ii<10; ii++)
	{
		max = pixelIntensity(intPos);
		changed = false;
		
		// Determine which neighboring pixel has largest intensity
		testPos = intPos + QPointF(-1, -1);
		if (pixelIntensity(testPos) >= max)
		{
			max = pixelIntensity(testPos);
			maxPos = testPos;
			changed = true;
		}
		
		testPos = intPos + QPointF(0, -1);
		if (pixelIntensity(testPos) >= max)
		{
			max = pixelIntensity(testPos);
			maxPos = testPos;
			changed = true;
		}
		
		testPos = intPos + QPointF(1, -1);
		if (pixelIntensity(testPos) >= max)
		{
			max = pixelIntensity(testPos);
			maxPos = testPos;
			changed = true;
		}
		
		testPos = intPos + QPointF(-1, 0);
		if (pixelIntensity(testPos) >= max)
		{
			max = pixelIntensity(testPos);
			maxPos = testPos;
			changed = true;
		}
		
		testPos = intPos + QPointF(1, 0);
		if (pixelIntensity(testPos) >= max)
		{
			max = pixelIntensity(testPos);
			maxPos = testPos;
			changed = true;
		}
		
		testPos = intPos + QPointF(-1, 1);
		if (pixelIntensity(testPos) >= max)
		{
			max = pixelIntensity(testPos);
			maxPos = testPos;
			changed = true;
		}
		
		testPos = intPos + QPointF(0, 1);
		if (pixelIntensity(testPos) >= max)
		{
			max = pixelIntensity(testPos);
			maxPos = testPos;
			changed = true;
		}
		
		testPos = intPos + QPointF(1, 1);
		if (pixelIntensity(testPos) >= max)
		{
			max = pixelIntensity(testPos);
			maxPos = testPos;
			changed = true;
		}
		
		intPos = maxPos;
		
		if (!changed)
			break;
	}
	
	// Set the position to that found by walking
	pos = maxPos;
	
	// Prep data for centroid algorithm
	float *im;
	im = (float *) malloc(9 * sizeof(float));
	
	// Copy intensity values to array
	im[0] = pixelIntensity(pos + QPointF(-1, -1));
	im[1] = pixelIntensity(pos + QPointF(0, -1));
	im[2] = pixelIntensity(pos + QPointF(1, -1));
	im[3] = pixelIntensity(pos + QPointF(-1, 0));
	im[4] = pixelIntensity(pos + QPointF(0, 0));
	im[5] = pixelIntensity(pos + QPointF(1, 0));
	im[6] = pixelIntensity(pos + QPointF(-1, 1));
	im[7] = pixelIntensity(pos + QPointF(0, 1));
	im[8] = pixelIntensity(pos + QPointF(1, 1));
	
	float xcen, ycen;
	PinpointWCSUtils::cen3x3(im, &xcen, &ycen);
	free(im);
	
	// Map coordinate to image
	pos.setX(floor(pos.x())+0.5);
	pos.setY(floor(pos.y())+0.5);
	pos = pos + QPointF(-1+xcen, -1+ycen);

	emit centroid(pos);
}

double* FitsImage::pix2sky(QPointF pos)
{		
	qDebug() << "Pixel Intensity:\t" << pixelIntensity(pos);
	
	if (!wcs)
		return world;
	
	// Get unbinned pixel
	float xf, yf;
	
	// Transform from binned QPixmap pixels to FITS pixels
	// this includes a 1 and 1/2 pixel offset.
	
	// First unbin the pixel
	xf = M*(pos.x()-1)+1;
	yf = M*(pos.y()-1)+1;
	
	// Now transform QGraphicsScene pixels to FITS pixels
	xf = xf+0.5;
	yf = (naxisn[1]-yf)+0.5;
	
	pix2wcs(wcs, xf, yf, &world[0], &world[1]);
	
	// Check if coordinates are galatic
	if (wcs->syswcs != WCS_J2000)
		wcscon(wcs->syswcs, WCS_J2000, wcs->equinox, wcs->eqout, &world[0], &world[1], wcs->epoch);
	
	// Perhaps the transformationStatus needs to be checked ...
	return world;
}