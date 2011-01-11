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
#include <string>
#include <QDebug>
#include <QImage>
#include "fitsio.h"
#include "version.h"
#include "ExportWCS.h"

// Must be defined to instantiate template classes
#define TXMP_STRING_TYPE std::string 

// Must be defined to give access to XMPFiles
#define XMP_INCLUDE_XMPFILES 1 

// Ensure XMP templates are instantiated
#include "XMP.incl_cpp"

// Provide access to the API
#include "XMP.hpp"

#include <iostream>
#include <fstream>

// Define the AVM namespace
#define kXMP_NS_AVM "http://www.communicatingastronomy.org/avm/1.0/"
#define kXMP_NS_CXC "www.cfa.harvard.edu/~akapadia/pinpointwcs/"


ExportWCS::ExportWCS(QString *f, QPixmap *p, ComputeWCS *cwcs)
{
	filename = f;
	pixmap = p;
	computewcs = cwcs;
}


ExportWCS::~ExportWCS()
{}


void ExportWCS::setWCS(struct WorldCoor *w)
{
	wcs = w;
}


void ExportWCS::clearWCS()
{
	wcs = NULL;
}


void ExportWCS::exportFITS()
{	
	// Initialize variables
	fitsfile *fptr;
	int status = 0;
	int ii, jj;
	long fpixel, nelements;
	unsigned short *imagedata[pixmap->height()];
	QImage im = pixmap->toImage();
	
	// Prompt user for filename
	QString saveas = QFileDialog::getSaveFileName(NULL, "Export FITS Image", *filename+"_ppwcs.fits", "Images(*.fit *.fits)", NULL, NULL);
	
	// Initialize FITS image parameters
	int bitpix = BYTE_IMG;
	long naxis = 2;
	long naxes[2] = {pixmap->width(), pixmap->height()};
	
	// Allocate memory for the entire image
	imagedata[0] = (unsigned short *) malloc(naxes[0] * naxes[1] * sizeof(unsigned short));
	if (!imagedata)
		emit exportResults(false);
	
	// Initialize pointers to the start of each row of the image
    for(ii=1; ii<naxes[1]; ii++)
		imagedata[ii] = imagedata[ii-1] + naxes[0];
	
	// Remove file is it already exists
	remove(saveas.toStdString().c_str());
	
	// Attempt to create the FITS file (writes to disk)
	if (fits_create_file(&fptr, saveas.toStdString().c_str(), &status))
		emit exportResults(false);
	
	// Attempt to create the image HDU
	if (fits_create_img(fptr,  bitpix, naxis, naxes, &status))
        emit exportResults(false);
	
	// Initialize the values in the image using the QPixmaps
    for (jj = 0; jj < naxes[1]; jj++)
		for (ii = 0; ii < naxes[0]; ii++)
			imagedata[jj][ii] = qGray(im.pixel(ii, naxes[1]-jj-1));
	
	// First pixel and number of pixels to write
	fpixel = 1;
	nelements = naxes[0] * naxes[1];
	
	// Write the array to the FITS image
    if (fits_write_img(fptr, TUSHORT, fpixel, nelements, imagedata[0], &status))
		emit exportResults(false);
	
	// Free memory
	free(imagedata[0]);
	
	// Initialize variables for the FITS header
	char xtension[] = "IMAGE";
	char origin[] = "PinpointWCS by the Chandra X-ray Center";
	char wcsname[] = "Primary WCS";
	char ctype1[] = "RA---TAN";
	char ctype2[] = "DEC--TAN";
	char cunit[] = "deg";
	
	// TSTRING, TLOGICAL (== int), TBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TFLOAT, TDOUBLE
	if (fits_update_key(fptr, TSTRING, "XTENSION", &xtension, NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "ORIGIN", &origin, NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TINT, "WCSAXES", &naxis, NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "WCSNAME", &wcsname, NULL, &status))
		emit exportResults(false);
	
	// Initialize QStrings for some numeric values
	QString equinox;
	QString crpix1;
	QString crval1;
	QString crpix2;
	QString crval2;
	QString cd11;
	QString cd12;
	QString cd21;
	QString cd22;
	
	// Format values to specific digits
	equinox.sprintf("%.1f", wcs->equinox);
	crpix1.sprintf("%.11f", wcs->xrefpix);
	crval1.sprintf("%.11f", wcs->xref);
	crpix2.sprintf("%.11f", wcs->yrefpix);
	crval2.sprintf("%.11f", wcs->yref);
	cd11.sprintf("%.11f", wcs->cd[0]);
	cd12.sprintf("%.11f", wcs->cd[1]);
	cd21.sprintf("%.11f", wcs->cd[2]);
	cd22.sprintf("%.11f", wcs->cd[3]);

	if (fits_update_key(fptr, TSTRING, "EQUINOX", (void*) equinox.toStdString().c_str(), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "RADESYS", &(wcs->radecsys), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CTYPE1", &ctype1, NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CRPIX1", (void*) crpix1.toStdString().c_str(), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CRVAL1", (void*) crval1.toStdString().c_str(), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CUNIT1", &cunit, NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CTYPE2", &ctype2, NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CRPIX2", (void*) crpix2.toStdString().c_str(), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CRVAL2", (void*) crval2.toStdString().c_str(), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CUNIT2", &cunit, NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CD1_1", (void*) cd11.toStdString().c_str(), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CD1_2", (void*) cd21.toStdString().c_str(), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CD2_1", (void*) cd12.toStdString().c_str(), NULL, &status))
		emit exportResults(false);
	if (fits_update_key(fptr, TSTRING, "CD2_2", (void*) cd22.toStdString().c_str(), NULL, &status))
		emit exportResults(false);

	// Write comments
	if (fits_write_comment(fptr, "World Coordinate System computed using PinpointWCS by the Chandra X-ray Center.  PinpointWCS is developed and maintained by Amit Kapadia (CfA) akapadia@cfa.harvard.edu.", &status))
		emit exportResults(false);
	
	// Close FITS file
	if (fits_close_file(fptr, &status))
		emit exportResults(false);
	
	emit exportResults(true);
}


void ExportWCS::exportAVM()
{
	qDebug() << "Attempting to export AVM ...";
	
	std::string f = filename->toStdString();
	
	// Initialize the Adobe XMP Toolkit
	if (!SXMPMeta::Initialize())
		emit exportResults(false);
	
	// Set some options
	XMP_OptionBits options = 0;
#if UNIX_ENV
	options |= kXMPFiles_ServerMode;
#endif
	
	// Initialize SXMPFiles
	if (SXMPFiles::Initialize(options))
	{
		try
		{
			// Put the good stuff here
			
			// Set some options that will be used to open the file
			XMP_OptionBits opts = kXMPFiles_OpenForUpdate|kXMPFiles_OpenUseSmartHandler;
			
			// Initialize some variables
			bool ok;
			SXMPFiles epoimage;
			
			// Open the file
			ok = epoimage.OpenFile(f, kXMP_UnknownFile, opts);
			if (!ok)
			{
				qDebug() << "No smart handler available for the file.";
				qDebug() << "Trying packet scanning ...";
				
				// Packet scanning technique
				opts = kXMPFiles_OpenForUpdate|kXMPFiles_OpenUsePacketScanning;
				ok = epoimage.OpenFile(f, kXMP_UnknownFile, opts);
			}
			
			// Procede if file is open
			if (ok)
			{
				qDebug() << "All is good.";
				
				// Create XMP object and read XMP from file
				SXMPMeta avm;
				epoimage.GetXMP(&avm);
				
				// Register namespaces
				std::string avmprefix;
				std::string cxcprefix;
				SXMPMeta::RegisterNamespace(kXMP_NS_AVM, "avm", &avmprefix);
				SXMPMeta::RegisterNamespace(kXMP_NS_CXC, "cxc", &cxcprefix);
				
				// Clean the existing Coordinate Metadata
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.CoordinateFrame");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.Equinox");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.ReferenceValue");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.ReferenceDimension");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.ReferencePixel");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.Scale");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.Rotation");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.CoordsystemProjection");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.Quality");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.Notes");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.FITSheader");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Spatial.CDMatrix");
				
				// Clean existing Publisher Metadata
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Publisher.MetadataDate");
				avm.DeleteProperty(kXMP_NS_AVM, "avm:Publisher.MetadataVersion");
				
				// Clean the existing CXC Metadata
				avm.DeleteProperty(kXMP_NS_CXC, "cxc:WCSResolver");
				avm.DeleteProperty(kXMP_NS_CXC, "cxc:WCSResolverVersion");
				avm.DeleteProperty(kXMP_NS_CXC, "cxc:WCSResolverRevision");
				
				// Initialize QStrings to format the WCS data
				QString equinox;
				QString crval1;
				QString crval2;
				QString crpix1;
				QString crpix2;
				QString scale;
				QString orientation;
				QString cd11;
				QString cd12;
				QString cd21;
				QString cd22;
				QString width;
				QString height;
				QString spatialnotes;
				QString metadatadate;
				
				// Format values to specific digits
				equinox.sprintf("%.1f", wcs->equinox);
				crval1.sprintf("%.11f", wcs->xref);
				crval2.sprintf("%.11f", wcs->yref);
				crpix1.sprintf("%.11f", wcs->xrefpix);
				crpix2.sprintf("%.11f", wcs->yrefpix);
				scale.sprintf("%.11f", computewcs->scale);
				orientation.sprintf("%.11f", computewcs->orientation);
				cd11.sprintf("%.11f", wcs->cd[0]);
				cd12.sprintf("%.11f", wcs->cd[1]);
				cd21.sprintf("%.11f", wcs->cd[2]);
				cd22.sprintf("%.11f", wcs->cd[3]);
				width.sprintf("%.2f", computewcs->width);
				height.sprintf("%.2f", computewcs->height);
				spatialnotes.sprintf("World Coordinate System resolved using PinpointWCS %s revision %s by the Chandra X-ray Center", VERSION, REVISION);
				
				
				// Begin modifying AVM
				XMP_OptionBits itemOptions;
				itemOptions = kXMP_PropValueIsArray|kXMP_PropArrayIsOrdered;
				
				// Set the Coordinate Metadata
				avm.SetProperty(kXMP_NS_AVM, "avm:Spatial.CoordinateFrame", "ICRS", 0);
				avm.SetProperty(kXMP_NS_AVM, "avm:Spatial.Equinox", equinox.toStdString(), 0);
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.ReferenceValue", itemOptions, crval1.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.ReferenceValue", itemOptions, crval2.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.ReferenceDimension", itemOptions, width.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.ReferenceDimension", itemOptions, height.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.ReferencePixel", itemOptions, crpix1.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.ReferencePixel", itemOptions, crpix2.toStdString());				
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.Scale", itemOptions, scale.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.Scale", itemOptions, scale.toStdString());
				avm.SetProperty(kXMP_NS_AVM, "avm:Spatial.Rotation", orientation.toStdString(), 0);
				avm.SetProperty(kXMP_NS_AVM, "avm:Spatial.CoordsystemProjection", "TAN", 0);
				avm.SetProperty(kXMP_NS_AVM, "avm:Spatial.Quality", "Full", 0);
				avm.SetLocalizedText(kXMP_NS_AVM, "avm:Spatial.Notes", "x-default", "x-default", spatialnotes.toStdString(), 0);
