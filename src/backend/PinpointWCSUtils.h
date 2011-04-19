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

#ifndef PINPOINTWCS_UTILS_H
#define PINPOINTWCS_UTILS_H

#include "wcs.h"

namespace PinpointWCSUtils
{
	float computeQuantile(float *arr, long arraysize, double q);
	
	// Functions for centroid fitting
	bool cen3(float f0, float f1, float f2, float *xcen);
	bool cen3x3(float *image, float *xcen, float *ycen);
	
	// Functions for checking WCS
	void dumpWCS(struct WorldCoor *wcs);
};

#endif

