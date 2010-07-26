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


ComputeWCS::ComputeWCS(QList< QPair<QPointF, QPointF> > *m)
{
	dataModel = m;
}

ComputeWCS::~ComputeWCS()
{}

void ComputeWCS::initializeMatrixVectors(int d)
{
	degree = d;
	int size = 2*degree+1;
	
	matrix = MatrixXd::Zero(size, size);
	xcoeff = VectorXd::Zero(size);
	ycoeff = VectorXd::Zero(size);
	xvector = VectorXd::Zero(size);
	yvector = VectorXd::Zero(size);
}

void ComputeWCS::computeTargetWCS()
{}

void ComputeWCS::xi_eta()
{}

void ComputeWCS::computeSums()
{
	// Dynamically initialize matrix and vectors
	initializeMatrixVectors(2);
	
	int ii;	
	if (degree == 1)
	{
		// Linear mapping
		for (ii=0; ii < dataModel->size(); ii++)
		{
			QPointF point1 = dataModel->at(ii).first; 
			QPointF point2 = dataModel->at(ii).second;
			
			matrix(0, 0) += pow(point2.x(), 2);
			matrix(0, 1) = matrix(1, 0) += point2.x() * point2.y();
			matrix(0, 2) = matrix(2, 0) += point2.x();
			matrix(1, 1) += pow(point2.y(), 2);
			matrix(1, 2) = matrix(2, 1) += point2.y();
			matrix(2, 2) += 1;
			
			xvector(0) += point1.x() * point2.x();
			xvector(1) += point1.x() * point2.y();
			xvector(2) += point1.x();
			
			yvector(0) += point1.y() * point2.x();
			yvector(1) += point1.y() * point2.y();
			yvector(2) += point1.y();
		}
		
	}
	else if (degree == 2)
	{
		// Quadratic mapping
		for (ii=0; ii < dataModel->size(); ii++)
		{
			QPointF point1 = dataModel->at(ii).first; 
			QPointF point2 = dataModel->at(ii).second;
			
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
	
		}
	}
	else if (degree == 3)
	{
		// Cubic mapping
	}
	
	
	/*
	// Initialize variable for the matrix elements
	float a_elem[5] = {0.0};
	float x_elem[3] = {0.0};
	float y_elem[3] = {0.0};
	
	// Loop over all the elements in the data model (QList)
	int ii;
	for (ii=0; ii < dataModel->size(); ii++)
	{
		QPointF point1 = dataModel->at(ii).first; 
		QPointF point2 = dataModel->at(ii).second;
		
		a_elem[0] += pow(point2.x(), 2);
		a_elem[1] += point2.x() * point2.y();
		a_elem[2] += point2.x();
		a_elem[3] += pow(point2.y(), 2);
		a_elem[4] += point2.y();
		
		x_elem[0] += point1.x() * point2.x();
		x_elem[1] += point1.x() * point2.y();
		x_elem[2] += point1.x();
		
		y_elem[0] += point1.y() * point2.x();
		y_elem[1] += point1.y() * point2.y();
		y_elem[2] += point1.y();
		
	}
	
	matrix << a_elem[0], a_elem[1], a_elem[2], a_elem[1], a_elem[3], a_elem[4], a_elem[2], a_elem[4], dataModel->size();
	xvector << x_elem[0], x_elem[1], x_elem[2];
	yvector << y_elem[0], y_elem[1], y_elem[2];
	 */
//	std::cout << A << std::endl;
//	std::cout << xvector << std::endl;
//	std::cout << yvector << std::endl;
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

void ComputeWCS::plateSolution()
{
	// Generate the matrix and vectors
	computeSums();
	
	// Solve the linear system
	matrix.lu().solve(xvector, &xcoeff);
	matrix.lu().solve(yvector, &ycoeff);
	
	// Print to standard output
	std::cout << xcoeff << std::endl;
	std::cout << ycoeff << std::endl;
	
	// Compute residuals
	computeResiduals();
}

Vector2d ComputeWCS::fitsToEpo(QPointF *p)
{}


Vector2d ComputeWCS::epoToFits(QPointF *p)
{
	Vector2d coordinate;
	if (degree = 1)
	{
		coordinate(0) = xcoeff[0] * p->x() + xcoeff[1] * p->y() + xcoeff[2];
		coordinate(1) = ycoeff[0] * p->x() + ycoeff[1] * p->y() + ycoeff[2];
	}
	else
	{
		coordinate(0) = xcoeff[0] * pow(p->x(), 2) + xcoeff[1] * pow(p->y(), 2) + xcoeff[2] * p->x() + xcoeff[3] * p->y() + xcoeff[4];
		coordinate(1) = ycoeff[0] * pow(p->x(), 2) + ycoeff[1] * pow(p->y(), 2) + ycoeff[2] * p->x() + ycoeff[3] * p->y() + ycoeff[4];
	}
	
	return coordinate;
}
