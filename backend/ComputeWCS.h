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

#ifndef COMPUTEWCS_H
#define COMPUTEWCS_H

#include <QList>
#include <QPair>
#include <QPointF>
#include <Eigen/Core>
#include "wcs.h"

USING_PART_OF_NAMESPACE_EIGEN

class ComputeWCS
{
public:
	// Methods
	ComputeWCS(QList< QPair<QPointF, QPointF> > *m, struct WorldCoor *refWCS);
	~ComputeWCS();
	
	void computeTargetWCS();
	
private:
	// Methods
	void initializeMatrixVectors(int d);
	void plateSolution();
	void xi_eta();
	void computeSums();
	void computeResiduals();
	Vector2d fitsToEpo(QPointF *p);
	Vector2d epoToFits(QPointF *p);

	// Attributes
	QList< QPair<QPointF, QPointF> > *dataModel;
	int degree;
	MatrixXd matrix;
	VectorXd xvector;
	VectorXd yvector;
	VectorXd xcoeff;
	VectorXd ycoeff;
	VectorXd basis;
	double rms_x, rms_y;
	struct WorldCoor *referenceWCS;
	struct WorldCoor *targetWCS;
};

#endif
