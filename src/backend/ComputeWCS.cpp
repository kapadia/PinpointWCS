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
	rms_x = NULL;
	rms_y = NULL;
	center_x = NULL;
	center_y = NULL;
	centerRA = NULL;
	centerDec = NULL;
	
	// Flip matrix
	flip << 0, 1, 1, 0;
	
	// Vector to be used to compute offsets
	v << 1000.0, 0;
}

ComputeWCS::~ComputeWCS()
{}

void ComputeWCS::initializeMatrixVectors()
{
	int size = 3;
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
	int numPoints;
	if (epoCoords->size() >= 3)
	{
		// Determine the number of points to use
		if (epoCoords->last() == QPointF(-1, -1))
		{
			if (epoCoords->size() == 3)
			{
				epoWCS = false;
				emit nowcs();
				return;
			}
			numPoints = refCoords->size()-1;
		}
		else 
			numPoints = refCoords->size();

		// Compute matrix and vectors
		computeSums(numPoints);
		
		// Solve matrix equation for mapping
		plateSolution();
		
		// Compute residuals
		computeResiduals(numPoints);
		
		// Declare the CRPIX for the EPO image to be the center pixel
		crpix = fitsToEpo(referenceWCS->xrefpix, referenceWCS->yrefpix);
		
		// Determine corresponding pixel in the FITS image in QGraphicsScene space
		Vector2d ref0;
		ref0 << referenceWCS->xrefpix, referenceWCS->yrefpix;
		std::cout << "ref0:\t" << ref0 << std::endl;
		
		// Transform from QGraphicsScene pixels to FITS pixels
		ref0 = gsPix2fitsPix(ref0);
		
		Vector2d xieta_0 = xi_eta(ref0);
		std::cout << "xieta_0:\t" << xieta_0 << std::endl;
		
		// Determine the celestial coordinates for ref0		
		pix2wcs(referenceWCS, ref0[0], ref0[1], &crval[0], &crval[1]);

		// Coordinate axis flipped, need to make adjustment
		// TODO: Find sample data to test this with
		if (referenceWCS->coorflip == 1)
			crval = flip*crval;
		
		// Compute offset in x direction
		Vector2d ref_x = gsPix2fitsPix( epoToFits(crpix + v) );
		Vector2d xieta_x = xi_eta(ref_x);
		
		// Compute offset in y direction
		Vector2d ref_y = gsPix2fitsPix( epoToFits(crpix + flip*v) );
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
		
		// TODO: Store the coordinates for the center of the image
		
		// EPO WCS calculated!!
		epoWCS = true;
		
		// Broadcast computation
		emit wcs();
		return;
	}
	
	epoWCS = false;
	emit nowcs();
}


struct WorldCoor* ComputeWCS::initTargetWCS()
{
	struct WorldCoor *targetWCS;
	double *cd;
	cd = (double *) malloc(4 * sizeof(double));
	cd[0] = cdmatrix(0);
	cd[1] = cdmatrix(1);
	cd[2] = cdmatrix(2);
	cd[3] = cdmatrix(3);
	
	// Determine the reference image's reference pixel from the frame of reference of the EPO image
	crpix = fitsToEpo(referenceWCS->crpix[0], referenceWCS->crpix[1]);

	targetWCS = wcskinit(width, height, "RA---TAN", "DEC--TAN",
						 crpix(0), height-crpix(1)+1, referenceWCS->crval[0], referenceWCS->crval[1],
						 cd, NULL, NULL,
						 NULL, referenceWCS->equinox, referenceWCS->epoch
	);
	
	free(cd);
	
	// Set output coordinates
	wcsoutinit(targetWCS, "FK5");
	
