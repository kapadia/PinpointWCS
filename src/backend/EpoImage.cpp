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
	
	inverted = false;
	// Call finishInit from base class
	finishInitialization();
}

EpoImage::~EpoImage()
{
	delete pixmap;
}

void EpoImage::invert()
{
	qDebug() << "Inverting EPO Image ...";
	QImage image = pixmap->toImage();
	image.invertPixels(QImage::InvertRgb);
	*pixmap = QPixmap::fromImage(image, Qt::DiffuseDither);
	inverted = !inverted;
	emit pixmapChanged(pixmap);
}

double* EpoImage::pix2sky(QPointF pos)
{
	if (!wcs)
		return world;
	
	// Get unbinned pixel
	float xf, yf;
	
	// Transform QGraphicsScene pixels to FITS pixels
        xf = pos.x() + 0.5;
        yf = naxisn[1] - pos.y() + 0.5;

	pix2wcs(wcs, xf, yf, &world[0], &world[1]);
	
	// Perhaps the transformationStatus needs to be checked ...
	return world;
}
