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
#include "PPWcsImage.h"

PPWcsImage::PPWcsImage()
{	
	qDebug() << "Initializing PPWcsImage object ...";
	
	// Initialize attributes common to all base classes
	wcs = NULL;
}

PPWcsImage::~PPWcsImage() {}

void PPWcsImage::finishInitialization()
{
	world = (double*) malloc( 2*sizeof(double) );
}


double* PPWcsImage::pix2sky(QPointF pos)
{
	if (!wcs)
		return world;
	
//	qDebug() << pos;
	xpix = pos.x();
	ypix = wcs->nypix - pos.y();
	pix2wcs(wcs, xpix, ypix, &world[0], &world[1]);
	
	// Perhaps the transformationStatus needs to be checked ...
	return world;
}