	// Calculate the coordinates for the center of the image (for the folks at STScI)
	// Center pixel transforms on to itself (no need to apply QGraphicsScene pixels -> FITS pixels transformation!)
	center_x = width/2. + 0.5;
	center_y = height - height/2. + 0.5;
	pix2wcs(targetWCS, center_x, center_y, &centerRA, &centerDec);
	center_x = width/2.;
	center_y = height/2.;
	std::cout << "Dimensions\t" << width/2. << "\t" << height/2. << std::endl;
	printf("Center Pixel:\t%.11f\t%.11f\n", centerRA, centerDec);
	
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


void ComputeWCS::computeSums(int numPoints)
{
	// Dynamically initialize matrix and vectors
	initializeMatrixVectors();
	
	for (int ii=0; ii < numPoints; ii++)
	{
		QPointF point1 = refCoords->at(ii);
		QPointF point2 = epoCoords->at(ii);
		
		// Set the base
		basis << point2.x(), point2.y(), 1;
		
		// Generate matrix and vectors
		matrix += basis * basis.transpose();
		xvector += point1.x() * basis;
		yvector += point1.y() * basis;
	}
}


void ComputeWCS::plateSolution()
{	
	// Solve the linear system
	xcoeff = matrix.lu().solve(xvector);
	ycoeff = matrix.lu().solve(yvector);
	
	mappingExists = true;
}


void ComputeWCS::computeResiduals(int numPoints)
{
	// Initialize some variables
	double sumn = 0;
	double sumx2 = 0;
	double sumy2 = 0;
	
	// Loop over the pairs stored in the data model
	for (int ii=0; ii < numPoints; ii++)
	{
		// Store the coordinate pairs
		QPointF point1 = refCoords->at(ii);
		QPointF point2 = epoCoords->at(ii);
		
		// Map EPO coordinates to FITS coordinates
		Vector2d fit = epoToFits(&point2);
		
		// Compute residuals
		sumn += 1;
		sumx2 += pow(point1.x() - fit[0], 2);
		sumy2 += pow(point1.y() - fit[1], 2);
	}
	
	rms_x = sqrt(sumx2 / sumn);
	rms_y = sqrt(sumy2 / sumn);
}


QPointF ComputeWCS::fitsToEpo(QPointF *p)
{
	// TODO: Check that bd-af is nonzero
	QPointF epoCoord;
	float x0, y0;
	
	// Create matrix of coeffients
	Matrix2d m;
	m << xcoeff(0), xcoeff(1), ycoeff(0), ycoeff(1);
	
	// Take the inverse
	m = m.inverse().eval();
	Vector3d xinverse;
	Vector3d yinverse;
	xinverse << m(0,0), m(0,1), xcoeff[2];
	yinverse << m(1,0), m(1,1), ycoeff[2];
	
	// Find the difference
	x0 = p->x() - xinverse[2];
	y0 = p->y() - yinverse[2];
	
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
	m = m.inverse().eval();
	Vector3d xinverse;
	Vector3d yinverse;
	xinverse << m(0,0), m(0,1), xcoeff[2];
	yinverse << m(1,0), m(1,1), ycoeff[2];
	
	// Find the difference
	x0 = x - xinverse[2];
	y0 = y - yinverse[2];
	
	epoCoord << xinverse[0] * x0 + xinverse[1] * y0, yinverse[0] * x0 + yinverse[1] * y0;	
	return epoCoord;
}



Vector2d ComputeWCS::epoToFits(QPointF *p)
{
	Vector2d coordinate;

	// x_r = a*x + b*y + c
	// y_r = d*x + f*b + g
	coordinate(0) = xcoeff[0] * p->x() + xcoeff[1] * p->y() + xcoeff[2];
	coordinate(1) = ycoeff[0] * p->x() + ycoeff[1] * p->y() + ycoeff[2];
	
	return coordinate;
}


Vector2d ComputeWCS::epoToFits(double x, double y)
{
	Vector2d coordinate;
	
	coordinate(0) = xcoeff[0] * x + xcoeff[1] * y + xcoeff[2];
	coordinate(1) = ycoeff[0] * x + ycoeff[1] * y + ycoeff[2];
	
	return coordinate;
}

Vector2d ComputeWCS::epoToFits(Vector2d p)
{
	Vector2d coordinate;
	
	coordinate(0) = xcoeff[0] * p[0] + xcoeff[1] * p[1] + xcoeff[2];
	coordinate(1) = ycoeff[0] * p[0] + ycoeff[1] * p[1] + ycoeff[2];
	
	return coordinate;
}


Vector2d ComputeWCS::gsPix2fitsPix(Vector2d p)
{
	Vector2d coordinate;
	
	// Transform from QGraphicsScene pixels to FITS pixels
	coordinate(0) = p[0];
	coordinate(1) = referenceWCS->nypix - p[1];

	return coordinate;
}