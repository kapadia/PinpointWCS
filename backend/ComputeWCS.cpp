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


ComputeWCS::ComputeWCS(QList< QPair<QPointF, QPointF> > *m, struct WorldCoor *refWCS, double w, double h)
{
	dataModel = m;
	referenceWCS = refWCS;
	width = w;
	height = h;
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
	// Compute matrix and vectors
	computeSums();
	
	// Solve matrix equation for mapping
	plateSolution();
	
	// Compute residuals
	computeResiduals();
	
	// Declare the CRPIX for the EPO image to be the center pixel
	// Maybe this can eventually be set by the user
	crpix1 = width / 2.;
	crpix2 = height / 2.;
	
	// Determine corresponding pixel in the FITS image
	Vector2d ref0 = epoToFits(crpix1, crpix2);
	
	// Determine the celestial coordinates for ref0
	crval = (double*) malloc( 2*sizeof(double) );
	
	pix2wcs(referenceWCS, ref0(0), ref0(1), &crval[0], &crval[1]);
	
	
}

Vector2d ComputeWCS::xi_eta(double xpix, double ypix)
{
	// Initialize some variables
	Vector2d pixel(xpix, ypix);
	Vector2d crpix(referenceWCS->xrefpix, referenceWCS->yrefpix);
	Vector2d cdelt(referenceWCS->cdelt[0], referenceWCS->cdelt[1]);
	Matrix2d pc;
	pc << referenceWCS->pc[0], referenceWCS->pc[1], referenceWCS->pc[2], referenceWCS->pc[3];
	
	// Compute intermediate pixel coordinates
	Vector2d intermediate = pc * (pixel - crpix);
	
	// Compute intermediate world coordinates
	intermediate = intermediate.cwise() * cdelt;
	
	// Convert degrees to radians
	intermediate = (180. / M_PI) * intermediate;
	
	if (referenceWCS->coorflip == 1)
	{
		// Coordinate axes flipped, need to make some adjustments
		Matrix2d f1;
		f1 << 0, 1, 1, 0;
		return f1*intermediate;
	}
	std::cout << intermediate;
	
	return intermediate;
}

void ComputeWCS::computeSums()
{
	// Dynamically initialize matrix and vectors
	initializeMatrixVectors(1);
	
	int ii;	
	if (degree == 1)
	{
		for (ii=0; ii < dataModel->size(); ii++)
		{
			QPointF point1 = dataModel->at(ii).first;
			QPointF point2 = dataModel->at(ii).second;
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
		for (ii=0; ii < dataModel->size(); ii++)
		{
			QPointF point1 = dataModel->at(ii).first;
			QPointF point2 = dataModel->at(ii).second;
			point1.setY(referenceWCS->nypix - point1.y());
			
			// Set the base
			basis << 1, point2.x(), point2.y(), point2.x()*point2.y(), pow(point2.x(), 2), pow(point2.y(), 2);
//			basis << pow(point2.x(), 2), pow(point2.y(), 2), point2.x() * point2.y(), point2.x(), point2.y(), 1;
			
			// Generate matrix and vectors
			matrix += basis * basis.transpose();
			xvector += point1.x() * basis;
			yvector += point1.y() * basis;
			
			/*
			matrix(0, 0) += pow(point2.x(), 4);
			matrix(0, 1) = matrix(1, 0) += pow(point2.y(), 2) * pow(point2.x(), 2);
			matrix(0, 2) = matrix(2, 0) += pow(point2.x(), 3);
			matrix(0, 3) = matrix(3, 0) += point2.y() * pow(point2.x(), 2);
			matrix(0, 4) = matrix(4, 0) += pow(point2.x(), 2);
			
			matrix(1, 1) += pow(point2.y(), 4);
			matrix(1, 2) = matrix(2, 1) += point2.x() * pow(point2.y(), 2);
			matrix(1, 3) = matrix(3, 1) += pow(point2.y(), 3);
			matrix(1, 4) = matrix(4, 1) += pow(point2.y(), 2);
			
			matrix(2, 2) += pow(point2.x(), 2);
			matrix(2, 3) = matrix(3, 2) += point2.y() * point2.x();
			matrix(2, 4) = matrix(4, 2) += point2.x();
			
			matrix(3, 3) += pow(point2.y(), 2);
			matrix(3, 4) = matrix(4, 3) += point2.y();
			
			matrix(4, 4) += 1;
			
			xvector(0) += point1.x() * pow(point2.x(), 2);
			xvector(1) += point1.x() * pow(point2.y(), 2);
			xvector(2) += point1.x() * point2.x();
			xvector(3) += point1.x() * point2.y();
			xvector(4) += point1.x();
			
			yvector(0) += point1.y() * pow(point2.x(), 2);
			yvector(1) += point1.y() * pow(point2.y(), 2);
			yvector(2) += point1.y() * point2.x();
			yvector(3) += point1.y() * point2.y();
			yvector(4) += point1.y();
			 */
	
		}
	}
	else if (degree == 3)
	{
		// Cubic mapping
		for (ii=0; ii < dataModel->size(); ii++)
		{
			QPointF point1 = dataModel->at(ii).first;
			QPointF point2 = dataModel->at(ii).second;
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
	
	// Print to standard output
//	std::cout << "xcoeff:\n" << xcoeff << "\n" << std::endl;
//	std::cout << "ycoeff:\n" << ycoeff << "\n" << std::endl;
}


void ComputeWCS::computeResiduals()
{
	// Initialize some variables
	double sumn = 0;
	double sumx2 = 0;
	double sumy2 = 0;
	
	// Loop over the pairs stored in the data model
	int ii;
	for (ii=0; ii < dataModel->size(); ii++)
	{
		// Store the coordinate pairs
		QPointF point1 = dataModel->at(ii).first;
		QPointF point2 = dataModel->at(ii).second;
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


Vector2d ComputeWCS::fitsToEpo(QPointF *p)
{
	return Vector2d(p->x(), p->y());
}


Vector2d ComputeWCS::epoToFits(QPointF *p)
{
	Vector2d coordinate;
		
	if (degree == 1)
	{
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