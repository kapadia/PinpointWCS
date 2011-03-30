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

// FIXME: Compute WCS Correctly!!!

#include <math.h>
#include <Eigen/LU>
#include <QDebug>
#include "ComputeWCS.h"
#include "math.h"
#include "PinpointWCSUtils.h"


ComputeWCS::ComputeWCS(QList<QPointF> *ref, QList<QPointF> *epo, struct WorldCoor *refWCS, double w, double h)
{
	qDebug() << "Initializing ComputeWCS ...";
	refCoords = ref;
	epoCoords = epo;
	referenceWCS = refWCS;
	epoWCS = false;
	mappingExists = false;
	width = w;
	height = h;
	
	// Flip matrix
	flip << 0, 1, 1, 0;
	
	// Vector to be used to compute offsets
	v << 1000.0, 0;
}

ComputeWCS::~ComputeWCS()
{}

void ComputeWCS::initializeMatrixVectors(int d)
{
	degree = d;
	int size = 3*degree;
	
	basis = VectorXd::Zero(size);
	matrix = MatrixXd::Zero(size, size);
	xvector = VectorXd::Zero(size);
	yvector = VectorXd::Zero(size);
	xcoeff = VectorXd::Zero(size);
	ycoeff = VectorXd::Zero(size);
}

void ComputeWCS::computeTargetWCS()
{

	qDebug() << "Attempting to compute EPO WCS ...";
	// Check if enough points have been selected
	if (epoCoords->size() >= 3 && epoCoords->last() != QPointF(-1, -1))
	{
		// Compute matrix and vectors
		computeSums();
		
		// Solve matrix equation for mapping
		plateSolution();
		
		// Compute residuals
		computeResiduals();
		
		// Declare the CRPIX for the EPO image to be the center pixel
		// Maybe this can eventually be set by the user
		crpix << width / 2., height / 2.;
		
		// Determine corresponding pixel in the FITS image
		Vector2d ref0 = epoToFits(crpix);
		std::cout << "ref0:\t" << ref0 << std::endl;
		Vector2d xieta_0 = xi_eta(ref0);
		std::cout << "xieta_0:\t" << xieta_0 << std::endl;
		
		// Determine the celestial coordinates for ref0
		pix2wcs(referenceWCS, ref0[0], ref0[1], &crval[0], &crval[1]);

		// Coordinate axis flipped, need to make adjustment
		// TODO: Find sample data to test this with
		if (referenceWCS->coorflip == 1)
			crval = flip*crval;
		
		// Compute offset in x direction
		Vector2d ref_x = epoToFits(crpix + v);
		Vector2d xieta_x = xi_eta(ref_x);
		
		// Compute offset in y direction
		Vector2d ref_y = epoToFits(crpix + flip*v);
		Vector2d xieta_y = xi_eta(ref_y);
		
		// Compute the cd matrix
		cdmatrix(0) = (xieta_x(0) - xieta_0(0)) / v(0);
		cdmatrix(1) = (xieta_0(0) - xieta_y(0)) / v(0);
		cdmatrix(2) = (xieta_x(1) - xieta_0(1)) / v(0);
		cdmatrix(3) = (xieta_0(1) - xieta_y(1)) / v(0);
		
		// Calculate the scale in units of arcseconds
	//	scale = 3600 * sqrt(pow(xieta_y(0) - xieta_0(0), 2) + pow(xieta_y(1) - xieta_0(1), 2)) / v(0);
		scale = sqrt(pow(xieta_y(0) - xieta_0(0), 2) + pow(xieta_y(1) - xieta_0(1), 2)) / v(0);
		
		// Calculate the orientation
		orientation = atan2(xieta_y(0) - xieta_0(0), -(xieta_y(1) - xieta_0(1))) * (180. / M_PI);
		
		// EPO WCS calculated!!
		epoWCS = true;
		
		// Broadcast computation
		emit wcs();
		
//		std::cout.precision(15);
//		qDebug() << "*** EPO WCS ***";
//		std::cout << "Reference Pixel:\t" << crpix << std::endl;
//		std::cout << "Reference Value:\t" << crval << std::endl;
//		std::cout << "CD Matrix:\t" << cdmatrix << std::endl;
//		std::cout << "Scale:\t" << scale << std::endl;
//		std::cout << "Orientation:\t" << orientation << std::endl;
		
		return;
	}
	
	epoWCS = false;
	emit nowcs();
}


// FIXME: Fix and make work with galatic coordinates
struct WorldCoor* ComputeWCS::initTargetWCS()
{
	qDebug() << "initTargetWCS()";
	struct WorldCoor *targetWCS;
	double *cd;
	cd = (double *) malloc(4 * sizeof(double));
	cd[0] = cdmatrix(0);
	cd[1] = cdmatrix(1);
	cd[2] = cdmatrix(2);
	cd[3] = cdmatrix(3);
	
	// TESTING: Create a FITS header to feed into wcsinit
	QString header = PinpointWCSUtils::createHeader(width, height, referenceWCS->equinox, referenceWCS->radecsys, crpix(0), crval(0), crpix(1), crval(1), cd);
	targetWCS = wcsinit(header.toStdString().c_str());
	
