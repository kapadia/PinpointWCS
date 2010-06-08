#include <iostream>
#include <algorithm>
#include <QImage>

#include "math.h"
#include "FitsImage.h"

FitsImage::FitsImage(QString &fileName)
{	
	std::cout << "Initializing FitsImage object ...\n";
	
	// Initialize some attributes
	fptr = NULL;
	imagedata = NULL;
	renderdata = NULL;
	lowerPercentile = 0.0025;
	upperPercentile = 0.9975;
	
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
		
		std::cout << "Header: " << kk << "\n";
		
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
		
		fits_get_img_size(fptr, 2, naxisn, &status);
		if (status)
		{
			std::cout << "fits_get_img_size\n";
			fits_report_error(stderr, status);
			continue;
		}
		// Compute the total number of pixels in image array
		numelements = naxisn[0]*naxisn[1];
		
		// The HDU is a valid image, increment
		numimgs++;
		
		// Determine the data type of image (i.e. bitpix)
		fits_get_img_type(fptr, &bitpix, &status);
		if (status)
		{
			fits_report_error(stderr, status);
			continue;
		}
		
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
		
		// Calculate the minimum and maximum pixel values
		calculateExtremals();
		std::cout << "MIN: " << minpix << "\n";
		std::cout << "MAX: " << maxpix << "\n";
		
		// Calcuate percentiles
		calculatePercentile(lowerPercentile, upperPercentile);
		
		// Initialize a working array
		renderdata = (float *) malloc(numelements * sizeof(float));
		
		// Calibrate Image
		calibrateImage(LINEAR_STRETCH);
		
		// Initialize QImage with correct dimensions and data type
		this->image = new QImage(naxisn[0], naxisn[1], QImage::Format_RGB32);
//		std::cout << "NAXIS1: " << this->image->size().width() << "\n";
//		std::cout << "NAXIS2: " << this->image->size().height() << "\n";
		
		int index;
		float prepixel;
		int pixel;
		int y;
		for (x=0; x<naxisn[0]; x++)
		{
			for (y=0; y<naxisn[1]; y++)
			{	
				index = y+naxisn[0]*x;
//				prepixel = (255.0/scaledMaxPix)*theScaledData[index];
//				if (prepixel < 0)
//					prepixel = 0;
//				prepixel = ((thedata[index] - minpix) / diff)*255;
//				pixel = floor(prepixel + 0.5);
//				std::cout << pixel << "\n";
				uint *p = (uint *) this->image->scanLine(x) + y;
				*p = qRgb(renderdata[index], renderdata[index], renderdata[index]);
			}
		}
		
		// Free the allocated memory
		free(renderdata);
		// Found a good HDU
		break;
	}
	
	// Seems that no HDU was appropriate ...
	fits_close_file(fptr, &status);
}

FitsImage::~FitsImage() {}

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
	difference = minpix - maxpix;
}

void FitsImage::convolve()
{
	std::cout << "Convolving data ... \n";
}

bool FitsImage::calculatePercentile(float lp, float up)
{
	// Create a sparse copy of the data
	float* dataForSorting;
	int ii;
	dataForSorting = (float *) malloc(numelements * sizeof(float));
	
	for (ii=0; ii < numelements; ii++)
		dataForSorting[ii] = imagedata[ii];
	
	// Sort using standard library
	std::sort(&(dataForSorting)[0], &(dataForSorting)[numelements]);
	
	// Determine percentiles
	int vminIndex = floor(lp*(numelements-1)+1);
	int vmaxIndex= floor(up*(numelements-1)+1);
	vmin = dataForSorting[vminIndex];
	vmax = dataForSorting[vmaxIndex];
	
	std::cout << "VMIN: " << vmin << "\n";
	std::cout << "VMAX: " << vmax << "\n";
	
	free(dataForSorting);
	return true;
}

void FitsImage::calibrateImage(int stretch)
{
	std::cout << "Calibrating image for display ...\n";

	int i;	
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
		renderdata[i] = (imagedata[i] - minpix)/difference;
		
		std::cout << renderdata[i] << "\n";
		
		// Stretch the array
		switch (stretch) {
			case LINEAR_STRETCH:
//				std::cout << "Linear Stretch ...\n";
				break;
			case LOG_STRETCH:
//				std::cout << "Logarithm Stretch ...\n";
				renderdata[i] = log10(renderdata[i]/0.05 + 1.0) / log10(1.0/0.05 +1.0);
				break;
			case SQRT_STRETCH:
//				std::cout << "Square Root Stretch ...\n";
				renderdata[i] = sqrt(renderdata[i]);
				break;
			case ARCSINH_STRETCH:
//				std::cout << "Inverse Hyperbolic Sine Stretch ...\n";
				renderdata[i] = asinh(renderdata[i]/-0.033) / asinh(1.0/-0.033);
				break;
			case POWER_STRETCH:
//				std::cout << "Power Stretch ...\n";
				renderdata[i] = pow(renderdata[i], 2);
				break;
			default:
				break;
		}
	}
	
	// Normalize the array
	normalize();
}

