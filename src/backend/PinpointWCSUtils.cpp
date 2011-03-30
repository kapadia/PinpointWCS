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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>

#include <QString>

#include "PinpointWCSUtils.h"

namespace PinpointWCSUtils {
	
	float computeQuantile(float *arr, long arraysize, double q)
	{
		std::cout << "Determining Quantile ...\n";
		long length = arraysize;
		float dd;
		double dq;
		int i, s1len, s2len, randIndex;
		int num_below = 0;
		int iter = 0;
		float* D;
		float* s1;
		float* s2;
		float eps = 1. / length;
		
		// Initialize a random number
		srand(time(NULL));
		
		// Copy data to local variable
		D = (float *) malloc(length * sizeof(float));
		memcpy(D, arr, length * sizeof(float));
		
		while (true) {
			
			// Choose element from array
			randIndex = rand() % length;
			dd = D[randIndex];
			s1len = 0;
			s2len = 0;
			
			// Allot memory to the two arrays created
			s1 = (float *) malloc((length) * sizeof(float));
			s2 = (float *) malloc((length) * sizeof(float));
			
			// Begin comparison
			for (i=0; i<length; i++)
			{
				if (D[i] < dd)
				{
					s1[s1len] = D[i];
					s1len++;
				}
				else
				{
					s2[s2len] = D[i];
					s2len++;
				}
			}
			
			// Reallocate memory to both arrays
			s1 = (float *) realloc(s1, s1len * sizeof(float));
			s2 = (float *) realloc(s2, s2len * sizeof(float));
			
			// Check length of s1
			if (s1len == 0)
				return dd;
			
			// Determine the quantile of d
			dq = (double) (s1len + num_below) / arraysize;
			
			// Choose the appropriate array
			if (dq > q - eps and dq < q + eps)
				return dd;
			else if (dq < q)
			{
				// Choose s2
				D = (float *) realloc(D, s2len * sizeof(float));
				memcpy(D, s2, s2len * sizeof(float));
				length = s2len;
				num_below = num_below + s1len;
			}
			else
			{
				// Choose s1
				D = (float *) realloc(D, s1len * sizeof(float));
				memcpy(D, s1, s1len * sizeof(float));
				length = s1len;				
			}

			// Deallocate s1 and s2
			free(s1);
			free(s2);

//			std::cout << "dd = " << dd << "\n";
//			std::cout << "length = " << length << "\n";
//			std::cout << "dq = " << dq << "\n";
//			std::cout << "iter = " << iter << "\n\n";
			
			iter++;
		}  // while
		
	} // computeQuantile function
	
	
	bool cen3(float f0, float f1, float f2, float *xcen)
	{
		float a, b;
		
		a = 0.5 * (f2 - 2*f1 + f0);
		if (a == 0.0)
			return false;
		
		b = f1 - a - f0;
		*xcen = -0.5 * b / a;
		if ((*xcen < 0.0) || (*xcen > 2.0))
			return false;
		
		return true;
	} // cen3 function
	
	
	bool cen3x3(float *image, float *xcen, float *ycen)
	{
		float mx0, mx1, mx2;
		float my0, my1, my2;
		float bx, by, mx , my;
		int badcen = 0;
		
		badcen += cen3(image[0 + 3*0], image[1 + 3*0], image[2 + 3*0], &mx0);
		badcen += cen3(image[0 + 3*1], image[1 + 3*1], image[2 + 3*1], &mx1);
		badcen += cen3(image[0 + 3*2], image[1 + 3*2], image[2 + 3*2], &mx2);
		
		badcen += cen3(image[0 + 3*0], image[0 + 3*1], image[0 + 3*2], &my0);
		badcen += cen3(image[1 + 3*0], image[1 + 3*1], image[1 + 3*2], &my1);
		badcen += cen3(image[2 + 3*0], image[2 + 3*1], image[2 + 3*2], &my2);
		
		/* are we not okay? */
		if (badcen != 6) {
			return false;
		}
		
		/* x = (y-1) mx + bx */
		bx = (mx0 + mx1 + mx2) / 3.;
		mx = (mx2 - mx0) / 2.;
		
		/* y = (x-1) my + by */
		by = (my0 + my1 + my2) / 3.;
		my = (my2 - my0) / 2.;
		
		/* find intersection */
		(*xcen) = (mx * (by - my - 1.) + bx) / (1. + mx * my);
		(*ycen) = ((*xcen) - 1.) * my + by;
		
		printf("%f\n", *xcen);
		printf("%f\n", *ycen);
		
		/* check that we are in the box */
		if (((*xcen) < 0.0) || ((*xcen) > 2.0) ||
			((*ycen) < 0.0) || ((*ycen) > 2.0)){
			return false;
		}
		
        /* check for nan's and inf's */
		if (!isnormal(*xcen) || !isnormal(*ycen))
			return false;
		
		return true;
	} // cen3x3