	/*
	targetWCS = wcskinit(width, height, "RA--", "DEC-",
						 crpix(0), crpix(1), crval(0), crval(1),
						 cd, NULL, NULL,
						 NULL, referenceWCS->equinox, NULL
	);
	*/

	return targetWCS;
}


// TODO: Try copying WCS over and only changing the reference pixel
struct WorldCoor* ComputeWCS::initTargetWCSII()
{
	qDebug() << "initTargetWCSII()";
	struct WorldCoor *targetWCS;
	
	// Determine the reference image's reference pixel from the frame of reference of the epo image
	crpix = fitsToEpo(referenceWCS->crpix[0], referenceWCS->crpix[1]);
	
	targetWCS = wcskinit(width, height, "RA--", "DEC-",
						 crpix(0), crpix(1), referenceWCS->crval[0], referenceWCS->crval[1],
						 referenceWCS->cd, referenceWCS->cdelt[0], referenceWCS->cdelt[1],
						 referenceWCS->rot, referenceWCS->equinox, referenceWCS->epoch
						 );

	return targetWCS;
}



Vector2d ComputeWCS::xi_eta(double xpix, double ypix)
{
	const double pix[2] = {xpix, ypix};
	double interworld[2];
	linrev(pix, &(referenceWCS->lin), interworld);
	
	Vector2d intermediate(interworld[0], interworld[1]);
	return intermediate;
}

Vector2d ComputeWCS::xi_eta(Vector2d pixel)
{
	const double pix[2] = {pixel(0), pixel(1)};
	double interworld[2];
	linrev(pix, &(referenceWCS->lin), interworld);
	
	Vector2d intermediate(interworld[0], interworld[1]);
	return intermediate;
}

void ComputeWCS::computeSums()
{
	// Dynamically initialize matrix and vectors
	initializeMatrixVectors(1);
	
	int ii;	
	if (degree == 1)
	{
		for (ii=0; ii < epoCoords->size(); ii++)
		{
			QPointF point1 = refCoords->at(ii);
			QPointF point2 = epoCoords->at(ii);
			
			// Account the FITS flip
			point1.setY(referenceWCS->nypix - point1.y());
			
			// Set the base
			basis << point2.x(), point2.y(), 1;
			
			// Generate matrix and vectors
			matrix += basis * basis.transpose();
			xvector += point1.x() * basis;
			yvector += point1.y() * basis;
		}
	}
	else if (degree == 2)
	{
		// Quadratic mapping
		for (ii=0; ii < epoCoords->size(); ii++)
		{
			QPointF point1 = refCoords->at(ii);
			QPointF point2 = epoCoords->at(ii);
			point1.setY(referenceWCS->nypix - point1.y());
			
			// Set the basis
			basis << 1, point2.x(), point2.y(), point2.x()*point2.y(), pow(point2.x(), 2), pow(point2.y(), 2);
			
			// Generate matrix and vectors
			matrix += basis * basis.transpose();
			xvector += point1.x() * basis;
			yvector += point1.y() * basis;	
		}
	}
	else if (degree == 3)
	{
		// Cubic mapping
		for (ii=0; ii < epoCoords->size(); ii++)
		{
			QPointF point1 = refCoords->at(ii);
			QPointF point2 = epoCoords->at(ii);
			point1.setY(referenceWCS->nypix - point1.y());
			
			// Set the base
			basis << pow(point2.x(), 3), pow(point2.y(), 3), pow(point2.x(), 2), pow(point2.y(), 2), point2.x(), point2.y(), 1;
			
			// Generate matrix and vectors
			matrix += basis * basis.transpose();
			xvector += point1.x() * basis;
			yvector += point1.y() * basis;
		}
	}
	
//	std::cout << "matrix:" << matrix << "\n" << std::endl;
//	std::cout << "x vector:" << xvector << "\n" << std::endl;
//	std::cout << "y vector:" << yvector << "\n" << std::endl;
}


void ComputeWCS::plateSolution()
{	
	// Solve the linear system
	matrix.lu().solve(xvector, &xcoeff);
	matrix.lu().solve(yvector, &ycoeff);
	
	mappingExists = true;
}


// TODO: Compute the residuals!!!
void ComputeWCS::computeResiduals()
{
	// Initialize some variables
	double sumn = 0;
	double sumx2 = 0;
	double sumy2 = 0;
	
	// Loop over the pairs stored in the data model
	int ii;
	for (ii=0; ii < epoCoords->size(); ii++)
	{
		// Store the coordinate pairs
		QPointF point1 = refCoords->at(ii);
		QPointF point2 = epoCoords->at(ii);
		point1.setY(referenceWCS->nypix - point1.y());
		
		// Map EPO coordinates to FITS coordinates
		Vector2d fit = epoToFits(&point2);
		
		// Compute residuals
		sumn += 1;
		sumx2 += pow(point1.x() - fit[0], 2);
		sumy2 += pow(point1.y() - fit[1], 2);
	}
	
	rms_x = sqrt(sumx2 / sumn);
	rms_y = sqrt(sumy2 / sumn);
	
	qDebug() << "RMS X:" << rms_x;
	qDebug() << "RMS Y:" << rms_y;
}

