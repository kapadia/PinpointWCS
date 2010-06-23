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

#include <iostream>
#include <algorithm>
#include <exception>
#include <QDebug>

#include "math.h"
#include "FitsImage.h"
#include "PinpointWCSUtils.h"

FitsImage::FitsImage(QString &fileName) : PPWcsImage()
{	
	qDebug() << "Initializing FitsImage object ...\n";
	
	// Initialize some attributes
	fptr = NULL;
	wcs = NULL;
	status = 0;
	imagedata = NULL;
	renderdata = NULL;
	lowerPercentile = 0.0025;
	upperPercentile = 0.9975;
	downsampled = false;
	
	// Open FITS file
	fits_open_file(&fptr, fileName.toStdString().c_str(), READONLY, &status);
	if (status)
	{
		fits_report_error(stderr, status);
		return;
	}
	
	// Check the number of HDUs
	fits_get_num_hdus(fptr, &numhdus, &status);
	if (status)
	{
		fits_report_error(stderr, status);
		return;
	}
	
	// Set number of images to zero
	numimgs = 0;
	
	// Begin looping over each HDU
	for (int kk=1; kk <= numhdus; kk++)
	{
		int x;
		status = 0;
		
		// Change to another HDU and check type
		fits_movabs_hdu(fptr, kk, &hdutype, &status);
		fits_get_hdu_type(fptr, &hdutype, &status);
		
		std::cout << "Header Number: " << kk << "\n";
		
		if (hdutype != IMAGE_HDU)
			continue;
		
		// Check image dimensions and size
		fits_get_img_dim(fptr, &naxis, &status);
		if (status)
		{
			std::cout << "fits_get_img_dim\n";
			fits_report_error(stderr, status);
			continue;
		}
		
		if (naxis != 2)
		{
			std::cout << "Image does not have the correct dimensions ...\n";
			continue;
		}
		
		fits_get_img_size(fptr, 2, naxisn, &status);
		if (status)
		{
			std::cout << "fits_get_img_size\n";
			fits_report_error(stderr, status);
			continue;
		}
		width = naxisn[0];
		height = naxisn[1];
		
		// Check that the image contains sufficient WCS
		if (!checkWorldCoordinateSystem())
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
		std::cout << "BITPIX: " << bitpix << "\n";
		
		// Allocate memory for the first pixel
		fpixel = (long *) malloc(naxis * sizeof(long));
		
		// Initialize the first pixel location to {1, ..., 1}
		for (x=0; x<naxis; x++)
			fpixel[x] = 1;
		
		// Allocate memory for the image pixels
		imagedata = (float *) malloc(numelements * sizeof(float));
		if (!imagedata)
		{
			std::cout << "Failed to allocate memory for the image array ...\n";
			free(fpixel);
			continue;
		}
		
		fits_read_pix(fptr, TFLOAT, fpixel, numelements, NULL, imagedata, NULL, &status);
		free(fpixel);		
		if (status)
		{
			// Free the allocated memory
			free(imagedata);
			std::cout << "fits_read_pix\n";
			fits_report_error(stderr, status);
			continue;
		}
		
		// FITS data retrieved!!!
		
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
		}

		// Calculate the minimum and maximum pixel values
		calculateExtremals();
		
		// Calcuate percentiles
		calculatePercentile(lowerPercentile, upperPercentile);
		
		// Initialize a working array
		renderdata = (float *) malloc(numelements * sizeof(float));
		if (!renderdata)
		{
			std::cout << "Failed to allocate memory for the render array ...\n";
			free(imagedata);
			continue;
		}
		
		
		// Calibrate Image
		calibrateImage(LINEAR_STRETCH);
		
		// Initialize QImage with correct dimensions and data type
		image = new QImage(width, height, QImage::Format_RGB32);
				
		int y;
		if (bitpix < 0) // Not sure why this is a problem, but without memory errors arise ...
		{
			for (x=0; x<width; x++)
			{
				for (y=0; y<height; y++)
				{	
					long index = y+width*x;
					int pixel = floor(255.0 * renderdata[index] + 0.5);
					image->setPixel(x, y, qRgb(pixel, pixel, pixel));
				}
			}
		}
		else
		{
			for (x=0; x<width; x++)
			{
				for (y=0; y<height; y++)
				{	
					long index = y+width*x;
					int pixel = floor(255.0 * renderdata[index] + 0.5);
					QRgb *p = (QRgb *) image->scanLine(x) + y;
					*p = qRgb(pixel, pixel, pixel);
				}
			}
		}
		
		// Free some allocated memory
		free(renderdata);
		
		// Flip the image
		
		// Create QPixmap
		pixmap = new QPixmap(QPixmap::fromImage(*image, Qt::DiffuseDither));
		
		// Found a good HDU
		std::cout << "BAM!\n";
		break;
	}
	
	// Seems that no HDU was appropriate ...
	fits_close_file(fptr, &status);
	
	// Call finishInit from base class
	finishInit();
}