//				avm.SetProperty(kXMP_NS_AVM, "avm:Spatial.FITSheader", "SPATIAL FITS HEADER TEST", 0);
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.CDMatrix", itemOptions, cd11.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.CDMatrix", itemOptions, cd12.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.CDMatrix", itemOptions, cd21.toStdString());
				avm.AppendArrayItem(kXMP_NS_AVM, "avm:Spatial.CDMatrix", itemOptions, cd22.toStdString());
				
				// Set Publisher Metadata
				XMP_DateTime updatedTime;
				SXMPUtils::CurrentDateTime(&updatedTime);
				avm.SetProperty_Date(kXMP_NS_AVM, "avm:MetadataDate", updatedTime, 0);
				avm.SetProperty(kXMP_NS_AVM, "avm:MetadataVersion", AVM_VERSION, 0);
				
				// Set CXC Metadata :)
				avm.SetProperty(kXMP_NS_CXC, "cxc:WCSResolver", "PinpointWCS developed by the Chandra X-ray Center", 0);
				avm.SetProperty(kXMP_NS_CXC, "cxc:WCSResolverVersion", VERSION, 0);
				avm.SetProperty(kXMP_NS_CXC, "cxc:WCSResolverRevision", REVISION, 0);
				
				// Write XMP object to file
				if (epoimage.CanPutXMP(avm))
					epoimage.PutXMP(avm);
				
				// Close file
				epoimage.CloseFile();
				
				// Broadcast results
				emit exportResults(true);
			}
			else
				emit exportResults(false);
		}
		catch (XMP_Error &e)
		{
			std::cout << "Error: " << e.GetErrMsg() << std::endl;
			
			// Broadcast results
			emit exportResults(false);
		}
		
		// Terminate the XMP Toolkit
		SXMPFiles::Terminate();
		SXMPMeta::Terminate();
	}
	else
	{
		qDebug() << "Could not intialize SXMPFiles!";
		
		// Broadcast results
		emit exportResults(false);
	}
}