void FitsImage::normalize()
{
	// First need to determine the current maximum value
	int i;
	float max = renderdata[0];
	for (i=0; i<numelements; i++)
		if (renderdata[i] > max)
			max = renderdata[i];
	
	// Normalize
	for (i=0; i<numelements; i++)
		renderdata[i] = floor((255.0/max)*renderdata[i] + 0.5);
}

// BORROWED FROM FABIEN'S CODE

// May need to use his method when dealing with FITS image
// of various BITPIX.
/*
 bool MainWindow::loadFitsImage(QString& filename)
 {
 std::cout << "Loading FITS image ...\n";
 
 // Some necessary variables when loading a FITS image.
 fitsfile *fptr;
 int status = 0;
 int bitpix, naxis;
 long naxes[2] = {1, 1}, numpix=0, firstpix[2] = {1,1};
 
 // Open FITS image
 fits_open_file(&fptr, filename.toStdString().c_str(), READONLY, &status);
 
 // Check status
 if (status)
 {
 fits_report_error(stderr, status);
 return false;
 }
 
 fits_get_img_param(fptr, 3, &bitpix, &naxis, naxes, &status);
 if (status)
 {
 fits_report_error(stderr, status);
 return false;
 }
 
 if (naxis==2)
 numpix = naxes[0]*naxes[1];
 else if (naxis > 2)
 {
 std::cout << "Cannot load higher dimensional FITS image ...\n";
 fits_close_file(fptr, &status);
 return false;
 }
 
 double *pixels;
 pixels = (double *) malloc(numpix * sizeof(double));
 if (pixels == NULL)
 {
 std::cout << "Memory allocation error ...\n";
 return(1);
 }
 
 
 fits_read_pix(fptr, TDOUBLE, firstpix, numpix, NULL, pixels, NULL, &status);
 if (status)
 {
 fits_report_error(stderr, status);
 return false;
 }
 
 std::cout << pixels << "\n";
 
 
 
 int bytePix = (bitpix>0) ? bitpix/8 : -bitpix/8;
 GLubyte* data = (GLubyte*)calloc(bytePix, numpix);
 if (!data)
 {
 std::cout << "Insufficient memory for FITS image data allocation ...\n";
 fits_close_file(fptr, &status);
 return false;
 }
 
 if (bitpix==8)
 {
 fits_read_pix(fptr, TBYTE, firstpix, numpix, NULL, data, NULL, &status);
 }
 else if (bitpix==16)
 {
 int tmp;
 fits_get_img_equivtype(fptr, &tmp, &status);
 if (tmp==USHORT_IMG)
 {
 fits_read_pix(fptr, TUSHORT, firstpix, numpix, NULL, data, NULL, &status);
 }
 else if (tmp==SHORT_IMG)
 {
 fits_read_pix(fptr, TSHORT, firstpix, numpix, NULL, data, NULL, &status);
 }
 else
 {
 std::cout << "Unable to load the FITS image ...\n";
 }
 }
 else if (bitpix==32)
 {
 fits_read_pix(fptr, TINT, firstpix, numpix, NULL, data, NULL, &status);
 }
 else if (bitpix==-32)
 {
 fits_read_pix(fptr, TFLOAT, firstpix, numpix, NULL, data, NULL, &status);
 }
 else
 {
 std::cout << "Can not read FITS files with bitpix = " << bitpix;
 fits_close_file(fptr, &status);
 free(data);
 return false;
 }
 
 if (status)
 {
 fits_report_error(stderr, status);
 fits_close_file(fptr, &status);
 free(data);
 return false;
 }
 
 
 // We now have our image loaded in row major format in data
 std::cout << "Got here with out false returns!!";
 fits_close_file(fptr, &status);
 
 // Load to a QPixmap ?!?!?!
 //std::cout << data;
 //	ui.graphicsView->setup(QPixmap().loadFromData(pixels), true);
 
 return true;
 }
 */

