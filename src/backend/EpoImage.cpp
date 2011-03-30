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
#include "EpoImage.h"


EpoImage::EpoImage(QString filename) : PPWcsImage()
{
	qDebug() << "Initializing EpoImage object ...";
	pixmap = new QPixmap(filename);
	naxisn[0] = pixmap->width();
	naxisn[1] = pixmap->height();
	
	// Call finishInit from base class
	finishInitialization();
}

EpoImage::~EpoImage()
{}

// FIXME: Pixel mapping not correct.  Find reason!!!
double* EpoImage::pix2sky(QPointF pos)
{
	if (!wcs)
		return world;
	
	// Get unbinned pixel
	float xf, yf;
	xf = M*(pos.x()-1)+2-0.5;
	yf = naxisn[1]-(M*(pos.y()-1)-0.5);
	
	pix2wcs(wcs, xf, yf, &world[0], &world[1]);
	
	// TODO: Support galatic coordinates
	if (wcs->syswcs != WCS_J2000)
		wcscon(wcs->syswcs, WCS_J2000, wcs->equinox, wcs->eqout, &world[0], &world[1], wcs->epoch);
	
	// Perhaps the transformationStatus needs to be checked ...
	return world;
}