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

#define _USE_MATH_DEFINES
USING_PART_OF_NAMESPACE_EIGEN

class ComputeWCS : public QObject
{
	
	Q_OBJECT
	
public:
	// Methods
	ComputeWCS(QList<QPointF> *ref, QList<QPointF> *epo, struct WorldCoor *refWCS, double w, double h);
	~ComputeWCS();
	struct WorldCoor* initTargetWCS();
	
	// Public Attributes
	bool epoWCS;
	
	// EPO Attributes
	double width;
	double height;
	Vector2d crpix;
	Vector2d crval;
	Matrix2d cdmatrix;
	double scale;
	double orientation;
	
public slots:
	void computeTargetWCS();
	
signals:
	void wcs();
	void nowcs();
	
private:
	// Methods
	void initializeMatrixVectors(int d);
	void plateSolution();
	Vector2d xi_eta(double xpix, double ypix);
	Vector2d xi_eta(Vector2d pixel);
	void computeSums();
	void computeResiduals();
	Vector2d fitsToEpo(QPointF *p);
	Vector2d epoToFits(QPointF *p);
	Vector2d epoToFits(double x, double y);
	Vector2d epoToFits(Vector2d p);

	// Attributes
	QList<QPointF> *refCoords;
	QList<QPointF> *epoCoords;

	int degree;
	MatrixXd matrix;
	VectorXd xvector;
	VectorXd yvector;
	VectorXd xcoeff;
	VectorXd ycoeff;
	VectorXd basis;
	double rms_x, rms_y;
	struct WorldCoor *referenceWCS;
		
	// Common calculation variables
	Matrix2d flip;
	Vector2d v;
};

#endif
