#include <iostream>
#include <QImage>

#include "math.h"
#include "FitsImage.h"

FitsImage::FitsImage(QString &fileName)
{	
	std::cout << "Initializing FitsImage object ...\n";
	
	// Initialize some attributes
	fptr = NULL;
	status = 0;
	thedata = NULL;
	
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
		
		// Change to another HDU and check type
		fits_movabs_hdu(fptr, kk, &hdutype, &status);
		fits_get_hdu_type(fptr, &hdutype, &status);
		
		if (hdutype != IMAGE_HDU)
			continue;
		
		// Check image dimensions and size
		fits_get_img_dim(fptr, &naxis, &status);
		if (status)
		{
			fits_report_error(stderr, status);
			continue;
		}
		
		fits_get_img_size(fptr, 2, naxisn, &status);
		if (status)
		{
			fits_report_error(stderr, status);
			continue;
		}
		
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
		thedata = (float *) malloc(naxisn[0] * naxisn[1] * sizeof(float));
		if (!thedata)
		{
			std::cout << "Failed to allocate memory for the image array ...\n";
			continue;
		}
		
		fits_read_pix(fptr, TFLOAT, fpixel, naxisn[0]*naxisn[1], NULL, thedata, NULL, &status);
		free(fpixel);
		
		if (status)
		{
			fits_report_error(stderr, status);
			continue;
		}
		
		// Now we have the data from the FITS image
		// need to determine the min and max pixel values
		// to normalize to a value between [0, 255]
		minpix = thedata[0];
		maxpix = thedata[0];
		for (x=0; x<naxisn[0]*naxisn[1]; x++)
		{
			if (thedata[x] < minpix)
				minpix = thedata[x];
			if (thedata[x] > maxpix)
				maxpix = thedata[x];
		}
		
		diff = maxpix - minpix;
		
		// Loop over the pixel values to normalize them
		// then add it to a QImage
		this->image = new QImage(naxisn[0], naxisn[1], QImage::Format_RGB32);
//		std::cout << "NAXIS1: " << this->image->size().width() << "\n";
//		std::cout << "NAXIS2: " << this->image->size().height() << "\n";
		
//		QRgb pixval;
		int index;
		float prepixel;
		int pixel;
		int y;
		for (x=0; x<naxisn[0]; x++)
		{
			for (y=0; y<naxisn[1]; y++)
			{	
				index = y+naxisn[0]*x;
				prepixel = ((thedata[index] - minpix) / diff)*255;
				pixel = floor(prepixel + 0.5);
//				pixval = qRgb(pixel, pixel, pixel);
//				std::cout << pixval << "\n";
				uint *p = (uint *) this->image->scanLine(x) + y;
				*p = qRgb(pixel, pixel, pixel);
//				this->image->setPixel(b, a, pixval);
			}
		}
		
		// Free the allocated memory
		free(thedata);
		
		// Found a good HDU
		break;
	}
	
	// Seems that no HDU was appropriate ...
	fits_close_file(fptr, &status);
}

FitsImage::~FitsImage() {}

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