	void dumpWCS(struct WorldCoor *wcs)
	{
		std::cout << "Dumping WCS ---------- ---------- ---------- ---------- ---------- ----------" << std::endl;
		std::cout << "CRVAL1:\t" << wcs->xref << std::endl;
		std::cout << "CRVAL2:\t" << wcs->yref << std::endl;
		std::cout << "CRPIX1:\t" << wcs->xrefpix << std::endl;
		std::cout << "CRPIX2:\t" << wcs->yrefpix << std::endl;
		
		std::cout << "CDELT1:\t" << wcs->xinc << std::endl;
		std::cout << "CDELT2:\t" << wcs->yinc << std::endl;
		std::cout << "CROTA2:\t" << wcs->rot << std::endl;
		std::cout << "CD1_1:\t" << wcs->cd[0] << std::endl;
		std::cout << "CD1_2:\t" << wcs->cd[1] << std::endl;
		std::cout << "CD2_1:\t" << wcs->cd[2] << std::endl;
		std::cout << "CD2_2:\t" << wcs->cd[3] << std::endl;
		std::cout << "EPOCH:\t" << wcs->equinox << std::endl;
		
		std::cout << "DATE-OBS:\t" << wcs->epoch << std::endl;
		std::cout << "NAXIS1:\t" << wcs->nxpix << std::endl;
		std::cout << "NAXIS2:\t" << wcs->nypix << std::endl;
		std::cout << "NAXIS:\t" << wcs->naxes << std::endl;
		std::cout << "CTYPE1:\t" << wcs->c1type << std::endl;
		
		std::cout << "CTYPE2:\t" << wcs->c2type << std::endl;
		std::cout << "CUNITn:\t" << wcs->units << std::endl;
		std::cout << "RADECSYS:\t" << wcs->radecsys << std::endl;
		std::cout << " ---------- ---------- ---------- ---------- ---------- ---------- ----------" << std::endl;
		
	} // dumpWCS
	
	QString createHeader(int width, int height, double equinox, char* radecsys, double crpix1, 
						 double crval1, double crpix2, double crval2, double cd[4])
	{
		QString header;
		header.sprintf(
					   "SIMPLE  =                    T / file does conform to FITS standard             "
					   "BITPIX  =                    8 / number of bits per data pixel                  "
					   "NAXIS   =                    2 / number of data axes                            "
					   "NAXIS1  =                 %d / length of data axis 1                          "
					   "NAXIS2  =                 %d / length of data axis 2                          "
					   "EXTEND  =                    T / FITS dataset may contain extensions            "
					   "XTENSION= 'IMAGE   '                                                            "
					   "ORIGIN  = 'PinpointWCS by the Chandra X-ray Center'                             "
					   "WCSAXES =                    2                                                  "
					   "WCSNAME = 'Primary WCS'                                                         "
					   "EQUINOX = '%.1f  '                                                            "
					   "RADESYS = '%s     '                                                            "
					   "CTYPE1  = '%s'                                                            "
					   "CRPIX1  = '%.11f'                                                    "
					   "CRVAL1  = '%.11f'                                                     "
					   "CUNIT1  = '%s     '                                                            "
					   "CTYPE2  = '%s'                                                            "
					   "CRPIX2  = '%.11f'                                                     "
					   "CRVAL2  = '%.11f'                                                      "
					   "CUNIT2  = '%s     '                                                            "
					   "CD1_1   = '%.11f'                                                       "
					   "CD1_2   = '%.11f'                                                       "
					   "CD2_1   = '%.11f'                                                       "
					   "CD2_2   = '%.11f'                                                      ",
					   width, // NAXIS1
					   height, // NAXIS2
					   equinox, // EQUINOX
					   radecsys, // RADESYS
					   "RA---TAN", // CTYPE1
					   crpix1, // CRPIX1
					   crval1, // CRVAL1
					   "deg", // CUNIT1
					   "DEC--TAN", // CTYPE2
					   crpix2, // CRPIX2
					   crval2, // CRVAL2
					   "deg", // CUNIT2
					   cd[0], // CD1_1
					   cd[1], // CD1_2
					   cd[2], // CD2_1
					   cd[3] // CD2_2
					   );
		return header;
	}
	
}  // namespace

