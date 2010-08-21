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

#include <QImage>
#include "ExportWCS.h"
#include "fitsio.h"

ExportWCS::ExportWCS(QPixmap *p)
{
	pixmap = p;
}


ExportWCS::~ExportWCS()
{}


bool ExportWCS::exportFITS(struct WorldCoor *wcs)
{
	// Initialize variables
	fitsfile *fptr;
	int status = 0;
	int ii, jj;
	long fpixel, nelements;
	unsigned short *imagedata[pixmap->height()];
	QImage im = pixmap->toImage();

	// Initialize FITS image parameters
	char filename[] = "/Users/akapadia/Desktop/whatwhat.fits";
	int bitpix = BYTE_IMG;
	long naxis = 2;
	long naxes[2] = {pixmap->width(), pixmap->height()};
	
	// Allocate memory for the entire image
	imagedata[0] = (unsigned short *) malloc(naxes[0] * naxes[1] * sizeof(unsigned short));
	if (!imagedata)
		return false;
	
	// Initialize pointers to the start of each row of the image
    for(ii=1; ii<naxes[1]; ii++)
		imagedata[ii] = imagedata[ii-1] + naxes[0];
	
	// Remove file is it already exists
	remove(filename);
	
	// Attempt to create the FITS file (writes to disk)
	if (fits_create_file(&fptr, filename, &status))
		return false;
	
	// Attempt to create the image HDU
	if (fits_create_img(fptr,  bitpix, naxis, naxes, &status))
        return false;
	
	// Initialize the values in the image using the QPixmaps
    for (jj = 0; jj < naxes[1]; jj++)
		for (ii = 0; ii < naxes[0]; ii++)
			imagedata[jj][ii] = qGray(im.pixel(ii, naxes[1]-jj));
	
	// First pixel and number of pixels to write
	fpixel = 1;
	nelements = naxes[0] * naxes[1];
	
	// Write the array to the FITS image
    if (fits_write_img(fptr, TUSHORT, fpixel, nelements, imagedata[0], &status))
		return false;
	
	// Free memory
	free(imagedata[0]);
	
	// Initialize variables for the FITS header
	char xtension[] = "IMAGE";
	char origin[] = "Chandra X-ray Center's PinpointWCS";
	char wcsname[] = "Primary WCS";
	float equinox = wcs->equinox;
	char ctype1[] = "RA---TAN";
	char ctype2[] = "DEC--TAN";
	float crpix1 = wcs->xrefpix;
	float crval1 = wcs->xref;
	char cunit[] = "deg";
	float crpix2 = wcs->yrefpix;
	float crval2 = wcs->yref;
	double cd[4] = {1.0, 0.0, 1.0, 0.0};
	
	// TSTRING, TLOGICAL (== int), TBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TFLOAT, TDOUBLE
	if (fits_update_key(fptr, TSTRING, "XTENSION", &xtension, NULL, &status))
		return false;
	if (fits_update_key(fptr, TSTRING, "ORIGIN", &origin, NULL, &status))
		return false;
	if (fits_update_key(fptr, TINT, "WCSAXES", &naxis, NULL, &status))
		return false;
	if (fits_update_key(fptr, TSTRING, "WCSNAME", &wcsname, NULL, &status))
		return false;
	if (fits_update_key(fptr, TFLOAT, "EQUINOX", &equinox, NULL, &status))
		return false;
	if (fits_update_key(fptr, TSTRING, "RADESYS", &(wcs->radecsys), NULL, &status))
		return false;
	if (fits_update_key(fptr, TSTRING, "CTYPE1", &ctype1, NULL, &status))
		return false;
	if (fits_update_key(fptr, TSTRING, "CTYPE2", &ctype2, NULL, &status))
		return false;
	
	
	// Write comments
	if (fits_write_comment(fptr, "World Coordinate System computed using PinpointWCS by the Chandra X-ray Center.  PinpointWCS is being developed by Amit Kapadia (CfA) akapadia@cfa.harvard.edu.", &status))
		return false;
	
	// Close FITS file
	if (fits_close_file(fptr, &status))
		return false;
	
	return true;
}


bool ExportWCS::exportAVM(struct WorldCoor *wcs)
{
	return true;
}