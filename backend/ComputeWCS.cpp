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
#include "ComputeWCS.h"


ComputeWCS::ComputeWCS(QList< QPair<QPointF, QPointF> > *m)
{
	dataModel = m;
}

ComputeWCS::~ComputeWCS()
{}

void ComputeWCS::computeTargetWCS()
{}

void ComputeWCS::xi_eta()
{}

void ComputeWCS::computeSums()
{
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
	
	A << a_elem[0], a_elem[1], a_elem[2], a_elem[1], a_elem[3], a_elem[4], a_elem[2], a_elem[4], dataModel->size();
	xvector << x_elem[0], x_elem[1], x_elem[2];
	yvector << y_elem[0], y_elem[1], y_elem[2];
	
//	std::cout << A << std::endl;
//	std::cout << xvector << std::endl;
//	std::cout << yvector << std::endl;
}

void ComputeWCS::computeResiduals()
{}

void ComputeWCS::plateSolution()
{
	// Generate the matrix and vectors
	computeSums();
	
	// Solve the linear system
	A.lu().solve(xvector, &xcoeff);
	A.lu().solve(yvector, &ycoeff);
	
	// Print to standard output
	std::cout << xcoeff << std::endl;
	std::cout << ycoeff << std::endl;
}

void ComputeWCS::referenceToTarget()
{}

void ComputeWCS::targetToReference()
{}
