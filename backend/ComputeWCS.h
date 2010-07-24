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

USING_PART_OF_NAMESPACE_EIGEN

class ComputeWCS
{
public:
	// Methods
	ComputeWCS(QList< QPair<QPointF, QPointF> > *m);
	~ComputeWCS();
	void initializeMatrixVectors(int degree);
	void plateSolution();
	// Attributes
	
private:
	// Methods
	void computeTargetWCS();
	void xi_eta();
	void computeSums();
	void computeResiduals();
	Vector2d fitsToEpo(QPointF *p);
	Vector2d epoToFits(QPointF *p);

	// Attributes
	QList< QPair<QPointF, QPointF> > *dataModel;
	Matrix3d A;
	Vector3d xcoeff;
	Vector3d ycoeff;
	Vector3d xvector;
	Vector3d yvector;
	double rms_x, rms_y;
	
	MatrixXd matrix_test;
	VectorXd xcoeff_test;
	VectorXd ycoeff_test;
	VectorXd xvector_test;
	VectorXd yvector_test;
};

#endif
