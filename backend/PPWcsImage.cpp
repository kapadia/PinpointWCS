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
#include <QDebug>

#include "PPWcsImage.h"

PPWcsImage::PPWcsImage()
{	
	qDebug() << "Initializing PPWcsImage object ...\n";
	
	// Initialize attributes common to all base classes
	wcs = NULL;
	wcsExists = false; // Might be able to get ride of this boolean, depending on how if(wcs) works ...
	double *pixcrd = NULL;
	double *imgcrd = NULL;
	double phi, theta = NULL;
	double *world = NULL;
	int *transformStatus = NULL;
}

PPWcsImage::~PPWcsImage() {}

void PPWcsImage::finishInit()
{
	pixcrd = (double *) malloc(2 * sizeof(double));
	imgcrd = (double *) malloc(2 * sizeof(double));
	world = (double *) malloc(2 * sizeof(double));
	transformStatus = (int *) malloc(2 * sizeof(int));
}

double* PPWcsImage::pix2sky(QPointF pos)
{
	
	if (!wcs)
		return world;
	
	// Write the xy pixel coordinate to variable
	pixcrd[0] = pos.x();
	pixcrd[1] = pos.y();
	
	// Call pixel to sky function from WCSLIB
	wcsp2s(wcs, 1, 2, pixcrd, imgcrd, &phi, &theta, world, transformStatus);
	
	// Perhaps the transformationStatus needs to be checked ...
	return world;
}