FitsImage::~FitsImage() {}

bool FitsImage::checkWorldCoordinateSystem()
{
	std::cout << "Checking World Coordinate System ...\n";
	// Define all the variables needed for complete WCS
	char *header;
	int ncards;
	
	// Call cfitsio routines to get the header
	fits_hdr2str(fptr, 1, NULL, 0, &header, &ncards, &status);
	if (status)
	{
		// Free the allocated memory
		free(header);
		std::cout << "fits_hdr2str\n";
		fits_report_error(stderr, status);
		return false;
	}
	
	// Pass header to WCSLIB
	int nreject, nwcs;
	wcsstatus = wcspih(header, ncards, WCSHDR_all, -3, &nreject, &nwcs, &wcs);
	free(header);
	
	if (wcs == 0x0) {
		std::cout << "No world coordinate systems found ...\n";
		return false;
	}
	
	// Check the status
	if (wcsstatus == 0)
	{
		std::cout << "World coordinate systems found!\n";
		std::cout << wcs->crpix[0] << "\n";
		return true;
	}
	
	return false;
}


void FitsImage::calculateExtremals()
{
	minpix = imagedata[0];
	maxpix = imagedata[0];
	int i;
	for (i=0; i<numelements; i++)
	{
		if (imagedata[i] < minpix)
			minpix = imagedata[i];
		if (imagedata[i] > maxpix)
			maxpix = imagedata[i];
	}
}

void FitsImage::downsample(float** arr, int W, int H, int S, int* newW, int* newH)
{
	std::cout << "Downsampling data ... \n";
	
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
	
	/*
	vmin = PinpointWCSUtils::computeQuantile(imagedata, numelements, lp);
	vmax = PinpointWCSUtils::computeQuantile(imagedata, numelements, up);
	difference = vmax - vmin;
	std::cout << "VMIN: " << vmin << "\n";
	std::cout << "VMAX: " << vmax << "\n";
	return true;
	*/

	// Create a copy of the data
	long numelem = numelements;
	float* dataForSorting = NULL;
	dataForSorting = (float *) malloc(numelem * sizeof(float));
	if (!dataForSorting)
	{
		std::cout << "Failed to allocate memory for the sorting array ...\n";
		return false;
	}
	memcpy(dataForSorting, imagedata, numelem * sizeof(float));
	
	// First downsample to reduce the number of operations in sort
	if (downsampled)
	{
		int newW, newH;
		downsample(&dataForSorting, width, height, 10*M, &newW, &newH);
		numelem = newW*newH;
	}
	
	
	// Sort using standard library
	std::sort(&(dataForSorting)[0], &(dataForSorting)[numelem]);
	
	// Determine percentiles
	int vminIndex = floor(lp*(numelem-1)+1);
	int vmaxIndex= floor(up*(numelem-1)+1);
	vmin = dataForSorting[vminIndex];
	vmax = dataForSorting[vmaxIndex];
	difference = vmax - vmin;
	
	std::cout << "VMIN: " << vmin << "\n";
	std::cout << "VMAX: " << vmax << "\n";
	
	free(dataForSorting);
	return true;
}

void FitsImage::calibrateImage(int stretch)
{
	std::cout << "Calibrating image for display ...\n";
	
	int i;
	switch (stretch) {
		case LOG_STRETCH:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < vmin)
					renderdata[i] = vmin;
				if (renderdata[i] > vmax)
					renderdata[i] = vmax;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - vmin)/difference;
				renderdata[i] = log10(renderdata[i]/0.05 + 1.0) / log10(1.0/0.05 +1.0);
			}
			break;
		case SQRT_STRETCH:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < vmin)
					renderdata[i] = vmin;
				if (renderdata[i] > vmax)
					renderdata[i] = vmax;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - vmin)/difference;
				renderdata[i] = sqrt(renderdata[i]);
			}
			break;
		case ARCSINH_STRETCH:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < vmin)
					renderdata[i] = vmin;
				if (renderdata[i] > vmax)
					renderdata[i] = vmax;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - vmin)/difference;
				renderdata[i] = asinh(renderdata[i]/-0.033) / asinh(1.0/-0.033);
			}
			break;
		case POWER_STRETCH:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < vmin)
					renderdata[i] = vmin;
				if (renderdata[i] > vmax)
					renderdata[i] = vmax;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - vmin)/difference;
				renderdata[i] = pow(renderdata[i], 2);
			}
			break;
		default:
			for (i = 0; i < numelements; i++)
			{
				// Copy the array value
				renderdata[i] = imagedata[i];
				
				// First clip values outside of the interval [min, max]
				if (renderdata[i] < vmin)
					renderdata[i] = vmin;
				if (renderdata[i] > vmax)
					renderdata[i] = vmax;
				
				// Scale the array
				renderdata[i] = (renderdata[i] - vmin)/difference;
			}
			break;
	}
}