// TODO: Compute the mapped coordinates
QPointF ComputeWCS::fitsToEpo(QPointF *p)
{
	// TODO: Check that bd-af is nonzero
	QPointF epoCoord;
	float x0, y0;
	
	// Create matrix of coeffients
	Matrix2d m;
	m << xcoeff(0), xcoeff(1), ycoeff(0), ycoeff(1);
	
	// Take the inverse
	m = m.inverse();
	Vector3d xinverse;
	Vector3d yinverse;
	xinverse << m(0,0), m(0,1), xcoeff[2];
	yinverse << m(1,0), m(1,1), ycoeff[2];
	
	// Find the difference
	x0 = p->x() - xinverse[2];
	y0 = referenceWCS->nypix - p->y() - yinverse[2];
	
	epoCoord.setX(xinverse[0] * x0 + xinverse[1] * y0);
	epoCoord.setY(yinverse[0] * x0 + yinverse[1] * y0);

	return epoCoord;
}


Vector2d ComputeWCS::fitsToEpo(double x, double y)
{
	// TODO: Check that bd-af is nonzero
	Vector2d epoCoord;
	float x0, y0;
	
	// Create matrix of coeffients
	Matrix2d m;
	m << xcoeff(0), xcoeff(1), ycoeff(0), ycoeff(1);
	
	// Take the inverse
	m = m.inverse();
	Vector3d xinverse;
	Vector3d yinverse;
	xinverse << m(0,0), m(0,1), xcoeff[2];
	yinverse << m(1,0), m(1,1), ycoeff[2];
	
	// Find the difference
	x0 = x - xinverse[2];
	y0 = referenceWCS->nypix - y - yinverse[2];
	
	epoCoord << xinverse[0] * x0 + xinverse[1] * y0, yinverse[0] * x0 + yinverse[1] * y0;	
	return epoCoord;
}



Vector2d ComputeWCS::epoToFits(QPointF *p)
{
	Vector2d coordinate;
		
	if (degree == 1)
	{
		// x_r = a*x + b*y + c
		// y_r = d*x + f*b + g
		coordinate(0) = xcoeff[0] * p->x() + xcoeff[1] * p->y() + xcoeff[2];
		coordinate(1) = ycoeff[0] * p->x() + ycoeff[1] * p->y() + ycoeff[2];
	}
	else if (degree == 2)
	{
		coordinate(0) = xcoeff[0] + xcoeff[1] * p->x() + xcoeff[2] * p->y() + xcoeff[3] * p->x() * p->y() + xcoeff[4] * pow(p->x(), 2) + xcoeff[5] * pow(p->y(), 2);
		coordinate(1) = ycoeff[0] + ycoeff[1] * p->x() + ycoeff[2] * p->y() + ycoeff[3] * p->x() * p->y() + ycoeff[4] * pow(p->x(), 2) + ycoeff[5] * pow(p->y(), 2);
	} 
	
	return coordinate;
}


Vector2d ComputeWCS::epoToFits(double x, double y)
{
	Vector2d coordinate;
	
	if (degree == 1)
	{
		coordinate(0) = xcoeff[0] * x + xcoeff[1] * y + xcoeff[2];
		coordinate(1) = ycoeff[0] * x + ycoeff[1] * y + ycoeff[2];
	}
	else if (degree == 2)
	{
		coordinate(0) = xcoeff[0] + xcoeff[1] * x + xcoeff[2] * y + xcoeff[3] * x * y + xcoeff[4] * pow(x, 2) + xcoeff[5] * pow(y, 2);
		coordinate(1) = ycoeff[0] + ycoeff[1] * x + ycoeff[2] * y + ycoeff[3] * x * y + ycoeff[4] * pow(x, 2) + ycoeff[5] * pow(y, 2);
	}
	
	return coordinate;
}

Vector2d ComputeWCS::epoToFits(Vector2d p)
{
	Vector2d coordinate;
	
	if (degree == 1)
	{
		coordinate(0) = xcoeff[0] * p[0] + xcoeff[1] * p[1] + xcoeff[2];
		coordinate(1) = ycoeff[0] * p[0] + ycoeff[1] * p[1] + ycoeff[2];
	}
	else if (degree == 2)
	{
		coordinate(0) = xcoeff[0] + xcoeff[1] * p[0] + xcoeff[2] * p[1] + xcoeff[3] * p[0] * p[1] + xcoeff[4] * pow(p[0], 2) + xcoeff[5] * pow(p[1], 2);
		coordinate(1) = ycoeff[0] + ycoeff[1] * p[0] + ycoeff[2] * p[1] + ycoeff[3] * p[0] * p[1] + ycoeff[4] * pow(p[0], 2) + ycoeff[5] * pow(p[1], 2);
	}
	
	return coordinate;
}
